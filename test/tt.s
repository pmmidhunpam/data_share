
test.o:     file format elf32-i386

Disassembly of section .text:

00000000 <fun>:
  short s3;
  char c2;
  int i;
};

void fun(int a) {
   0:	55                   	push   %ebp
   1:	89 e5                	mov    %esp,%ebp
   3:	83 ec 10             	sub    $0x10,%esp
  int i = 888;
   6:	c7 45 fc 78 03 00 00 	movl   $0x378,-0x4(%ebp)
  i = a;
   d:	8b 45 08             	mov    0x8(%ebp),%eax
  10:	89 45 fc             	mov    %eax,-0x4(%ebp)
  return;
}
  13:	c9                   	leave  
  14:	c3                   	ret    

00000015 <main>:
int main(int argc, char **argv) {
  15:	8d 4c 24 04          	lea    0x4(%esp),%ecx
  19:	83 e4 f0             	and    $0xfffffff0,%esp
  1c:	ff 71 fc             	pushl  -0x4(%ecx)
  1f:	55                   	push   %ebp
  20:	89 e5                	mov    %esp,%ebp
  22:	51                   	push   %ecx
  23:	83 ec 04             	sub    $0x4,%esp
  }
  for (i=0; i<100; i++) {
    printf("%d ", p->data[i]);
  }
*/
  fun(0);
  26:	c7 04 24 00 00 00 00 	movl   $0x0,(%esp)
  2d:	e8 fc ff ff ff       	call   2e <main+0x19>
  return 0;
  32:	b8 00 00 00 00       	mov    $0x0,%eax
}
  37:	83 c4 04             	add    $0x4,%esp
  3a:	59                   	pop    %ecx
  3b:	5d                   	pop    %ebp
  3c:	8d 61 fc             	lea    -0x4(%ecx),%esp
  3f:	c3                   	ret    
