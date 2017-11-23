/* 
 * tsh - A tiny shell program with job control
 * 
 * 林涛 1600012773
 * 
 * There's nothing interesting to comment in this header.
 */
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <signal.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <errno.h>

/* for I/O */
#define DEF_MODE S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH

/* Misc manifest constants */
#define MAXLINE    1024   /* max line size */
#define MAXARGS     128   /* max args on a command line */
#define MAXJOBS      16   /* max jobs at any point in time */
#define MAXJID    1<<16   /* max job ID */

/* Job states */
#define UNDEF         0   /* undefined */
#define FG            1   /* running in foreground */
#define BG            2   /* running in background */
#define ST            3   /* stopped */

/* 
 * Jobs states: FG (foreground), BG (background), ST (stopped)
 * Job state transitions and enabling actions:
 *     FG -> ST  : ctrl-z
 *     ST -> FG  : fg command
 *     ST -> BG  : bg command
 *     BG -> FG  : fg command
 * At most 1 job can be in the FG state.
 */

/* Parsing states */
#define ST_NORMAL   0x0   /* next token is an argument */
#define ST_INFILE   0x1   /* next token is the input file */
#define ST_OUTFILE  0x2   /* next token is the output file */

/* Global variables */
extern char **environ;      /* defined in libc */
char prompt[] = "tsh> ";    /* command line prompt (DO NOT CHANGE) */
int verbose = 0;            /* if true, print additional output */
int nextjid = 1;            /* next job ID to allocate */
char sbuf[MAXLINE];         /* for composing sprintf messages */

struct job_t {              /* The job struct */
    pid_t pid;              /* job PID */
    int jid;                /* job ID [1, 2, ...] */
    int state;              /* UNDEF, BG, FG, or ST */
    char cmdline[MAXLINE];  /* command line */
};
struct job_t job_list[MAXJOBS]; /* The job list */

struct cmdline_tokens {
    int argc;               /* Number of arguments */
    char *argv[MAXARGS];    /* The arguments list */
    char *infile;           /* The input file */
    char *outfile;          /* The output file */
    enum builtins_t {       /* Indicates if argv[0] is a builtin command */
        BUILTIN_NONE,
        BUILTIN_QUIT,
        BUILTIN_JOBS,
        BUILTIN_BG,
        BUILTIN_FG} builtins;
};

/* pid of foreground job and whether it's terminated or stopped */
volatile int fg_pid, fg_done;

/* End global variables */

/* Function prototypes */
void eval(char *cmdline);

int builtin_fg(struct cmdline_tokens tok);
int builtin_bg(struct cmdline_tokens tok);
int builtin_jobs(struct cmdline_tokens tok);
int run_exe(struct cmdline_tokens, char *cmdline, int bg);

void sigchld_handler(int sig);
void sigtstp_handler(int sig);
void sigint_handler(int sig);

/* Here are helper routines that we've provided for you */
int parseline(const char *cmdline, struct cmdline_tokens *tok); 
void sigquit_handler(int sig);

void clearjob(struct job_t *job);
void initjobs(struct job_t *job_list);
int maxjid(struct job_t *job_list); 
int addjob(struct job_t *job_list, pid_t pid, int state, char *cmdline);
int deletejob(struct job_t *job_list, pid_t pid); 
pid_t fgpid(struct job_t *job_list);
struct job_t *getjobpid(struct job_t *job_list, pid_t pid);
struct job_t *getjobjid(struct job_t *job_list, int jid); 
int pid2jid(pid_t pid); 
void listjobs(struct job_t *job_list, int output_fd);

void usage(void);
void unix_error(char *msg);
void app_error(char *msg);
ssize_t sio_puts(char s[]);
ssize_t sio_putl(long v);
void sio_error(char s[]);

typedef void handler_t(int);
handler_t *Signal(int signum, handler_t *handler);


/*
 * main - The shell's main routine 
 */
