
#include "custom_ops.S"

#define ENABLE_QREGS

#undef ENABLE_FASTIRQ

.section .init
.global main
.global start
.global entry
.global irq
.global _start

_start:
	// no more than 16 bytes here !
	// picorv32_waitirq_insn(zero)
	picorv32_maskirq_insn(zero, zero)
	j start


/* Interrupt handler
 **********************************/

.balign 16
irq_vec:
	/* save registers */

#ifdef ENABLE_QREGS

	picorv32_setq_insn(q2, x1)
	picorv32_setq_insn(q3, x2)

	lui x1, %hi(irq_regs)
	addi x1, x1, %lo(irq_regs)

	picorv32_getq_insn(x2, q0)
	sw x2,   0*4(x1)

	picorv32_getq_insn(x2, q2)
	sw x2,   1*4(x1)

	picorv32_getq_insn(x2, q3)
	sw x2,   2*4(x1)

#ifdef ENABLE_FASTIRQ
	sw x5,   5*4(x1)
	sw x6,   6*4(x1)
	sw x7,   7*4(x1)
	sw x10, 10*4(x1)
	sw x11, 11*4(x1)
	sw x12, 12*4(x1)
	sw x13, 13*4(x1)
	sw x14, 14*4(x1)
	sw x15, 15*4(x1)
	sw x16, 16*4(x1)
	sw x17, 17*4(x1)
	sw x28, 28*4(x1)
	sw x29, 29*4(x1)
	sw x30, 30*4(x1)
	sw x31, 31*4(x1)
#else
	sw x3,   3*4(x1)
	sw x4,   4*4(x1)
	sw x5,   5*4(x1)
	sw x6,   6*4(x1)
	sw x7,   7*4(x1)
	sw x8,   8*4(x1)
	sw x9,   9*4(x1)
	sw x10, 10*4(x1)
	sw x11, 11*4(x1)
	sw x12, 12*4(x1)
	sw x13, 13*4(x1)
	sw x14, 14*4(x1)
	sw x15, 15*4(x1)
	sw x16, 16*4(x1)
	sw x17, 17*4(x1)
	sw x18, 18*4(x1)
	sw x19, 19*4(x1)
	sw x20, 20*4(x1)
	sw x21, 21*4(x1)
	sw x22, 22*4(x1)
	sw x23, 23*4(x1)
	sw x24, 24*4(x1)
	sw x25, 25*4(x1)
	sw x26, 26*4(x1)
	sw x27, 27*4(x1)
	sw x28, 28*4(x1)
	sw x29, 29*4(x1)
	sw x30, 30*4(x1)
	sw x31, 31*4(x1)
#endif

#else // ENABLE_QREGS

#ifdef ENABLE_FASTIRQ
	sw gp,   0*4+0x200(zero)
	sw x1,   1*4+0x200(zero)
	sw x2,   2*4+0x200(zero)
	sw x5,   5*4+0x200(zero)
	sw x6,   6*4+0x200(zero)
	sw x7,   7*4+0x200(zero)
	sw x10, 10*4+0x200(zero)
	sw x11, 11*4+0x200(zero)
	sw x12, 12*4+0x200(zero)
	sw x13, 13*4+0x200(zero)
	sw x14, 14*4+0x200(zero)
	sw x15, 15*4+0x200(zero)
	sw x16, 16*4+0x200(zero)
	sw x17, 17*4+0x200(zero)
	sw x28, 28*4+0x200(zero)
	sw x29, 29*4+0x200(zero)
	sw x30, 30*4+0x200(zero)
	sw x31, 31*4+0x200(zero)
#else
	sw gp,   0*4+0x200(zero)
	sw x1,   1*4+0x200(zero)
	sw x2,   2*4+0x200(zero)
	sw x3,   3*4+0x200(zero)
	sw x4,   4*4+0x200(zero)
	sw x5,   5*4+0x200(zero)
	sw x6,   6*4+0x200(zero)
	sw x7,   7*4+0x200(zero)
	sw x8,   8*4+0x200(zero)
	sw x9,   9*4+0x200(zero)
	sw x10, 10*4+0x200(zero)
	sw x11, 11*4+0x200(zero)
	sw x12, 12*4+0x200(zero)
	sw x13, 13*4+0x200(zero)
	sw x14, 14*4+0x200(zero)
	sw x15, 15*4+0x200(zero)
	sw x16, 16*4+0x200(zero)
	sw x17, 17*4+0x200(zero)
	sw x18, 18*4+0x200(zero)
	sw x19, 19*4+0x200(zero)
	sw x20, 20*4+0x200(zero)
	sw x21, 21*4+0x200(zero)
	sw x22, 22*4+0x200(zero)
	sw x23, 23*4+0x200(zero)
	sw x24, 24*4+0x200(zero)
	sw x25, 25*4+0x200(zero)
	sw x26, 26*4+0x200(zero)
	sw x27, 27*4+0x200(zero)
	sw x28, 28*4+0x200(zero)
	sw x29, 29*4+0x200(zero)
	sw x30, 30*4+0x200(zero)
	sw x31, 31*4+0x200(zero)
#endif

#endif // ENABLE_QREGS

	/* call interrupt handler C function */

	lui sp, %hi(irq_stack)
	addi sp, sp, %lo(irq_stack)

	// arg0 = address of regs
	lui a0, %hi(irq_regs)
	addi a0, a0, %lo(irq_regs)

	// arg1 = interrupt type
