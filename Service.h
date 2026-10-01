#pragma once
#include <string>
#include <map>
#include <Windows.h>
#include "Persistance.h"

typedef struct service {
	std::wstring serviceName;
	int serviceType;
	int startType;
	std::wstring binaryPath;
	std::wstring pathDllService; // if the service is launch by svchost.exe, we can see which dll is launch
};

class Service : public Persistance {
public:
	Service();
	void Init() override;
	void GetCount() override;
	void GetAll() override;
private:
	void LoadServices(DWORD serviceType);
	void printService(struct service myService);

	int countUserServices;
	std::map<std::wstring, struct service> servicesUser; // Key : lpServiceNamed (unique) | Data : service struct contains all data needed 

	int countKernelServices;
	std::map<std::wstring, struct service> servicesKernel;
};
