#include "Service.h"
#include <iostream>
#include <vector>
#include <Windows.h>
#include <winsvc.h>


Service::Service() {
	this->countUserServices = 0;
	this->countKernelServices = 0;
}


void Service::LoadServices(DWORD serviceType) {
	// Open SCM
	SC_HANDLE hSCM = OpenSCManagerW(
		NULL,
		NULL,
		SC_MANAGER_CONNECT | SC_MANAGER_ENUMERATE_SERVICE
	);
	if (hSCM == NULL) {
		std::cout << "Error opening SC Manager\n" << std::endl;
		return;
	}

	// Ask size struct array contains all services
	DWORD bytesNeeded = 0;
	DWORD servicesReturned = 0;
	BOOL check = EnumServicesStatusExW(
		hSCM,
		SC_ENUM_PROCESS_INFO,
		serviceType,
		SERVICE_STATE_ALL,
		nullptr,
		0,
		&bytesNeeded,
		&servicesReturned,
		nullptr,
		nullptr
	);

	// Create the buffer
	BYTE* bufferEnumServices = new BYTE[bytesNeeded];

	// Get all services in buffer
	check = EnumServicesStatusExW(
		hSCM,
		SC_ENUM_PROCESS_INFO,
		serviceType,
		SERVICE_STATE_ALL,
		bufferEnumServices,
		bytesNeeded,
		&bytesNeeded,
		&servicesReturned,
		nullptr,
		nullptr
	);
	if (!check) {
		std::cout << "Error opening SC Manager\n" << std::endl;
		return;
	}

	// Array of all services
	ENUM_SERVICE_STATUS_PROCESSW* services = reinterpret_cast<ENUM_SERVICE_STATUS_PROCESSW*>(bufferEnumServices);


	for (int i = 0;i < servicesReturned;i++) {
		// for each service. OpenService, load data, add to the map.
		SC_HANDLE hService = OpenServiceW(
			hSCM,
			services[i].lpServiceName,
			SERVICE_QUERY_CONFIG | SERVICE_QUERY_STATUS
		);
		if (hService == NULL) {
			std::cout << "Error open service\n" << std::endl;
			continue;
		}
		// request bytes size
		DWORD bytes = 0;
		check = QueryServiceConfigW(
			hService,
			nullptr,
			0,
			&bytes
		);

		// request data
		BYTE* bufferAllocService = new BYTE[bytes];
		QUERY_SERVICE_CONFIGW* bufferService = reinterpret_cast<QUERY_SERVICE_CONFIGW*>(bufferAllocService);

		check = QueryServiceConfigW(
			hService,
			bufferService,
			bytes,
			&bytes
		);
		if (!check) {
			DWORD error = GetLastError();
			std::cout << "Error opening service config\nError : " << error << std::endl;
			delete[] bufferAllocService;
			CloseHandle(hService);
			continue;
		}

		// build struct service
		struct service myService;
		myService.serviceType = bufferService->dwServiceType;
		myService.startType = bufferService->dwStartType;
		if (bufferService->lpBinaryPathName != NULL) {
			myService.binaryPath = bufferService->lpBinaryPathName;
		}
		if (bufferService->lpDisplayName != NULL) {
			myService.serviceName = bufferService->lpDisplayName;
		}


		// Open registrery for service dll
		std::wstring dllServicePath = L"SYSTEM\\CurrentControlSet\\Services\\";
		dllServicePath += services[i].lpServiceName;
		dllServicePath += L"\\Parameters";

		HKEY hKey;

		LSTATUS status = RegOpenKeyExW(
			HKEY_LOCAL_MACHINE,
			dllServicePath.c_str(),
			0,
			KEY_READ,
			&hKey
		);
		// Presence of dll for this service
		if (status == ERROR_SUCCESS) {
			BYTE pathDll[512];
			DWORD sizePathDll = sizeof(pathDll);
			DWORD type = 0;
			LSTATUS status2 = RegQueryValueExW(
				hKey,
				L"ServiceDll",
				0,
				&type,
				pathDll,
				&sizePathDll
			);
			if (status2 == ERROR_SUCCESS) {
				// add
				if (type == REG_SZ || type == REG_EXPAND_SZ) {
					wchar_t* ptString = reinterpret_cast<wchar_t*>(pathDll);
					myService.pathDllService = ptString;
				}
			}
		}
		RegCloseKey(hKey);

		// add in private map
		if (serviceType == SERVICE_WIN32) {
			this->servicesUser.insert_or_assign(services[i].lpServiceName, myService);
			this->countUserServices += 1;
		}
		else if (serviceType == SERVICE_DRIVER) {
			this->servicesKernel.insert_or_assign(services[i].lpServiceName, myService);
			this->countKernelServices += 1;
		}
		
		delete[] bufferAllocService;
		CloseServiceHandle(hService);

	}

	delete[] bufferEnumServices;
	CloseServiceHandle(hSCM);
}


void Service::Init() {
	LoadServices(SERVICE_WIN32);
	LoadServices(SERVICE_DRIVER);
}



void Service::GetCount() {
	std::wcout
		<< L"Services User : " << this->countUserServices << std::endl
		<< L"Services Kernel : " << this->countKernelServices << std::endl;
}


void Service::printService(struct service myService) {
	std::wcout
		<< L"Name : " << myService.serviceName << std::endl
		<< L"PathBinary : " << myService.binaryPath << std::endl
		<< L"Type : " << myService.serviceType << std::endl
		<< L"Start : " << myService.startType << std::endl
		<< L"PathDllService : " << myService.pathDllService << std::endl;
}

void Service::GetAll() {
	std::cout << "\nUSER SERVICES" << std::endl;
	for (auto& element : this->servicesUser) {
		std::wcout << L"\nService Internal Name : " << element.first << std::endl;
		printService(element.second);
	}
	std::cout << "\nKERNEL SERVICES" << std::endl;
	for (auto& element : this->servicesKernel) {
		std::wcout << L"\nService Internal Name : " << element.first << std::endl;
		printService(element.second);
	}
}



std::wstring Service::getCountUser() {
	return std::to_wstring(this->countUserServices);
}

std::wstring Service::getCountKernel() {
	return std::to_wstring(this->countKernelServices);
}