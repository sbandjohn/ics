
useop.o:     file format elf64-x86-64
useop.o
architecture: i386:x86-64, flags 0x00000011:
HAS_RELOC, HAS_SYMS
start address 0x0000000000000000

Sections:
Idx Name          Size      VMA               LMA               File off  Algn
  0 .text         0000006c  0000000000000000  0000000000000000  00000040  2**0
                  CONTENTS, ALLOC, LOAD, RELOC, READONLY, CODE
  1 .data         00000000  0000000000000000  0000000000000000  000000ac  2**0
                  CONTENTS, ALLOC, LOAD, DATA
  2 .bss          00000000  0000000000000000  0000000000000000  000000ac  2**0
                  ALLOC
  3 .rodata       0000000a  0000000000000000  0000000000000000  000000ac  2**0
                  CONTENTS, ALLOC, LOAD, READONLY, DATA
  4 .comment      00000035  0000000000000000  0000000000000000  000000b6  2**0
                  CONTENTS, READONLY
  5 .note.GNU-stack 00000000  0000000000000000  0000000000000000  000000eb  2**0
                  CONTENTS, READONLY
  6 .eh_frame     00000038  0000000000000000  0000000000000000  000000f0  2**3
                  CONTENTS, ALLOC, LOAD, RELOC, READONLY, DATA
SYMBOL TABLE:
0000000000000000 l    df *ABS*	0000000000000000 useop.c
0000000000000000 l    d  .text	0000000000000000 .text
0000000000000000 l    d  .data	0000000000000000 .data
0000000000000000 l    d  .bss	0000000000000000 .bss
0000000000000000 l    d  .rodata	0000000000000000 .rodata
0000000000000000 l    d  .note.GNU-stack	0000000000000000 .note.GNU-stack
0000000000000000 l    d  .eh_frame	0000000000000000 .eh_frame
0000000000000000 l    d  .comment	0000000000000000 .comment
0000000000000000 g     F .text	000000000000006c main
0000000000000000         *UND*	0000000000000000 __isoc99_scanf
0000000000000000         *UND*	0000000000000000 add
0000000000000000         *UND*	0000000000000000 printf
0000000000000000         *UND*	0000000000000000 __stack_chk_fail



Disassembly of section .text:

0000000000000000 <main>:
   0:	55                   	push   %rbp
   1:	48 89 e5             	mov    %rsp,%rbp
   4:	48 83 ec 10          	sub    $0x10,%rsp
   8:	64 48 8b 04 25 28 00 	mov    %fs:0x28,%rax
   f:	00 00 
  11:	48 89 45 f8          	mov    %rax,-0x8(%rbp)
  15:	31 c0                	xor    %eax,%eax
  17:	48 8d 55 f4          	lea    -0xc(%rbp),%rdx
  1b:	48 8d 45 f0          	lea    -0x10(%rbp),%rax
  1f:	48 89 c6             	mov    %rax,%rsi
  22:	bf 00 00 00 00       	mov    $0x0,%edi
			23: R_X86_64_32	.rodata
  27:	b8 00 00 00 00       	mov    $0x0,%eax
  2c:	e8 00 00 00 00       	callq  31 <main+0x31>
			2d: R_X86_64_PC32	__isoc99_scanf-0x4
  31:	8b 55 f4             	mov    -0xc(%rbp),%edx
  34:	8b 45 f0             	mov    -0x10(%rbp),%eax
  37:	89 d6                	mov    %edx,%esi
  39:	89 c7                	mov    %eax,%edi
  3b:	e8 00 00 00 00       	callq  40 <main+0x40>
			3c: R_X86_64_PC32	add-0x4
  40:	89 c6                	mov    %eax,%esi
  42:	bf 00 00 00 00       	mov    $0x0,%edi
			43: R_X86_64_32	.rodata+0x6
  47:	b8 00 00 00 00       	mov    $0x0,%eax
  4c:	e8 00 00 00 00       	callq  51 <main+0x51>
			4d: R_X86_64_PC32	printf-0x4
  51:	b8 00 00 00 00       	mov    $0x0,%eax
  56:	48 8b 4d f8          	mov    -0x8(%rbp),%rcx
  5a:	64 48 33 0c 25 28 00 	xor    %fs:0x28,%rcx
  61:	00 00 
  63:	74 05                	je     6a <main+0x6a>
  65:	e8 00 00 00 00       	callq  6a <main+0x6a>
			66: R_X86_64_PC32	__stack_chk_fail-0x4
  6a:	c9                   	leaveq 
  6b:	c3                   	retq   