int 
main(int argc, char **argv) 
{
    char c;
    char cmdline[MAXLINE];    /* cmdline for fgets */
    int emit_prompt = 1; /* emit prompt (default) */

    /* Redirect stderr to stdout (so that driver will get all output
     * on the pipe connected to stdout) */
    dup2(1, 2);

    /* Parse the command line */
    while ((c = getopt(argc, argv, "hvp")) != EOF) {
        switch (c) {
        case 'h':             /* print help message */
            usage();
            break;
        case 'v':             /* emit additional diagnostic info */
            verbose = 1;
            break;
        case 'p':             /* don't print a prompt */
            emit_prompt = 0;  /* handy for automatic testing */
            break;
        default:
            usage();
        }
    }

    /* Install the signal handlers */

    /* These are the ones you will need to implement */
    Signal(SIGINT,  sigint_handler);   /* ctrl-c */
    Signal(SIGTSTP, sigtstp_handler);  /* ctrl-z */
    Signal(SIGCHLD, sigchld_handler);  /* Terminated or stopped child */
    Signal(SIGTTIN, SIG_IGN);
    Signal(SIGTTOU, SIG_IGN);

    /* This one provides a clean way to kill the shell */
    Signal(SIGQUIT, sigquit_handler); 

    /* Initialize the job list */
    initjobs(job_list);

    /* Execute the shell's read/eval loop */
    while (1) {

        if (emit_prompt) {
            printf("%s", prompt);
            fflush(stdout);
        }
        if ((fgets(cmdline, MAXLINE, stdin) == NULL) && ferror(stdin))
            app_error("fgets error");
        if (feof(stdin)) { 
            /* End of file (ctrl-d) */
            printf ("\n");
            fflush(stdout);
            fflush(stderr);
            exit(0);
        }
        
        /* Remove the trailing newline */
        cmdline[strlen(cmdline)-1] = '\0';
        
        /* Evaluate the command line */
        eval(cmdline);
        
        fflush(stdout);
        fflush(stderr);
    } 
    
    exit(0); /* control never reaches here */
}

/* Fork in csapp */
pid_t Fork(void){
	pid_t pid;
	if ( (pid = fork()) < 0)
		unix_error("Fork error");
	return pid;
}

/* 
 * eval - Evaluate the command line that the user has just typed in
 * 
 * If the user has requested a built-in command (quit, jobs, bg or fg)
 * then execute it immediately. Otherwise, fork a child process and
 * run the job in the context of the child. If the job is running in
 * the foreground, wait for it to terminate and then return.  Note:
 * each child process must have a unique process group ID so that our
 * background children don't receive SIGINT (SIGTSTP) from the kernel
 * when we type ctrl-c (ctrl-z) at the keyboard.  
 */
void 
eval(char *cmdline) 
{
    int bg;              /* should the job run in bg or fg? */
    struct cmdline_tokens tok;

    /* Parse command line */
    bg = parseline(cmdline, &tok);

    if (bg == -1) /* parsing error */
        return;
    if (tok.argv[0] == NULL) /* ignore empty lines */
        return;

	/* I/O redirection */
	/* save original file */
	int pre_in = dup(STDIN_FILENO), pre_out = dup(STDOUT_FILENO);
	if (pre_in == -1 || pre_out == -1) unix_error("Error: dup");

	int fin = STDIN_FILENO, fout = STDOUT_FILENO;
	if (tok.infile){
		if ( (fin = open(tok.infile, O_RDONLY, 0)) < 0){
			fprintf(stderr, "Error: %s No such file or directory\n", tok.infile);
			return ;
		}
	}
	if (tok.outfile){
		if ( (fout = open(tok.outfile, O_WRONLY|O_CREAT|O_TRUNC, DEF_MODE)) < 0){
			fprintf(stderr, "Error: %s No such file or directory\n", tok.outfile);
			/* close opened input file before return */
			if (tok.infile && close(fin)<0) unix_error("Cannot close input file");
			return ;
		}
	}
	if (dup2(fin, STDIN_FILENO) < 0) unix_error("Error: dup2 of input file");
	if (dup2(fout, STDOUT_FILENO) < 0) unix_error("Error: dup2 of output file");

	/* case by case */
	if (tok.builtins == BUILTIN_QUIT)
		exit(0); 

	if (tok.builtins == BUILTIN_JOBS)
		builtin_jobs(tok);

	if (tok.builtins == BUILTIN_FG)
		builtin_fg(tok);

	if (tok.builtins == BUILTIN_BG)
		builtin_bg(tok);

	if (tok.builtins == BUILTIN_NONE)
		run_exe(tok, cmdline, bg);
	
	/* recover std I/O file descriptors */
	if (tok.infile){
		if (dup2(pre_in, STDIN_FILENO) < 0) unix_error("Error: dup2 of input file");
		if (close(fin) < 0) unix_error("Cannot close input file\n");
	}
	if (tok.outfile){
		if (dup2(pre_out, STDOUT_FILENO) < 0) unix_error("Error: dup2 of output file");
		if (close(fout) < 0) unix_error("Cannot close output file\n");
	}

	return;
}

