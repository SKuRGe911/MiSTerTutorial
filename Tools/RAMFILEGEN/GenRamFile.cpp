
#include <iostream>
#include <fstream>
#include <iomanip>

using namespace std;

int main()
{
	ofstream file;
	file.open(L"sram.vmem", ios::out | ios::trunc);
	unsigned short int val=0;

	for (unsigned int a = 0; a < (4194304*4)-1; a++) {
		file << hex << setw(4) << setfill('0') << val << '\n';
		val = val + 1;
		if (val == -1) {
			val = 0;
		}
	}
	file.close();
	
	return 0;

}

