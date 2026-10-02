#include <stdio.h>
#include <string>
#include "Run.h"
#include "Startup.h"
#include "TaskScheduler.h"
#include "Service.h"
#include "Graphic.h"

// MAIN UI APPLICATION
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {
	// Load Data

	Graphic myApplication(hInstance, nCmdShow);
	myApplication.Init();
	myApplication.Build();
	myApplication.Print();
	myApplication.Message();

	return 0;
}