/*
   Wait for process (pid) to terminate or stop
   When the child process (pid) terminates or stops,
   it sends a SIGCHLD to be handled in sigchld_handler(),
   and fg_done is set to 1 and the loop is ended.
   Signals should be blocked before calling this function.
*/
static void waitfg(pid_t pid, sigset_t *prevp){
	if (verbose) printf(" start waiting for %d\n", pid);

	fg_pid = pid;
	fg_done = 0;
	while (!fg_done)
		sigsuspend(prevp);

	if (verbose) printf(" stop waiting for %d\n", pid);
}

/*
   run executable program
   always return 0
*/
int run_exe(struct cmdline_tokens tok, char *cmdline, int bg){
	/* Block all the signals to prevent races.
	   Parent process will not be interrupted by signal sent from child */
	sigset_t mask_all, prev_one;
	sigfillset(&mask_all);
	sigprocmask(SIG_BLOCK, &mask_all, &prev_one);
	
	pid_t pid;
	if ( (pid = Fork()) == 0){
		/* reset the child process and start running*/
		setpgid(0,0);
		Signal(SIGCHLD, SIG_DFL);
		Signal(SIGINT, SIG_DFL);
		Signal(SIGTSTP, SIG_DFL);
		Signal(SIGTTIN, SIG_DFL);
		Signal(SIGTTOU, SIG_DFL);
		Signal(SIGQUIT, SIG_DFL); 
		sigprocmask(SIG_SETMASK, &prev_one, NULL);
		
		if (execve(tok.argv[0], tok.argv, environ) < 0){
			fprintf(stderr, "%s: Command not found\n", cmdline);
			exit(0);
		}
	}

	/* Signals have been blocked, so addjob() is called safely */
	addjob(job_list, pid, bg ? BG : FG, cmdline);
	if (!bg)
		waitfg(pid, &prev_one);
	else
		printf("[%d] (%d) %s\n", pid2jid(pid), pid, cmdline);

	/* Unblock signals */
	sigprocmask(SIG_SETMASK, &prev_one, NULL);
	return 0;
}

/* Convert string to int, return -1 if it fails */
static int my_atoi(char *s){
	int v = 0;
	if (NULL == s) return -1;
	for (; *s; ++s){
		if (*s<'0' || *s>'9') return -1;
		v = v*10 + *s-'0';
	}
	return v;
}

/* Extract pid or jid from the command line argument of "FG" and "BG" command,
   and store them by pointers.
   return 
   0 if the job is identified by pid
   1 if the job is identified by jid
   -1 if fail
*/
static int parse_pid_jid(char *s, pid_t *pidp, int *jidp){
	int is_jid;
	if (NULL == s) return -1;
	if (*s == '%'){
		is_jid = 1;
		if ( (*jidp = my_atoi(s+1)) < 0 ) return -1;
	}else{
		if ( (int)(*pidp = (pid_t)my_atoi(s)) < 0 ) return -1;
	}
	return is_jid;
}

/*
   Command "FG":
   There are two cases: BG->FG, ST->FG
   It's OK to send SIGCONT to the target process in both cases
   and wait for it to terminate or stop.
   return:
   -1 if the argument is invalid
   0 if no job is found or everything is normal.
*/
int builtin_fg(struct cmdline_tokens tok){
	int is_jid, jid;
	pid_t pid;
	struct job_t *job;
	if ( (is_jid = parse_pid_jid(tok.argv[1], &pid, &jid)) < 0){
		fprintf(stderr, "fg command requires PID or %%jobid argument\n");
		return -1;
	}

	/* Block all the signals to prevent signal handlers
	   accessing job_list and other global var */
	sigset_t mask, prev;
	sigfillset(&mask);
	sigprocmask(SIG_BLOCK, &mask, &prev);

	if (is_jid) job = getjobjid(job_list, jid);
	else job = getjobpid(job_list, pid);
	if (job){
		/* set the state of foreground job and wait for it */
		pid = job->pid;
		job->state = FG;
		kill(-pid, SIGCONT);
		waitfg(pid, &prev);
	}else{
		if (is_jid) printf("[%d]", jid);
		else printf("(%d)", pid);
		printf(": No such process\n");
	}	

	sigprocmask(SIG_SETMASK, &prev, NULL);
	return 0;
}