#ifdef ENABLE_QREGS
	picorv32_getq_insn(a1, q1)
#else
	addi a1, tp, 0
#endif

	// call to C function
	jal ra, irq

	/* restore registers */

#ifdef ENABLE_QREGS

	// new irq_regs address returned from C code in a0
	addi x1, a0, 0

	lw x2,   0*4(x1)
	picorv32_setq_insn(q0, x2)

	lw x2,   1*4(x1)
	picorv32_setq_insn(q1, x2)

	lw x2,   2*4(x1)
	picorv32_setq_insn(q2, x2)

#ifdef ENABLE_FASTIRQ
	lw x5,   5*4(x1)
	lw x6,   6*4(x1)
	lw x7,   7*4(x1)
	lw x10, 10*4(x1)
	lw x11, 11*4(x1)
	lw x12, 12*4(x1)
	lw x13, 13*4(x1)
	lw x14, 14*4(x1)
	lw x15, 15*4(x1)
	lw x16, 16*4(x1)
	lw x17, 17*4(x1)
	lw x28, 28*4(x1)
	lw x29, 29*4(x1)
	lw x30, 30*4(x1)
	lw x31, 31*4(x1)
#else
	lw x3,   3*4(x1)
	lw x4,   4*4(x1)
	lw x5,   5*4(x1)
	lw x6,   6*4(x1)
	lw x7,   7*4(x1)
	lw x8,   8*4(x1)
	lw x9,   9*4(x1)
	lw x10, 10*4(x1)
	lw x11, 11*4(x1)
	lw x12, 12*4(x1)
	lw x13, 13*4(x1)
	lw x14, 14*4(x1)
	lw x15, 15*4(x1)
	lw x16, 16*4(x1)
	lw x17, 17*4(x1)
	lw x18, 18*4(x1)
	lw x19, 19*4(x1)
	lw x20, 20*4(x1)
	lw x21, 21*4(x1)
	lw x22, 22*4(x1)
	lw x23, 23*4(x1)
	lw x24, 24*4(x1)
	lw x25, 25*4(x1)
	lw x26, 26*4(x1)
	lw x27, 27*4(x1)
	lw x28, 28*4(x1)
	lw x29, 29*4(x1)
	lw x30, 30*4(x1)
	lw x31, 31*4(x1)
#endif

	picorv32_getq_insn(x1, q1)
	picorv32_getq_insn(x2, q2)

#else // ENABLE_QREGS

	// new irq_regs address returned from C code in a0
	addi a1, zero, 0x200
	beq a0, a1, 1f
	ebreak
1:

#ifdef ENABLE_FASTIRQ
	lw gp,   0*4+0x200(zero)
	lw x1,   1*4+0x200(zero)
	lw x2,   2*4+0x200(zero)
	lw x5,   5*4+0x200(zero)
	lw x6,   6*4+0x200(zero)
	lw x7,   7*4+0x200(zero)
	lw x10, 10*4+0x200(zero)
	lw x11, 11*4+0x200(zero)
	lw x12, 12*4+0x200(zero)
	lw x13, 13*4+0x200(zero)
	lw x14, 14*4+0x200(zero)
	lw x15, 15*4+0x200(zero)
	lw x16, 16*4+0x200(zero)
	lw x17, 17*4+0x200(zero)
	lw x28, 28*4+0x200(zero)
	lw x29, 29*4+0x200(zero)
	lw x30, 30*4+0x200(zero)
	lw x31, 31*4+0x200(zero)
#else
	lw gp,   0*4+0x200(zero)
	lw x1,   1*4+0x200(zero)
	lw x2,   2*4+0x200(zero)
	// do not restore x3 (gp)
	lw x4,   4*4+0x200(zero)
	lw x5,   5*4+0x200(zero)
	lw x6,   6*4+0x200(zero)
	lw x7,   7*4+0x200(zero)
	lw x8,   8*4+0x200(zero)
	lw x9,   9*4+0x200(zero)
	lw x10, 10*4+0x200(zero)
	lw x11, 11*4+0x200(zero)
	lw x12, 12*4+0x200(zero)
	lw x13, 13*4+0x200(zero)
	lw x14, 14*4+0x200(zero)
	lw x15, 15*4+0x200(zero)
	lw x16, 16*4+0x200(zero)
	lw x17, 17*4+0x200(zero)
	lw x18, 18*4+0x200(zero)
	lw x19, 19*4+0x200(zero)
	lw x20, 20*4+0x200(zero)
	lw x21, 21*4+0x200(zero)
	lw x22, 22*4+0x200(zero)
	lw x23, 23*4+0x200(zero)
	lw x24, 24*4+0x200(zero)
	lw x25, 25*4+0x200(zero)
	lw x26, 26*4+0x200(zero)
	lw x27, 27*4+0x200(zero)
	lw x28, 28*4+0x200(zero)
	lw x29, 29*4+0x200(zero)
	lw x30, 30*4+0x200(zero)
	lw x31, 31*4+0x200(zero)
#endif

#endif // ENABLE_QREGS

	picorv32_retirq_insn()

#ifndef ENABLE_QREGS
.balign 0x200
#endif
irq_regs:
	// registers are saved to this memory region during interrupt handling
	// the program counter is saved as register 0
	.fill 32,4

	// stack for the interrupt handler
	.fill 128,4
irq_stack:

start:

jal ra, entry

/* break */
ebreak

