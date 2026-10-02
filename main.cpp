#include <stdio.h>
#include <string>
#include "Run.h"
#include "Startup.h"
#include "TaskScheduler.h"
#include "Service.h"


// MAIN CLI APPLICATION
int wmain(int argc, wchar_t* argv[]) {
	
	Run run;
	Startup startup;
	TaskScheduler taskScheduler;
	Service service;

	// Load data
	run.Init();
	startup.Init();
	taskScheduler.Init();
	service.Init();

	// Print count
	run.GetCount();
	startup.GetCount();
	taskScheduler.GetCount();
	service.GetCount();
	
	//run.GetAll();
	//startup.GetAll();
	//taskScheduler.GetAll();	
	//service.GetAll();
	

	
	

	return 0;
}