/*
   Command "BG":
   There is only one case: ST->BG.
   just send a SIGCONT
   like "FG", return:
   -1 if the argument is invalid,
   0 if no job is found or everything is normal.
*/
int builtin_bg(struct cmdline_tokens tok){
	int is_jid, jid;
	pid_t pid;
	struct job_t *job;
	if ( (is_jid = parse_pid_jid(tok.argv[1], &pid, &jid)) < 0){
		fprintf(stderr, "bg command requires PID or %%jobid argument\n");
		return -1;
	}
	
	/* Block all the signals to prevent signal handlers
	   accessing job_list and other global var */
	sigset_t mask, prev;
	sigfillset(&mask);
	sigprocmask(SIG_BLOCK, &mask, &prev);

	if (is_jid) job = getjobjid(job_list, jid);
	else job = getjobpid(job_list, pid);
	if (job){
		pid = job->pid;
		job->state = BG;
		kill(-pid, SIGCONT);
		printf("[%d] (%d) %s\n", job->jid, pid, job->cmdline);
	}else{
		if (is_jid) printf("[%d]", jid);
		else printf("(%d)", pid);
		printf(": No such process\n");
	}	

	sigprocmask(SIG_SETMASK, &prev, NULL);
	return 0;
}

/*
   Command "jobs" 
   always return 0
*/
int builtin_jobs(struct cmdline_tokens tok){
	/* Block all the signals to prevent signal handlers accessing global var */
	sigset_t mask_all, prev_one;
	sigfillset(&mask_all);
	sigprocmask(SIG_BLOCK, &mask_all, &prev_one);

	listjobs(job_list, STDOUT_FILENO);

	sigprocmask(SIG_SETMASK, &prev_one, NULL);
	return 0;
}

/* 
 * parseline - Parse the command line and build the argv array.
 * 
 * Parameters:
 *   cmdline:  The command line, in the form:
 *
 *                command [arguments...] [< infile] [> oufile] [&]
 *
 *   tok:      Pointer to a cmdline_tokens structure. The elements of this
 *             structure will be populated with the parsed tokens. Characters 
 *             enclosed in single or double quotes are treated as a single
 *             argument. 
 * Returns:
 *   1:        if the user has requested a BG job
 *   0:        if the user has requested a FG job  
 *  -1:        if cmdline is incorrectly formatted
 * 
 * Note:       The string elements of tok (e.g., argv[], infile, outfile) 
 *             are statically allocated inside parseline() and will be 
 *             overwritten the next time this function is invoked.
 */
