#include "Run.h"
#include "Startup.h"
#include <stdio.h>
#include <string>

int wmain(int argc, wchar_t* argv[]) {
	
	Run persistance;
	persistance.Init();
	persistance.GetCount();
	persistance.GetAll();
	printf("\n\n");
	Startup persistance2;
	persistance2.Init();
	persistance2.GetCount();
	persistance2.GetAll();
	

	return 0;
}