#include "TaskScheduler.h"

TaskScheduler::TaskScheduler() {
	this->count = 0;
}




ITaskService* TaskScheduler::initComTaskScheduler() {
	// Init COM
	HRESULT hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
	if (FAILED(hr)) {
		printf("init COM failed\n");
		CoUninitialize();
		return NULL;
	}

	ITaskService* taskService = nullptr;

	// Create instance COM object Task Scheduler
	hr = CoCreateInstance(
		CLSID_TaskScheduler,
		nullptr,
		CLSCTX_INPROC_SERVER,
		IID_ITaskService,
		reinterpret_cast<void**>(&taskService)
	);
	if (FAILED(hr)) {
		printf("Create COM instance failed\n");
		CoUninitialize();
		return NULL;
	}

	VARIANT empty;
	VariantInit(&empty);
	// Connect our instance to Task Scheduler service
	hr = taskService->Connect(empty, empty, empty, empty);
	if (FAILED(hr)) {
		printf("Connection TaskScheduler Service failed\n");
		taskService->Release();
		CoUninitialize();
		return NULL;
	}

	return taskService;
}





void TaskScheduler::LoadTasks(ITaskFolder* myFolder) {
	// get folder name
	BSTR folderName = nullptr;
	HRESULT hr2 = myFolder->get_Name(&folderName);
	if (FAILED(hr2)) {
		printf("Error get name folder\n");
		return;
	}

	// add data
	struct folder storeFolder;
	storeFolder.folderName = std::wstring(folderName);


	// get collection tasks
	IRegisteredTaskCollection* collectionTask = nullptr;
	hr2 = myFolder->GetTasks(TASK_ENUM_HIDDEN, &collectionTask);
	if (FAILED(hr2)) {
		printf("Error opening tasks in folder\n");
		return;
	}

	// get count of tasks in collection
	LONG countTasks = 0;
	hr2 = collectionTask->get_Count(&countTasks);
	if (FAILED(hr2)) {
		printf("Error count collection tasks\n");
		collectionTask->Release();
		return;
	}

	// browse collection tasks
	for (LONG j = 1;j <= countTasks;j++) {
		IRegisteredTask* myTask = nullptr;

		// get task
		hr2 = collectionTask->get_Item(_variant_t(j), &myTask);
		if (FAILED(hr2)) {
			printf("Error get item in collection tasks\n");
			continue;
		}

		// Loading data of the task
		// name
		BSTR name = nullptr;
		hr2 = myTask->get_Name(&name);
		if (FAILED(hr2)) {
			printf("Error get name in task\n");
			myTask->Release();
			continue;
		}
		// path
		BSTR path = nullptr;
		hr2 = myTask->get_Path(&path);
		if (FAILED(hr2)) {
			printf("Error get path in task\n");
			SysFreeString(name);
			myTask->Release();
			continue;
		}
		// enabled
		VARIANT_BOOL enabled;
		hr2 = myTask->get_Enabled(&enabled);
		if (FAILED(hr2)) {
			printf("Error get path in task\n");
			SysFreeString(name);
			SysFreeString(path);
			myTask->Release();
			continue;
		}
		// state
		TASK_STATE state;
		hr2 = myTask->get_State(&state);
		if (FAILED(hr2)) {
			printf("Error get state in task\n");
			SysFreeString(name);
			SysFreeString(path);
			myTask->Release();
			continue;
		}
		// definition
		ITaskDefinition* taskDefinition = nullptr;
		hr2 = myTask->get_Definition(&taskDefinition);
		if (FAILED(hr2)) {
			printf("Error get state in task\n");
			SysFreeString(name);
			SysFreeString(path);
			myTask->Release();
			continue;
		}
		/*
		// action
		IActionCollection* collectionAction = nullptr;
		taskDefinition->get_Actions(&collectionAction);
		LONG countAction = 0;
		collectionAction->get_Count(&countAction);


		TODO
		*/


		std::wcout
			<< L"Name : " << name
			<< L" Task Path : " << path
			<< L" Enabled : " << enabled
			<< L" State : " << state
			<< L"\n";

		SysFreeString(name);
		SysFreeString(path);

		myTask->Release();
	}
	collectionTask->Release();
}





void TaskScheduler::BrowseFolderRecurs(ITaskFolderCollection* collectionRootFolder) {
	// get count folders in collection
	LONG countRootFolders = 0;
	HRESULT hr = collectionRootFolder->get_Count(&countRootFolders);
	if (FAILED(hr)) {
		printf("Access count folder collection failed\n");
		return;
	}
	// stop condtion for recursive function
	if (countRootFolders == 0) {
		return;
	}

	// browse collection folders
	for (LONG i = 1;i <= countRootFolders;i++) {

		// get folder
		ITaskFolder* myFolder = nullptr;
		hr = collectionRootFolder->get_Item(_variant_t(i), &myFolder);
		if (SUCCEEDED(hr)) {
			// Load tasks in the folder
			this->LoadTasks(myFolder);

			// Browse recursive sub folders
			ITaskFolderCollection* collectionSubFolder = nullptr;
			HRESULT hr2 = myFolder->GetFolders(0, &collectionSubFolder);
			if (FAILED(hr2)) {
				printf("Get folders failed\n");
				myFolder->Release();
				continue;
			}
			this->BrowseFolderRecurs(collectionSubFolder);

			collectionSubFolder->Release();
			myFolder->Release();
		}
	}
}

void TaskScheduler::Init() {

	ITaskService* taskService = initComTaskScheduler();
	if (taskService == NULL) {
		printf("Error initialization COM TAskScheduler\n");
		return;
	}

	// get root folder
	ITaskFolder* rootFolder = nullptr;
	HRESULT hr = taskService->GetFolder(_bstr_t(L"\\"), &rootFolder);
	if (FAILED(hr)) {
		printf("Access root folder failed\n");
		taskService->Release();
		return;
	}
	LoadTasks(rootFolder);

	// get root collection folder
	ITaskFolderCollection* collectionRootFolder = nullptr;
	hr = rootFolder->GetFolders(0, &collectionRootFolder);
	if (FAILED(hr)) {
		printf("Access folder collection failed\n");
		rootFolder->Release();
		taskService->Release();
		return;
	}

	// browse folders and tasks
	BrowseFolderRecurs(collectionRootFolder);

	// release memory
	collectionRootFolder->Release();
	rootFolder->Release();
	taskService->Release();
	CoUninitialize();
}




void TaskScheduler::GetCount() {

}


void TaskScheduler::GetAll() {


}