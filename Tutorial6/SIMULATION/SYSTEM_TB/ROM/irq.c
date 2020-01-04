#include "config.h"
//////////////////////////////////////////////////////////////////////////
uint32_t* irq(uint32_t* regs, uint32_t irqs)
{
	//////////////////////////////////////////////////////////////////////////
	if (interupt_on==false) {
		return regs;
	}
	//////////////////////////////////////////////////////////////////////////
	
	//////////////////////////////////////////////////////////////////////////
	#if INTERUPT_VERTICALBLANK == CODE_ON
	//////////////////////////////////////////////////////////////////////////
	if ((irqs & GERANIUM_IRQ_VIDEO_SWAP) == GERANIUM_IRQ_VIDEO_SWAP) {
		// DO VERTICLE BLANKING INTERUPT STUFF HERE			
	}
	//////////////////////////////////////////////////////////////////////////
	#else
	//////////////////////////////////////////////////////////////////////////
	//(irqs = irqs);
	//////////////////////////////////////////////////////////////////////////
	#endif
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	#if INTERUPT_KEYBOARD == CODE_ON
	//////////////////////////////////////////////////////////////////////////
	if ((irqs & GERANIUM_IRQ_KEYBOARD_INPUT) == GERANIUM_IRQ_KEYBOARD_INPUT) {
		char key = ((char)READKEY() & 0x0FF);
		cmd_add_char(key);
	}
	//////////////////////////////////////////////////////////////////////////
	#else
	//////////////////////////////////////////////////////////////////////////
	//(irqs = irqs);
	//////////////////////////////////////////////////////////////////////////
	#endif
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	return regs;
	//////////////////////////////////////////////////////////////////////////

}
//////////////////////////////////////////////////////////////////////////

