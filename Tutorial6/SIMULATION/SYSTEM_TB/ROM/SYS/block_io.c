#include "..//geranium.h"
//////////////////////////////////////////////////////////////////////////
DSTATUS disk_initialize(void) {
	return RES_OK;
}
//////////////////////////////////////////////////////////////////////////
DRESULT disk_readp(BYTE* buff,DWORD sector,UINT offset,UINT count)
{
	static BYTE buffer[512];
	static DWORD buffer_sector = 0x1fffffff;
	DWORD* p;

	if (buffer_sector != sector) {
		BYTE cnt;

		WRITEIO(CPU_IO_SD_LBA, sector);
		WRITEIO(CPU_IO_SD_CONTROL, CPU_IO_SD_CONTROL_RD);
		uint32_t z = READIO(CPU_IO_SD_CONTROL);
		while ((z & CPU_IO_SD_CONTROL_DONE) != CPU_IO_SD_CONTROL_DONE) {
			z = READIO(CPU_IO_SD_CONTROL);
		}

		p = (DWORD*)buffer;
		for (cnt = 0; cnt < 128; cnt++) {
			*p++ = READIO(CPU_IO_SD_DATA);
		}

		buffer_sector = sector;
	}

	memcpy(buff, buffer + offset, count);

	return RES_OK;
}
//////////////////////////////////////////////////////////////////////////

