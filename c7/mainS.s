
main.o:     file format elf64-x86-64
main.o
architecture: i386:x86-64, flags 0x00000011:
HAS_RELOC, HAS_SYMS
start address 0x0000000000000000

Sections:
Idx Name          Size      VMA               LMA               File off  Algn
  0 .text         00000071  0000000000000000  0000000000000000  00000040  2**0
                  CONTENTS, ALLOC, LOAD, RELOC, READONLY, CODE
  1 .data         00000010  0000000000000000  0000000000000000  000000b4  2**2
                  CONTENTS, ALLOC, LOAD, DATA
  2 .bss          00000004  0000000000000000  0000000000000000  000000c4  2**2
                  ALLOC
  3 .comment      00000035  0000000000000000  0000000000000000  000000c4  2**0
                  CONTENTS, READONLY
  4 .note.GNU-stack 00000000  0000000000000000  0000000000000000  000000f9  2**0
                  CONTENTS, READONLY
  5 .eh_frame     00000080  0000000000000000  0000000000000000  00000100  2**3
                  CONTENTS, ALLOC, LOAD, RELOC, READONLY, DATA
SYMBOL TABLE:
0000000000000000 l    df *ABS*	0000000000000000 main.c
0000000000000000 l    d  .text	0000000000000000 .text
0000000000000000 l    d  .data	0000000000000000 .data
0000000000000000 l    d  .bss	0000000000000000 .bss
0000000000000004 l     O .data	0000000000000004 globalA
0000000000000008 l     O .data	0000000000000004 x.1839
0000000000000000 l     O .bss	0000000000000004 x.1842
000000000000000c l     O .data	0000000000000004 staticMainB.1845
0000000000000000 l    d  .note.GNU-stack	0000000000000000 .note.GNU-stack
0000000000000000 l    d  .eh_frame	0000000000000000 .eh_frame
0000000000000000 l    d  .comment	0000000000000000 .comment
0000000000000000 g     O .data	0000000000000004 str
0000000000000004       O *COM*	0000000000000004 globalUnInit
0000000000000000 g     F .text	000000000000000c f1
000000000000000c g     F .text	000000000000000c f2
0000000000000018 g     F .text	0000000000000059 main
0000000000000000         *UND*	0000000000000000 add
0000000000000000         *UND*	0000000000000000 addX



Disassembly of section .text:

0000000000000000 <f1>:
   0:	55                   	push   %rbp
   1:	48 89 e5             	mov    %rsp,%rbp
   4:	8b 05 00 00 00 00    	mov    0x0(%rip),%eax        # a <f1+0xa>
			6: R_X86_64_PC32	.data+0x4
   a:	5d                   	pop    %rbp
   b:	c3                   	retq   

000000000000000c <f2>:
   c:	55                   	push   %rbp
   d:	48 89 e5             	mov    %rsp,%rbp
  10:	8b 05 00 00 00 00    	mov    0x0(%rip),%eax        # 16 <f2+0xa>
			12: R_X86_64_PC32	.bss-0x4
  16:	5d                   	pop    %rbp
  17:	c3                   	retq   

0000000000000018 <main>:
  18:	55                   	push   %rbp
  19:	48 89 e5             	mov    %rsp,%rbp
  1c:	53                   	push   %rbx
  1d:	48 83 ec 18          	sub    $0x18,%rsp
  21:	8b 55 e4             	mov    -0x1c(%rbp),%edx
  24:	8b 45 e0             	mov    -0x20(%rbp),%eax
  27:	89 d6                	mov    %edx,%esi
  29:	89 c7                	mov    %eax,%edi
  2b:	e8 00 00 00 00       	callq  30 <main+0x18>
			2c: R_X86_64_PC32	add-0x4
  30:	89 45 e8             	mov    %eax,-0x18(%rbp)
  33:	b8 00 00 00 00       	mov    $0x0,%eax
  38:	e8 00 00 00 00       	callq  3d <main+0x25>
			39: R_X86_64_PC32	f1-0x4
  3d:	89 c2                	mov    %eax,%edx
  3f:	8b 45 e8             	mov    -0x18(%rbp),%eax
  42:	8d 1c 02             	lea    (%rdx,%rax,1),%ebx
  45:	b8 00 00 00 00       	mov    $0x0,%eax
  4a:	e8 00 00 00 00       	callq  4f <main+0x37>
			4b: R_X86_64_PC32	f2-0x4
  4f:	8d 14 03             	lea    (%rbx,%rax,1),%edx
  52:	8b 05 00 00 00 00    	mov    0x0(%rip),%eax        # 58 <main+0x40>
			54: R_X86_64_PC32	.data+0x8
  58:	01 c2                	add    %eax,%edx
  5a:	8b 05 00 00 00 00    	mov    0x0(%rip),%eax        # 60 <main+0x48>
			5c: R_X86_64_PC32	addX-0x4
  60:	01 d0                	add    %edx,%eax
  62:	89 45 ec             	mov    %eax,-0x14(%rbp)
  65:	b8 00 00 00 00       	mov    $0x0,%eax
  6a:	48 83 c4 18          	add    $0x18,%rsp
  6e:	5b                   	pop    %rbx
  6f:	5d                   	pop    %rbp
  70:	c3                   	retq   
