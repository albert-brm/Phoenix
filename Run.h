#pragma once
#include "Persistance.h"
#include <string>
#include <vector>
#include <Windows.h>

// Doc Run

// We load Run key in HKCU and HKLM register.
// For each input we load the 3 elements : name, type, data


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
	void GetCount() override;
	void GetAll() override;

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