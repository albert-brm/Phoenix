#include <stdio.h>
#include <string>
#include "Run.h"
#include "Startup.h"
#include "TaskScheduler.h"

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
	printf("\n\n");
	TaskScheduler persistance3;
	persistance3.Init();
	persistance3.GetAll();
	
	
	

	return 0;
}