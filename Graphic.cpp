#include "Graphic.h"
#include "resource.h"
#include <iostream>


Graphic::Graphic(HINSTANCE hInstance, int nCmdShow) {
    // Loading data
    this->myRun.Init();
    this->myStartup.Init();
    this->myTaskScheduler.Init();
    this->myService.Init();

    // Loading window
    this->hInstance = hInstance;
    this->nCmdShow = nCmdShow;
    this->hWindow = nullptr;
}


LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	// interaction before giving to windows native execution
    switch (uMsg) {
    case WM_CLOSE:
        DestroyWindow(hwnd);
        return 0;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }


	return DefWindowProcW(hwnd, uMsg, wParam, lParam);
}

void Graphic::Init() {

	// Registrer window class
	WNDCLASSEXW windowClass = {};
    windowClass.cbSize = sizeof(WNDCLASSEXW);
    windowClass.style = CS_HREDRAW | CS_VREDRAW;
    windowClass.lpfnWndProc = WindowProc;
    windowClass.cbClsExtra = 0;
    windowClass.cbWndExtra = 0;
    windowClass.hInstance = this->hInstance;
    windowClass.hIcon = LoadIconW(hInstance, MAKEINTRESOURCEW(IDI_ICON1));
    windowClass.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    windowClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    windowClass.lpszMenuName = nullptr;
    windowClass.lpszClassName = L"PhoenixWindowClass";
    windowClass.hIconSm = LoadIconW(hInstance, MAKEINTRESOURCEW(IDI_ICON1));

	ATOM check = RegisterClassExW(&windowClass);
	if (!check) {
		std::cout << "Error Register" << std::endl;
		return;
	}


    // Create instance window
    HWND hWindow = CreateWindowExW(
        0,
        L"PhoenixWindowClass",
        L"Phoenix",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        800,
        600,
        nullptr,
        nullptr,
        this->hInstance,
        nullptr
    );
    if (hWindow == nullptr) {
        std::cout << "Error Create Window" << std::endl;
        return;
    }

    this->hWindow = hWindow;
}

typedef struct DisplayRow {
    std::wstring name;
    std::wstring value;
};

void Graphic::Build() {

    std::vector<DisplayRow> myFirstDisplay;
    struct DisplayRow row;
    row.name = L"Run HKCU";
    row.value = this->myRun.getCountHKCU();
    myFirstDisplay.push_back(row);

    row.name = L"Run HKLM";
    row.value = this->myRun.getCountHKLM();
    myFirstDisplay.push_back(row);

    row.name = L"Startup User";
    row.value = this->myStartup.getCountUser();
    myFirstDisplay.push_back(row);

    row.name = L"Startup Machine";
    row.value = this->myStartup.getCountMachine();
    myFirstDisplay.push_back(row);

    row.name = L"TaskScheduler";
    row.value = this->myTaskScheduler.getCount();
    myFirstDisplay.push_back(row);

    row.name = L"Service User";
    row.value = this->myService.getCountUser();
    myFirstDisplay.push_back(row);

    row.name = L"Service Kernel";
    row.value = this->myService.getCountKernel();
    myFirstDisplay.push_back(row);

    for (int i = 0;i < 7;i++) {
        HWND hStatic = CreateWindowExW(
            0,
            L"STATIC",
            myFirstDisplay[i].name.c_str(),
            WS_CHILD | WS_VISIBLE,
            30,
            30 + (i * 30),
            180,
            25,
            this->hWindow,
            nullptr,
            this->hInstance,
            nullptr
        );

        HWND hStatic2 = CreateWindowExW(
            0,
            L"STATIC",
            myFirstDisplay[i].value.c_str(),
            WS_CHILD | WS_VISIBLE | SS_RIGHT,
            220,
            30 + (i * 30),
            60,
            25,
            this->hWindow,
            nullptr,
            this->hInstance,
            nullptr
        );
    }

}



void Graphic::Print() {
    ShowWindow(this->hWindow, this->nCmdShow);

    BOOL check = UpdateWindow(this->hWindow);
    if (!check) {
        std::cout << "Error Update Window" << std::endl;
        return;
    }
}


void Graphic::Message() {
    MSG msg = {};

    while (GetMessageW(&msg, nullptr, 0, 0) > 0)
    {
        TranslateMessage(&msg);
        DispatchMessageW(&msg); // Call our WindowProc()
    }
}