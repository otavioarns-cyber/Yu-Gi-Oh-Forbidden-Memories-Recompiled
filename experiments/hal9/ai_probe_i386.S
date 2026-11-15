/* Original inline loads change only EAX or ECX and leave flags/FPU alone.
 * Preserve all other state across the experimental core query. FXSR is
 * optional: i686 without it uses the original x87 save/restore format. */
.text
.macro HARD_PROBE name, result_offset
.p2align 4
.globl \name
.type \name,@function
\name:
 pushl %ebp
 movl %esp,%ebp
 pushfl
 pushal
 subl $528,%esp
 andl $-16,%esp
 movl $1,%eax
 cpuid
 testl $0x01000000,%edx
 jz 1f
 fxsave (%esp)
 call Hal9_CoreHardStateV1
 /* UNKNOWN is not ON. The core readiness query separately refuses it. */
 testl %eax,%eax
 setg %al
 movzbl %al,%eax
 fxrstor (%esp)
 jmp 2f
1:
 fnsave (%esp)
 call Hal9_CoreHardStateV1
 testl %eax,%eax
 setg %al
 movzbl %al,%eax
 frstor (%esp)
2:
 movl %eax,\result_offset(%ebp)
 leal -36(%ebp),%esp
 popal
 popfl
 leave
 ret
.size \name,.-\name
.endm
HARD_PROBE Hal9_ReadHardEAX,-8
HARD_PROBE Hal9_ReadHardECX,-12
.section .note.GNU-stack,"",@progbits
