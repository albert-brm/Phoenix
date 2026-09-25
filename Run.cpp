#include "Run.h"
#include <Windows.h>

Run::Run() {
	this->hRunHKCU = NULL;
	this->hRunHKLM = NULL;
	this->countHKCU = 0;
	this->countHKLM = 0;
}

bool Run::LoadValues(HKEY hRun) {
	std::wstring select = L"";
	if (hRun == HKEY_CURRENT_USER) {
		select = L"HKCU";
	}
	else if (hRun == HKEY_LOCAL_MACHINE) {
		select = L"HKLM";
	}
	else {
		return false;
	}

	LSTATUS status = RegOpenKeyExW(
		hRun,
		L"Software\\Microsoft\\Windows\\CurrentVersion\\Run",
		0,
		KEY_READ,
		&hRun
	);
	if (status != ERROR_SUCCESS) {
		printf("error status 1\n");
		return false;
	}


	// Count
	DWORD index = 0;
	while (true) {
		wchar_t valueName[256];
		DWORD sizeValue = _countof(valueName);
		DWORD type = 0;
		BYTE data[1024];
		DWORD sizeData = sizeof(data);

		status = RegEnumValueW(
			hRun,
			index,
			valueName,
			&sizeValue,
			0,
			&type,
			data,
			&sizeData
		);

		if (status == ERROR_NO_MORE_ITEMS) {
			break;
		}
		else if (status != ERROR_SUCCESS) {
			return false;
		}

		// Store
		struct value myValue {};
		myValue.name = valueName;
		myValue.type = type;
		myValue.data.assign(data, data + sizeData);

		if (select == L"HKCU") {
			this->myValuesHKCU.push_back(myValue);
		}
		else if (select == L"HKLM") {
			this->myValuesHKLM.push_back(myValue);
		}

		index++;
	}

	// Store handle
	if (select == L"HKCU") {
		this->hRunHKCU = hRun;
		this->countHKCU = index;
	}
	else if (select == L"HKLM") {
		this->hRunHKLM = hRun;
		this->countHKLM = index;
	}


	return true;
}

void Run::Init() {
	// Loading data

	// HKCU
	if (LoadValues(HKEY_CURRENT_USER)) {
		printf("HKCU Run Key Load\n");
	}
	else {
		printf("HKCU Run Key Load Error\n");
	}
	// HKLM
	if (LoadValues(HKEY_LOCAL_MACHINE)) {
		printf("HKLM Run Key Load\n");
	}
	else {
		printf("HKLM Run Key Load Error\n");
	}

}

int Run::GetCount() {
	return (this->countHKCU + this->countHKLM);
}

void Run::printValue(const value& myValue) {
	wprintf(
		L"Name : %ls\n"
		L"Type : %lu\n", myValue.name.c_str(), myValue.type
	);

	if (myValue.type == REG_SZ || myValue.type == REG_EXPAND_SZ) {
		wprintf(
			L"Data : %ls\n", reinterpret_cast<const wchar_t*>(myValue.data.data())
		);
	}
}

void Run::GetAll() {
	// Brower and print
	printf("\nHKCU\n");
	for (int i = 0;i < this->countHKCU;i++) {
		printf("*** Element %d ***\n", i);
		printValue(this->myValuesHKCU[i]);
	}
	printf("\nHKLM\n");
	for (int i = 0;i < this->countHKLM;i++) {
		printf("*** Element %d ***\n", i);
		printValue(this->myValuesHKLM[i]);
	}
}


