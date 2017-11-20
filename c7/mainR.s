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
  Start of section headers:          1200 (bytes into file)
  Flags:                             0x0
  Size of this header:               64 (bytes)
  Size of program headers:           0 (bytes)
  Number of program headers:         0
  Size of section headers:           64 (bytes)
  Number of section headers:         12
  Section header string table index: 9

Section Headers:
  [Nr] Name              Type             Address           Offset
       Size              EntSize          Flags  Link  Info  Align
  [ 0]                   NULL             0000000000000000  00000000
       0000000000000000  0000000000000000           0     0     0
  [ 1] .text             PROGBITS         0000000000000000  00000040
       0000000000000069  0000000000000000  AX       0     0     1
  [ 2] .rela.text        RELA             0000000000000000  00000378
       0000000000000090  0000000000000018   I      10     1     8
  [ 3] .data             PROGBITS         0000000000000000  000000ac
       0000000000000010  0000000000000000  WA       0     0     4
  [ 4] .bss              NOBITS           0000000000000000  000000bc
       0000000000000004  0000000000000000  WA       0     0     4
  [ 5] .comment          PROGBITS         0000000000000000  000000bc
       0000000000000035  0000000000000001  MS       0     0     1
  [ 6] .note.GNU-stack   PROGBITS         0000000000000000  000000f1
       0000000000000000  0000000000000000           0     0     1
  [ 7] .eh_frame         PROGBITS         0000000000000000  000000f8
       0000000000000080  0000000000000000   A       0     0     8
  [ 8] .rela.eh_frame    RELA             0000000000000000  00000408
       0000000000000048  0000000000000018   I      10     7     8
  [ 9] .shstrtab         STRTAB           0000000000000000  00000450
       0000000000000059  0000000000000000           0     0     1
  [10] .symtab           SYMTAB           0000000000000000  00000178
       00000000000001b0  0000000000000018          11    12     8
  [11] .strtab           STRTAB           0000000000000000  00000328
       000000000000004f  0000000000000000           0     0     1
Key to Flags:
  W (write), A (alloc), X (execute), M (merge), S (strings), l (large)
  I (info), L (link order), G (group), T (TLS), E (exclude), x (unknown)
  O (extra OS processing required) o (OS specific), p (processor specific)

There are no section groups in this file.

There are no program headers in this file.

Relocation section '.rela.text' at offset 0x378 contains 6 entries:
  Offset          Info           Type           Sym. Value    Sym. Name + Addend
000000000006  000300000002 R_X86_64_PC32     0000000000000000 .data + 4
000000000012  000400000002 R_X86_64_PC32     0000000000000000 .bss - 4
00000000002c  001100000002 R_X86_64_PC32     0000000000000000 add - 4
000000000039  000e00000002 R_X86_64_PC32     0000000000000000 f1 - 4
00000000004b  000f00000002 R_X86_64_PC32     000000000000000c f2 - 4
000000000054  000300000002 R_X86_64_PC32     0000000000000000 .data + 8

Relocation section '.rela.eh_frame' at offset 0x408 contains 3 entries:
  Offset          Info           Type           Sym. Value    Sym. Name + Addend
000000000020  000200000002 R_X86_64_PC32     0000000000000000 .text + 0
000000000040  000200000002 R_X86_64_PC32     0000000000000000 .text + c
000000000060  000200000002 R_X86_64_PC32     0000000000000000 .text + 18

The decoding of unwind sections for machine type Advanced Micro Devices X86-64 is not currently supported.

Symbol table '.symtab' contains 18 entries:
   Num:    Value          Size Type    Bind   Vis      Ndx Name
     0: 0000000000000000     0 NOTYPE  LOCAL  DEFAULT  UND 
     1: 0000000000000000     0 FILE    LOCAL  DEFAULT  ABS main.c
     2: 0000000000000000     0 SECTION LOCAL  DEFAULT    1 
     3: 0000000000000000     0 SECTION LOCAL  DEFAULT    3 
     4: 0000000000000000     0 SECTION LOCAL  DEFAULT    4 
     5: 0000000000000004     4 OBJECT  LOCAL  DEFAULT    3 globalA
     6: 0000000000000008     4 OBJECT  LOCAL  DEFAULT    3 x.1839
     7: 0000000000000000     4 OBJECT  LOCAL  DEFAULT    4 x.1842
     8: 000000000000000c     4 OBJECT  LOCAL  DEFAULT    3 staticMainB.1845
     9: 0000000000000000     0 SECTION LOCAL  DEFAULT    6 
    10: 0000000000000000     0 SECTION LOCAL  DEFAULT    7 
    11: 0000000000000000     0 SECTION LOCAL  DEFAULT    5 
    12: 0000000000000000     4 OBJECT  GLOBAL DEFAULT    3 str
    13: 0000000000000004     4 OBJECT  GLOBAL DEFAULT  COM globalUnInit
    14: 0000000000000000    12 FUNC    GLOBAL DEFAULT    1 f1
    15: 000000000000000c    12 FUNC    GLOBAL DEFAULT    1 f2
    16: 0000000000000018    81 FUNC    GLOBAL DEFAULT    1 main
    17: 0000000000000000     0 NOTYPE  GLOBAL DEFAULT  UND add

No version information found in this file.
