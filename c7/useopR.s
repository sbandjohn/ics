ELF Header:
  Magic:   7f 45 4c 46 02 01 01 00 00 00 00 00 00 00 00 00 
  Class:                             ELF64
  Data:                              2's complement, little endian
  Version:                           1 (current)
  OS/ABI:                            UNIX - System V
  ABI Version:                       0
  Type:                              REL (Relocatable file)
  Machine:                           Advanced Micro Devices X86-64
  Version:                           0x1
  Entry point address:               0x0
  Start of program headers:          0 (bytes into file)
  Start of section headers:          968 (bytes into file)
  Flags:                             0x0
  Size of this header:               64 (bytes)
  Size of program headers:           0 (bytes)
  Number of program headers:         0
  Size of section headers:           64 (bytes)
  Number of section headers:         13
  Section header string table index: 10

Section Headers:
  [Nr] Name              Type             Address           Offset
       Size              EntSize          Flags  Link  Info  Align
  [ 0]                   NULL             0000000000000000  00000000
       0000000000000000  0000000000000000           0     0     0
  [ 1] .text             PROGBITS         0000000000000000  00000040
       000000000000006c  0000000000000000  AX       0     0     1
  [ 2] .rela.text        RELA             0000000000000000  000002b8
       0000000000000090  0000000000000018   I      11     1     8
  [ 3] .data             PROGBITS         0000000000000000  000000ac
       0000000000000000  0000000000000000  WA       0     0     1
  [ 4] .bss              NOBITS           0000000000000000  000000ac
       0000000000000000  0000000000000000  WA       0     0     1
  [ 5] .rodata           PROGBITS         0000000000000000  000000ac
       000000000000000a  0000000000000000   A       0     0     1
  [ 6] .comment          PROGBITS         0000000000000000  000000b6
       0000000000000035  0000000000000001  MS       0     0     1
  [ 7] .note.GNU-stack   PROGBITS         0000000000000000  000000eb
       0000000000000000  0000000000000000           0     0     1
  [ 8] .eh_frame         PROGBITS         0000000000000000  000000f0
       0000000000000038  0000000000000000   A       0     0     8
  [ 9] .rela.eh_frame    RELA             0000000000000000  00000348
       0000000000000018  0000000000000018   I      11     8     8
  [10] .shstrtab         STRTAB           0000000000000000  00000360
       0000000000000061  0000000000000000           0     0     1
  [11] .symtab           SYMTAB           0000000000000000  00000128
       0000000000000150  0000000000000018          12     9     8
  [12] .strtab           STRTAB           0000000000000000  00000278
       0000000000000039  0000000000000000           0     0     1
Key to Flags:
  W (write), A (alloc), X (execute), M (merge), S (strings), l (large)
  I (info), L (link order), G (group), T (TLS), E (exclude), x (unknown)
  O (extra OS processing required) o (OS specific), p (processor specific)

There are no section groups in this file.

There are no program headers in this file.

Relocation section '.rela.text' at offset 0x2b8 contains 6 entries:
  Offset          Info           Type           Sym. Value    Sym. Name + Addend
000000000023  00050000000a R_X86_64_32       0000000000000000 .rodata + 0
00000000002d  000a00000002 R_X86_64_PC32     0000000000000000 __isoc99_scanf - 4
00000000003c  000b00000002 R_X86_64_PC32     0000000000000000 add - 4
000000000043  00050000000a R_X86_64_32       0000000000000000 .rodata + 6
00000000004d  000c00000002 R_X86_64_PC32     0000000000000000 printf - 4
000000000066  000d00000002 R_X86_64_PC32     0000000000000000 __stack_chk_fail - 4

Relocation section '.rela.eh_frame' at offset 0x348 contains 1 entries:
  Offset          Info           Type           Sym. Value    Sym. Name + Addend
000000000020  000200000002 R_X86_64_PC32     0000000000000000 .text + 0

The decoding of unwind sections for machine type Advanced Micro Devices X86-64 is not currently supported.

Symbol table '.symtab' contains 14 entries:
   Num:    Value          Size Type    Bind   Vis      Ndx Name
     0: 0000000000000000     0 NOTYPE  LOCAL  DEFAULT  UND 
     1: 0000000000000000     0 FILE    LOCAL  DEFAULT  ABS useop.c
     2: 0000000000000000     0 SECTION LOCAL  DEFAULT    1 
     3: 0000000000000000     0 SECTION LOCAL  DEFAULT    3 
     4: 0000000000000000     0 SECTION LOCAL  DEFAULT    4 
     5: 0000000000000000     0 SECTION LOCAL  DEFAULT    5 
     6: 0000000000000000     0 SECTION LOCAL  DEFAULT    7 
     7: 0000000000000000     0 SECTION LOCAL  DEFAULT    8 
     8: 0000000000000000     0 SECTION LOCAL  DEFAULT    6 
     9: 0000000000000000   108 FUNC    GLOBAL DEFAULT    1 main
    10: 0000000000000000     0 NOTYPE  GLOBAL DEFAULT  UND __isoc99_scanf
    11: 0000000000000000     0 NOTYPE  GLOBAL DEFAULT  UND add
    12: 0000000000000000     0 NOTYPE  GLOBAL DEFAULT  UND printf
    13: 0000000000000000     0 NOTYPE  GLOBAL DEFAULT  UND __stack_chk_fail

No version information found in this file.
