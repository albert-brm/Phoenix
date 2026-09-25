#include "Run.h"
#include <stdio.h>
#include <string>

int wmain(int argc, wchar_t* argv[]) {
	
	Run persistance;
	persistance.Init();
	printf("Count total : %d\n", persistance.GetCount());
	persistance.GetAll();

	return 0;
}