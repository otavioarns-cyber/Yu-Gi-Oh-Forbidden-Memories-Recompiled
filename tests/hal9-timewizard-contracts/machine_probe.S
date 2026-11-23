/* Test only: seed volatile/callee-saved registers, flags and x87 across the
 * actual relocated probe. No game routine or approved decision is replaced. */
.text
.globl Hal9_TestMachine
.type Hal9_TestMachine,@function
Hal9_TestMachine:
 pushl %ebp
 movl %esp,%ebp
 pushfl
 pushal
 subl $64,%esp
 movl 8(%ebp),%eax
 movl %eax,0(%esp)
 fnstcw 4(%esp)
 fld1
 movl $0x11111111,%eax
 movl $0x22222222,%ecx
 movl $0x33333333,%edx
 movl $0x44444444,%ebx
 movl $0x55555555,%esi
 movl $0x66666666,%edi
 pushl $0x247
 popfl
 pushfl
 popl 8(%esp)
 call *0(%esp)
 pushfl
 pushal
 movl 12(%ebp),%edi
 movl %esp,%esi
 movl $9,%ecx
 rep movsl
 addl $36,%esp
 movl 12(%ebp),%edi
 movl 8(%esp),%eax
 movl %eax,36(%edi)
 fstpl 40(%edi)
 fnstcw 48(%edi)
 movl %ebp,52(%edi)
 leal -104(%ebp),%eax
 movl %eax,56(%edi)
 movzwl 4(%esp),%eax
 movl %eax,60(%edi)
 fldcw 4(%esp)
 leal -36(%ebp),%esp
 popal
 popfl
 leave
 ret
.size Hal9_TestMachine,.-Hal9_TestMachine
.section .note.GNU-stack,"",@progbits