int 
parseline(const char *cmdline, struct cmdline_tokens *tok) 
{

    static char array[MAXLINE];          /* holds local copy of command line */
    const char delims[10] = " \t\r\n";   /* argument delimiters (white-space) */
    char *buf = array;                   /* ptr that traverses command line */
    char *next;                          /* ptr to the end of the current arg */
    char *endbuf;                        /* ptr to end of cmdline string */
    int is_bg;                           /* background job? */

    int parsing_state;                   /* indicates if the next token is the
                                            input or output file */

    if (cmdline == NULL) {
        (void) fprintf(stderr, "Error: command line is NULL\n");
        return -1;
    }

    (void) strncpy(buf, cmdline, MAXLINE);
    endbuf = buf + strlen(buf);

    tok->infile = NULL;
    tok->outfile = NULL;

    /* Build the argv list */
    parsing_state = ST_NORMAL;
    tok->argc = 0;

    while (buf < endbuf) {
        /* Skip the white-spaces */
        buf += strspn (buf, delims);
        if (buf >= endbuf) break;

        /* Check for I/O redirection specifiers */
        if (*buf == '<') {
            if (tok->infile) {
                (void) fprintf(stderr, "Error: Ambiguous I/O redirection\n");
                return -1;
            }
            parsing_state |= ST_INFILE;
            buf++;
            continue;
        }
        if (*buf == '>') {
            if (tok->outfile) {
                (void) fprintf(stderr, "Error: Ambiguous I/O redirection\n");
                return -1;
            }
            parsing_state |= ST_OUTFILE;
            buf ++;
            continue;
        }

        if (*buf == '\'' || *buf == '\"') {
            /* Detect quoted tokens */
            buf++;
            next = strchr (buf, *(buf-1));
        } else {
            /* Find next delimiter */
            next = buf + strcspn (buf, delims);
        }
        
        if (next == NULL) {
            /* Returned by strchr(); this means that the closing
               quote was not found. */
            (void) fprintf (stderr, "Error: unmatched %c.\n", *(buf-1));
            return -1;
        }

        /* Terminate the token */
        *next = '\0';

        /* Record the token as either the next argument or the i/o file */
        switch (parsing_state) {
        case ST_NORMAL:
            tok->argv[tok->argc++] = buf;
            break;
        case ST_INFILE:
            tok->infile = buf;
            break;
        case ST_OUTFILE:
            tok->outfile = buf;
            break;
        default:
            (void) fprintf(stderr, "Error: Ambiguous I/O redirection\n");
            return -1;
        }
        parsing_state = ST_NORMAL;

        /* Check if argv is full */
        if (tok->argc >= MAXARGS-1) break;

        buf = next + 1;
    }

    if (parsing_state != ST_NORMAL) {
        (void) fprintf(stderr,
                       "Error: must provide file name for redirection\n");
        return -1;
    }

    /* The argument list must end with a NULL pointer */
    tok->argv[tok->argc] = NULL;

    if (tok->argc == 0)  /* ignore blank line */
        return 1;

    if (!strcmp(tok->argv[0], "quit")) {                 /* quit command */
        tok->builtins = BUILTIN_QUIT;
    } else if (!strcmp(tok->argv[0], "jobs")) {          /* jobs command */
        tok->builtins = BUILTIN_JOBS;
    } else if (!strcmp(tok->argv[0], "bg")) {            /* bg command */
        tok->builtins = BUILTIN_BG;
    } else if (!strcmp(tok->argv[0], "fg")) {            /* fg command */
        tok->builtins = BUILTIN_FG;
    } else {
        tok->builtins = BUILTIN_NONE;
    }

    /* Should the job run in the background? */
    if ((is_bg = (*tok->argv[tok->argc-1] == '&')) != 0)
        tok->argv[--tok->argc] = NULL;

    return is_bg;
}


/*****************
 * Signal handlers
 *****************/

/* 
 * sigchld_handler - The kernel sends a SIGCHLD to the shell whenever
 *     a child job terminates (becomes a zombie), or stops because it
 *     received a SIGSTOP, SIGTSTP, SIGTTIN or SIGTTOU signal. The 
 *     handler reaps all available zombie children, but doesn't wait 
 *     for any other currently running children to terminate.  
 */
void 
sigchld_handler(int sig) 
{
	int olderrno = errno;
	sigset_t mask_all, prev_one;
	pid_t pid;
	int status;

	sigfillset(&mask_all);
	while ((pid = waitpid(-1, &status, WNOHANG | WUNTRACED)) > 0){
		sigprocmask(SIG_BLOCK, &mask_all, &prev_one);

		/* Whether foreground job is done */
		if (pid == fg_pid) fg_done = 1;

		/* job exited normally */
		if (WIFEXITED(status)){
			deletejob(job_list, pid);
		}
		/* job terminated because of signal */
		if (WIFSIGNALED(status)){
			sio_puts("Job ["); sio_putl(pid2jid(pid));
			sio_puts("] ("); sio_putl(pid);
			sio_puts(") terminated by signal "); sio_putl(WTERMSIG(status));
			sio_puts("\n");

			deletejob(job_list, pid);
		}
		/* job stopped */
		if (WIFSTOPPED(status)){
			sio_puts("Job ["); sio_putl(pid2jid(pid));
			sio_puts("] ("); sio_putl(pid);
			sio_puts(") stopped by signal "); sio_putl(WSTOPSIG(status));
			sio_puts("\n");

			struct job_t *job = getjobpid(job_list, pid);
			job->state = ST;
		}

		sigprocmask(SIG_SETMASK, &prev_one, NULL);
	}

	errno = olderrno;
    return;
}

/* 
 * sigint_handler - The kernel sends a SIGINT to the shell whenver the
 *    user types ctrl-c at the keyboard.  Catch it and send it along
 *    to the foreground job.  
 */
