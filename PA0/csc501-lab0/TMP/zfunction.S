.globl zfunction

zfunction:

pushl   %ebp
movl    %esp,%ebp
andl    $0xffc01fff,8(%ebp)
shll    $4,8(%ebp)
movl    8(%ebp),%eax
movl     %ebp,%esp 
popl    %ebp
ret   
