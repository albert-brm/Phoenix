#pragma once
#include "Persistance.h"
#include <string>
#include <vector>
#include <iostream>
#include "Windows.h"
#include <ShObjIdl.h>

// Doc Startup

// We load all files in the startup folder user and computer
// For each file : we load basics attribut (name,attribute,type) and when the file is .lnk type we load : path execution, arguments, working directory.

typedef struct app { // .exe or .lnk
	std::wstring name;
	DWORD attribute;
	std::wstring type;
	std::wstring path;
	std::wstring arguments;
	std::wstring workingDirectory;
};

class Startup : public Persistance {
public:
	Startup();

	void Init() override;
	void GetCount() override;
	void GetAll() override;

	std::wstring getCountUser();
	std::wstring getCountMachine();

private:
	int countUser; // list app
	std::vector<app> myStartupAppsUser; // informations app

	int countMachine; // list app
	std::vector<app> myStartupAppsMachine; // informations app

	bool LoadFilesFromPath(std::wstring pathSrc, std::wstring startupLocation);
	bool LoadLnkInformations(std::wstring lnkPath, struct app* myPtrApp);
	void printApp(struct app myApp);
};