void 
sigint_handler(int sig){
	int old = errno;
	sigset_t mask_all, prev;
	sigprocmask(SIG_BLOCK, &mask_all, &prev);

	pid_t pid = fgpid(job_list);
	if (pid){
		kill(-pid, SIGINT);
		if (verbose){
			sio_puts("sigint_handler: send a SIGINT to ");
			sio_putl(pid);
			sio_puts("\n");
		}
	}

	sigprocmask(SIG_SETMASK, &prev, NULL);
	errno = old;
    return;
}

/*
 * sigtstp_handler - The kernel sends a SIGTSTP to the shell whenever
 *     the user types ctrl-z at the keyboard. Catch it and suspend the
 *     foreground job by sending it a SIGTSTP.  
 */
void 
sigtstp_handler(int sig){
	int old = errno;
	sigset_t mask_all, prev;
	sigprocmask(SIG_BLOCK, &mask_all, &prev);
	
	pid_t pid = fgpid(job_list);
	if (pid){
		kill(-pid, SIGTSTP);
		if (verbose){
			sio_puts("sigint_handler: send a SIGTSTP to ");
			sio_putl(pid);
			sio_puts("\n");
		}
	}

	sigprocmask(SIG_SETMASK, &prev, NULL);
	errno = old;
    return;
}

/*
 * sigquit_handler - The driver program can gracefully terminate the
 *    child shell by sending it a SIGQUIT signal.
 */
void 
sigquit_handler(int sig) 
{
    sio_error("Terminating after receipt of SIGQUIT signal\n");
}



/*********************
 * End signal handlers
 *********************/

/***********************************************
 * Helper routines that manipulate the job list
 **********************************************/

/* clearjob - Clear the entries in a job struct */
void 
clearjob(struct job_t *job) {
    job->pid = 0;
    job->jid = 0;
    job->state = UNDEF;
    job->cmdline[0] = '\0';
}

/* initjobs - Initialize the job list */
void 
initjobs(struct job_t *job_list) {
    int i;

    for (i = 0; i < MAXJOBS; i++)
        clearjob(&job_list[i]);
}

/* maxjid - Returns largest allocated job ID */
int 
maxjid(struct job_t *job_list) 
{
    int i, max=0;

    for (i = 0; i < MAXJOBS; i++)
        if (job_list[i].jid > max)
            max = job_list[i].jid;
    return max;
}

/* addjob - Add a job to the job list */
int 
addjob(struct job_t *job_list, pid_t pid, int state, char *cmdline) 
{
    int i;

    if (pid < 1)
        return 0;

    for (i = 0; i < MAXJOBS; i++) {
        if (job_list[i].pid == 0) {
            job_list[i].pid = pid;
            job_list[i].state = state;
            job_list[i].jid = nextjid++;
            if (nextjid > MAXJOBS)
                nextjid = 1;
            strcpy(job_list[i].cmdline, cmdline);
            if(verbose){
                printf("Added job [%d] %d %s\n",
                       job_list[i].jid,
                       job_list[i].pid,
                       job_list[i].cmdline);
            }
            return 1;
        }
    }
    printf("Tried to create too many jobs\n");
    return 0;
}

/* deletejob - Delete a job whose PID=pid from the job list */
int 
deletejob(struct job_t *job_list, pid_t pid) 
{
    int i;

    if (pid < 1)
        return 0;

    for (i = 0; i < MAXJOBS; i++) {
        if (job_list[i].pid == pid) {
            clearjob(&job_list[i]);
            nextjid = maxjid(job_list)+1;
            return 1;
        }
    }
    return 0;
}

/* fgpid - Return PID of current foreground job, 0 if no such job */
pid_t 
fgpid(struct job_t *job_list) {
    int i;

    for (i = 0; i < MAXJOBS; i++)
        if (job_list[i].state == FG)
            return job_list[i].pid;
    return 0;
}

/* getjobpid  - Find a job (by PID) on the job list */
struct job_t 
*getjobpid(struct job_t *job_list, pid_t pid) {
    int i;

    if (pid < 1)
        return NULL;
    for (i = 0; i < MAXJOBS; i++)
        if (job_list[i].pid == pid)
            return &job_list[i];
    return NULL;
}

/* getjobjid  - Find a job (by JID) on the job list */
struct job_t *getjobjid(struct job_t *job_list, int jid) 
{
    int i;

    if (jid < 1)
        return NULL;
    for (i = 0; i < MAXJOBS; i++)
        if (job_list[i].jid == jid)
            return &job_list[i];
    return NULL;
}

