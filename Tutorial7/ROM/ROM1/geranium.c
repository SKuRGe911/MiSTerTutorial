#include "config.h"
//////////////////////////////////////////////////////////////////////////
bool restart_loop=false;
//////////////////////////////////////////////////////////////////////////
uint32_t get_status(void) {

	return READIO(CPU_IO_STATUS);

}
//////////////////////////////////////////////////////////////////////////
void nop(void){
	__asm("nop");
}
//////////////////////////////////////////////////////////////////////////