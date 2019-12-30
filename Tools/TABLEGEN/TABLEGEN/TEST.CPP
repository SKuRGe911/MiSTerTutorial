#include "GRNMPCH.H"
//////////////////////////////////////////////////////////////////////////
DWORD X_POS = 0;
DWORD Y_POS = 0;
DWORD X_LEN = 32;
DWORD Y_LEN = 32;
//////////////////////////////////////////////////////////////////////////
DWORD X_SCREEN = 256;
DWORD Y_SCREEN = 32;
//////////////////////////////////////////////////////////////////////////
DWORD IY = 0;					//SOURCE
DWORD IY_COUNT = 0;
DWORD IY_SKIP = 0;
DWORD IX = 0;					//DESTINATION
//////////////////////////////////////////////////////////////////////////
VOID BLIT(VOID) {
	DWORD X_COUNT = X_POS - X_LEN;
	if (X_COUNT < 0) {
		//DO NOTHING
	}
	else if (X_COUNT < X_LEN) {
		IY = IY + X_LEN - X_COUNT;
		IY_COUNT = X_COUNT;
		IY_SKIP = X_LEN - X_COUNT;
	}
	else if (X_COUNT >= X_LEN && X_COUNT < X_SCREEN - X_LEN) {
		IY = IY;
		IY_COUNT = X_LEN;
		IY_SKIP = 0;
	}
	else if (X_COUNT >= X_SCREEN - X_LEN && X_COUNT < X_SCREEN) {
		IY = IY;
		IY_COUNT = X_COUNT - X_SCREEN - X_LEN;
		IY_SKIP = X_LEN - IY_COUNT;
	}
	else {
		//DO NOTHING
	}
}

VOID WRITE_X_BLOCK(VOID) {

}

VOID WRITE_X_LINE(DWORD COUNT) {
	for (DWORD j = 0; j < COUNT; j++) {
		((LPVOID)IX) = ((LPVOID)IY);
		IX++;
		IY++;
	}
}

VOID SKIP_X_LINE(DWORD COUNT) {

}