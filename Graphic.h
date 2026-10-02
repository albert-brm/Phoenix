#pragma once
#include <Windows.h>
#include <string>
#include "Run.h"
#include "Startup.h"
#include "TaskScheduler.h"
#include "Service.h"


class Graphic {
public:
	Graphic(HINSTANCE hInstance, int nCmdShow);
	void Init();
	void Build();
	void Print();
	void Message();

private:
	Run myRun;
	Startup myStartup;
	TaskScheduler myTaskScheduler;
	Service myService;

	HINSTANCE hInstance;
	int nCmdShow;

	HWND hWindow;

};