#pragma once
#include "Persistance.h"
#include <string>
#include <vector>
#include <Windows.h>

// 3 elements store in key
typedef struct value {
	std::wstring name;
	DWORD type;
	std::vector<BYTE> data;
};

// HKCU & HKLM
class Run : public Persistance {
public:
	Run();

	void Init() override; // load count and list in memory, more fluent when utilisation, no more loading.
	int GetCount() override;
	void GetAll() override;
	//bool GetOnce() override; 

private:
	bool LoadValues(HKEY hRun);
	void printValue(const value& myValue);

	HKEY hRunHKCU; // keep handle pour remove mechanisme later.
	int countHKCU; // nb of HKCU
	std::vector<struct value> myValuesHKCU; // data store in memory

	HKEY hRunHKLM;
	int countHKLM; // nb of HKLM
	std::vector<struct value> myValuesHKLM; // data store in memory
};