/* pid2jid - Map process ID to job ID */
int 
pid2jid(pid_t pid) 
{
    int i;

    if (pid < 1)
        return 0;
    for (i = 0; i < MAXJOBS; i++)
        if (job_list[i].pid == pid) {
            return job_list[i].jid;
        }
    return 0;
}

/* listjobs - Print the job list */
void 
listjobs(struct job_t *job_list, int output_fd) 
{
    int i;
    char buf[MAXLINE];

    for (i = 0; i < MAXJOBS; i++) {
        memset(buf, '\0', MAXLINE);
        if (job_list[i].pid != 0) {
            sprintf(buf, "[%d] (%d) ", job_list[i].jid, job_list[i].pid);
            if(write(output_fd, buf, strlen(buf)) < 0) {
                fprintf(stderr, "Error writing to output file\n");
                exit(1);
            }
            memset(buf, '\0', MAXLINE);
            switch (job_list[i].state) {
            case BG:
                sprintf(buf, "Running    ");
                break;
            case FG:
                sprintf(buf, "Foreground ");
                break;
            case ST:
                sprintf(buf, "Stopped    ");
                break;
            default:
                sprintf(buf, "listjobs: Internal error: job[%d].state=%d ",
                        i, job_list[i].state);
            }
            if(write(output_fd, buf, strlen(buf)) < 0) {
                fprintf(stderr, "Error writing to output file\n");
                exit(1);
            }
            memset(buf, '\0', MAXLINE);
            sprintf(buf, "%s\n", job_list[i].cmdline);
            if(write(output_fd, buf, strlen(buf)) < 0) {
                fprintf(stderr, "Error writing to output file\n");
                exit(1);
            }
        }
    }
}
/******************************
 * end job list helper routines
 ******************************/


/***********************
 * Other helper routines
 ***********************/

/*
 * usage - print a help message
 */
void 
usage(void) 
{
    printf("Usage: shell [-hvp]\n");
    printf("   -h   print this message\n");
    printf("   -v   print additional diagnostic information\n");
    printf("   -p   do not emit a command prompt\n");
    exit(1);
}

/*
 * unix_error - unix-style error routine
 */
void 
unix_error(char *msg)
{
    fprintf(stdout, "%s: %s\n", msg, strerror(errno));
    exit(1);
}

/*
 * app_error - application-style error routine
 */
void 
app_error(char *msg)
{
    fprintf(stdout, "%s\n", msg);
    exit(1);
}

/* Private sio_functions */
/* sio_reverse - Reverse a string (from K&R) */
static void sio_reverse(char s[])
{
    int c, i, j;

    for (i = 0, j = strlen(s)-1; i < j; i++, j--) {
        c = s[i];
        s[i] = s[j];
        s[j] = c;
    }
}

/* sio_ltoa - Convert long to base b string (from K&R) */
static void sio_ltoa(long v, char s[], int b) 
{
    int c, i = 0;
    
    do {  
        s[i++] = ((c = (v % b)) < 10)  ?  c + '0' : c - 10 + 'a';
    } while ((v /= b) > 0);
    s[i] = '\0';
    sio_reverse(s);
}

/* sio_strlen - Return length of string (from K&R) */
static size_t sio_strlen(char s[])
{
    int i = 0;

    while (s[i] != '\0')
        ++i;
    return i;
}

/* Public Sio functions */
ssize_t sio_puts(char s[]) /* Put string */
{
    return write(STDOUT_FILENO, s, sio_strlen(s));
}

ssize_t sio_putl(long v) /* Put long */
{
    char s[128];
    
    sio_ltoa(v, s, 10); /* Based on K&R itoa() */ 
    return sio_puts(s);
}

void sio_error(char s[]) /* Put error message and exit */
{
    sio_puts(s);
    _exit(1);
}

/*
 * Signal - wrapper for the sigaction function
 */
handler_t 
*Signal(int signum, handler_t *handler) 
{
    struct sigaction action, old_action;

    action.sa_handler = handler;  
    sigemptyset(&action.sa_mask); /* block sigs of type being handled */
    action.sa_flags = SA_RESTART; /* restart syscalls if possible */

    if (sigaction(signum, &action, &old_action) < 0)
        unix_error("Signal error");
    return (old_action.sa_handler);
}

