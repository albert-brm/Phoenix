#include "Startup.h"


Startup::Startup() {
	this->countUser = 0;
	this->countMachine = 0;
}


bool Startup::LoadLnkInformations(std::wstring lnkPath, struct app* myPtrApp) {
	// Init COM 
	HRESULT hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
	if (FAILED(hr)) {
		printf("Load LNK failed\n");
		return false;
	}

	IShellLinkW* shellLink = nullptr;
	// Create instance COM for ShellLink
	hr = CoCreateInstance(
		CLSID_ShellLink,
		nullptr,
		CLSCTX_INPROC_SERVER,
		IID_IShellLinkW,
		(void**)&shellLink
	);
	if (FAILED(hr)) {
		printf("Load LNK failed\n");
		return false;
	}

	IPersistFile* persistFile = nullptr;
	// Query for a File
	hr = shellLink->QueryInterface(
		IID_IPersistFile,
		(void**)&persistFile
	);
	if (FAILED(hr)) {
		printf("Load LNK failed\n");
		return false;
	}

	// Load this file
	hr = persistFile->Load(lnkPath.c_str(), STGM_READ);
	if (FAILED(hr)) {
		printf("Load LNK failed\n");
		return false;
	}

	// Path execution
	std::wstring myPath;
	myPath.resize(256);
	hr = shellLink->GetPath(myPath.data(), static_cast<int>(myPath.size()), nullptr, SLGP_RAWPATH);
	if (SUCCEEDED(hr)) {
		myPath.resize(wcslen(myPath.c_str()));
		myPtrApp->path = myPath;
	}

	// Arguments
	std::wstring myArgs;
	myArgs.resize(256);
	hr = shellLink->GetArguments(myArgs.data(), static_cast<int>(myArgs.size()));
	if (SUCCEEDED(hr)) {
		myArgs.resize(wcslen(myArgs.c_str()));
		myPtrApp->arguments = myArgs;
	}

	// Working Directory
	std::wstring myWorkDir;
	myWorkDir.resize(256);
	hr = shellLink->GetWorkingDirectory(myWorkDir.data(), static_cast<int>(myWorkDir.size()));
	if (SUCCEEDED(hr)) {
		myWorkDir.resize(wcslen(myWorkDir.c_str()));
		myPtrApp->workingDirectory = myWorkDir;
	}

	return true;
}


// Loading name and type of all files form a path in arguments
bool Startup::LoadFilesFromPath(std::wstring pathSrc, std::wstring startupLocation) {
	std::wstring path;
	path.resize(256);
	DWORD expand = ExpandEnvironmentStringsW(pathSrc.c_str(), path.data(), path.size());
	if (expand == 0) {
		DWORD err = GetLastError();
		printf("Error ExpandPath de %ls : %lu\n", pathSrc.c_str(), err);
		return false;
	}

	_WIN32_FIND_DATAW fileData;
	HANDLE hSearchFile = FindFirstFileW(
		path.c_str(),
		&fileData
	);
	if (hSearchFile == INVALID_HANDLE_VALUE) {
		DWORD err = GetLastError();
		printf("Error FindFirstFile de %ls : %lu\n", path.c_str(), err);
		return false;
	}

	// load first file
	if (!(fileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
		struct app myApp;
		std::wstring fullName = fileData.cFileName;
		myApp.name = fullName;
		myApp.attribute = fileData.dwFileAttributes;
		size_t pos = fullName.rfind(L".");
		if (pos != std::wstring::npos) {
			std::wstring type = fullName.substr(pos);
			myApp.type = type;
			// lnk informations
			if (type == L".lnk") {
				struct app* myPtrApp = &myApp;
				std::wstring lnkPath = path + fullName;
				LoadLnkInformations(lnkPath, myPtrApp);
			}
		}
		// Add data
		if (startupLocation == L"USER") {
			this->myStartupAppsUser.push_back(myApp);
			this->countUser += 1;
		}
		else if (startupLocation == L"MACHINE") {
			this->myStartupAppsMachine.push_back(myApp);
			this->countMachine += 1;
		}
	}

	bool status = true;

	while (true) {
		status = FindNextFileW(
			hSearchFile,
			&fileData
		);

		if (status) {
			// load next file
			if (!(fileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
				struct app myApp;
				std::wstring fullName = fileData.cFileName;
				myApp.name = fullName;
				myApp.attribute = fileData.dwFileAttributes;
				size_t pos = fullName.rfind(L".");
				if (pos != std::wstring::npos) {
					std::wstring type = fullName.substr(pos);
					myApp.type = type;
					// lnk informations
					if (type == L".lnk") {
						struct app* myPtrApp = &myApp;
						size_t pos2 = path.rfind(L"*");
						if (pos2 != std::wstring::npos) {
							std::wstring dirPath = path.substr(0, pos2);
							std::wstring lnkPath = dirPath + fullName;
							LoadLnkInformations(lnkPath, myPtrApp);
						}
					}
				}

				// Add data
				if (startupLocation == L"USER") {
					this->myStartupAppsUser.push_back(myApp);
					this->countUser += 1;
				}
				else if (startupLocation == L"MACHINE") {
					this->myStartupAppsMachine.push_back(myApp);
					this->countMachine += 1;
				}
			}
		}
		else {
			DWORD err = GetLastError();
			if (err == ERROR_NO_MORE_FILES) {
				break;
			}
			else {
				printf("Error FindNextFile : %du\n", err);
				FindClose(hSearchFile);
				return false;
			}
		}
	}

	FindClose(hSearchFile);
	return true;
}




// Loading data from Startup folder
void Startup::Init() {
	// User
	if (LoadFilesFromPath(L"%APPDATA%\\Microsoft\\Windows\\Start Menu\\Programs\\Startup\\*", L"USER")) {
		printf("Load Startup User\n");
	}
	else {
		printf("Load Startup User failed\n");
	}
	// Computer
	if (LoadFilesFromPath(L"%ProgramData%\\Microsoft\\Windows\\Start Menu\\Programs\\Startup\\*", L"MACHINE")) {
		printf("Load Startup Machine\n");
	}
	else {
		printf("Load Startup Machine failed\n");
	}

	// Load data for LNK files
}


void Startup::GetCount() {
	printf("\nTotal Application : %d\n", (this->countUser+this->countMachine));
}

void Startup::printApp(struct app myApp) {
	printf("Name : %ls\nAttribute : %lu\nType : %ls\n", myApp.name.c_str(), myApp.attribute, myApp.type.c_str());
	if (myApp.type == L".lnk") {
		printf("Path execution : %ls\nArguments : %ls\nWorking directory : %ls\n",myApp.path.c_str(),myApp.arguments.c_str(),myApp.workingDirectory.c_str());
	}
}

void Startup::GetAll() {
	printf("\nStartup User\n");
	for (int i = 0;i < this->countUser;i++) {
		printf("*** Element %d ***\n", i);
		printApp(this->myStartupAppsUser[i]);
	}
	printf("\nStartup Machine\n");
	for (int i = 0;i < this->countMachine;i++) {
		printf("*** Element %d ***\n", i);
		printApp(this->myStartupAppsMachine[i]);
	}
}