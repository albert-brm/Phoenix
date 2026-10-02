#include "TaskScheduler.h"

TaskScheduler::TaskScheduler() {
	this->count = 0;
}




ITaskService* TaskScheduler::initCom() {
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




void TaskScheduler::LoadBaseTask(IRegisteredTask* myTask, struct task& storeTask) {
	// name
	BSTR name = nullptr;
	HRESULT hr2 = myTask->get_Name(&name);
	if (FAILED(hr2)) {
		printf("Error get name in task\n");
		return;
	}
	storeTask.name = static_cast<std::wstring>(name);
	// path
	BSTR path = nullptr;
	hr2 = myTask->get_Path(&path);
	if (FAILED(hr2)) {
		printf("Error get path in task\n");
		SysFreeString(name);
		return;
	}
	storeTask.path = static_cast<std::wstring>(path);
	// enabled
	VARIANT_BOOL enabled;
	hr2 = myTask->get_Enabled(&enabled);
	if (FAILED(hr2)) {
		printf("Error get path in task\n");
		SysFreeString(name);
		SysFreeString(path);
		return;
	}
	storeTask.enabled = static_cast<bool>(enabled);
	// state
	TASK_STATE state;
	hr2 = myTask->get_State(&state);
	if (FAILED(hr2)) {
		printf("Error get state in task\n");
		SysFreeString(name);
		SysFreeString(path);
		return;
	}
	storeTask.state = static_cast<int>(state);
	SysFreeString(name);
	SysFreeString(path);
}





void TaskScheduler::LoadActions(ITaskDefinition* taskDefinition, std::vector<struct action>& storeActions) {
	IActionCollection* collectionAction = nullptr;
	HRESULT hr2 = taskDefinition->get_Actions(&collectionAction);
	if (FAILED(hr2)) {
		std::wcout << "Error get actions\n" << std::endl;
		return;
	}
	LONG countAction = 0;
	hr2 = collectionAction->get_Count(&countAction);
	if (FAILED(hr2)) {
		std::wcout << "Error get actions\n" << std::endl;
		collectionAction->Release();
		return;
	}

	// Browse all actions in the task
	for (LONG i = 1;i <= countAction;i++) {
		IAction* myAction = nullptr;
		hr2 = collectionAction->get_Item(i, &myAction);
		if (FAILED(hr2)) {
			return;
		}
		TASK_ACTION_TYPE type;
		hr2 = myAction->get_Type(&type);
		if (FAILED(hr2)) {
			return;
		}

		// prepare loading actions in task
		struct action storeAction;
		storeAction.type = static_cast<int>(type);

		// EXEC
		if (type == TASK_ACTION_EXEC) {
			IExecAction* myExec = nullptr;
			hr2 = myAction->QueryInterface(
				IID_IExecAction,
				reinterpret_cast<void**>(&myExec)
			);
			if (FAILED(hr2)) {
				return;
			}
			BSTR path = nullptr;
			hr2 = myExec->get_Path(&path);
			if (FAILED(hr2)) {
				myExec->Release();
				return;
			}
			if (path != nullptr) {
				storeAction.parameters.push_back(static_cast<std::wstring>(path));
			}

			BSTR arguments = nullptr;
			hr2 = myExec->get_Arguments(&arguments);
			if (FAILED(hr2)) {
				SysFreeString(path);
				myExec->Release();
				return;
			}
			if (arguments != nullptr) {
				storeAction.parameters.push_back(static_cast<std::wstring>(arguments));
			}

			BSTR workingDirectory = nullptr;
			hr2 = myExec->get_WorkingDirectory(&workingDirectory);
			if (FAILED(hr2)) {
				SysFreeString(arguments);
				SysFreeString(path);
				myExec->Release();
				return;
			}
			if (workingDirectory != nullptr) {
				storeAction.parameters.push_back(static_cast<std::wstring>(workingDirectory));
			}
			SysFreeString(workingDirectory);
			SysFreeString(arguments);
			SysFreeString(path);
			myExec->Release();
		}


		// COM HANDLER
		else if (type == TASK_ACTION_COM_HANDLER) {
			IComHandlerAction* myComHandler = nullptr;
			hr2 = myAction->QueryInterface(
				IID_IComHandlerAction,
				reinterpret_cast<void**>(&myComHandler)
			);
			if (FAILED(hr2)) {
				return;
			}
			BSTR classId = nullptr;
			hr2 = myComHandler->get_ClassId(&classId);
			if (FAILED(hr2)) {
				myComHandler->Release();
				return;
			}
			if (classId != nullptr) {
				storeAction.parameters.push_back(static_cast<std::wstring>(classId));
			}

			BSTR data = nullptr;
			hr2 = myComHandler->get_Data(&data);
			if (FAILED(hr2)) {
				SysFreeString(classId);
				myComHandler->Release();
				return;
			}
			if (data != nullptr) {
				storeAction.parameters.push_back(static_cast<std::wstring>(data));
			}
			SysFreeString(classId);
			SysFreeString(data);
			myComHandler->Release();
		}
		// add in vector action
		storeActions.push_back(storeAction);

		myAction->Release();
	}

	collectionAction->Release();
}







void TaskScheduler::LoadTriggers(ITaskDefinition* taskDefinition, std::vector<struct trigger>& storeTriggers) {
	ITriggerCollection* collectionTrigger = nullptr;
	HRESULT hr2 = taskDefinition->get_Triggers(&collectionTrigger);
	if (FAILED(hr2)) {
		return;
	}
	LONG countTrigger = 0;
	hr2 = collectionTrigger->get_Count(&countTrigger);
	if (FAILED(hr2)) {
		collectionTrigger->Release();
		return;
	}
	for (LONG i = 1; i <= countTrigger; i++) {
		struct trigger storeTrigger;

		ITrigger* myTrigger = nullptr;
		hr2 = collectionTrigger->get_Item(i, &myTrigger);
		if (FAILED(hr2)) {
			return;
		}

		TASK_TRIGGER_TYPE2 type;
		hr2 = myTrigger->get_Type(&type);
		if (FAILED(hr2)) {
			myTrigger->Release();
			return;
		}
		storeTrigger.type = static_cast<int>(type);

		VARIANT_BOOL enabled;
		hr2 = myTrigger->get_Enabled(&enabled);
		if (FAILED(hr2)) {
			myTrigger->Release();
			return;
		}
		storeTrigger.enabled = static_cast<bool>(enabled);

		BSTR start = nullptr;
		hr2 = myTrigger->get_StartBoundary(&start);
		if (FAILED(hr2)) {
			myTrigger->Release();
			return;
		}
		if (start != nullptr) {
			storeTrigger.start = static_cast<std::wstring>(start);
		}

		BSTR end = nullptr;
		hr2 = myTrigger->get_EndBoundary(&end);
		if (FAILED(hr2)) {
			SysFreeString(start);
			myTrigger->Release();
			return;
		}
		if (end != nullptr) {
			storeTrigger.end = static_cast<std::wstring>(end);
		}

		storeTriggers.push_back(storeTrigger);
		SysFreeString(end);
		SysFreeString(start);
		myTrigger->Release();
	}
	collectionTrigger->Release();
}






void TaskScheduler::LoadPrincipal(ITaskDefinition* taskDefinition, struct principal& storePrincipal) {
	IPrincipal* myPrincipal = nullptr;
	HRESULT hr2 = taskDefinition->get_Principal(&myPrincipal);
	if (FAILED(hr2)) {
		return;
	}

	BSTR userId = nullptr;
	hr2 = myPrincipal->get_UserId(&userId);
	if (FAILED(hr2)) {
		myPrincipal->Release();
		return;
	}
	if (userId != nullptr) {
		storePrincipal.userId = static_cast<std::wstring>(userId);
	}

	TASK_LOGON_TYPE logonType;
	hr2 = myPrincipal->get_LogonType(&logonType);
	if (FAILED(hr2)) {
		SysFreeString(userId);
		myPrincipal->Release();
		return;
	}
	storePrincipal.logonType = static_cast<int>(logonType);

	TASK_RUNLEVEL_TYPE runLevel;
	hr2 = myPrincipal->get_RunLevel(&runLevel);
	if (FAILED(hr2)) {
		SysFreeString(userId);
		myPrincipal->Release();
		return;
	}
	storePrincipal.runLevel = static_cast<int>(runLevel);
	SysFreeString(userId);
	myPrincipal->Release();
}




void TaskScheduler::LoadDefinition(IRegisteredTask* myTask, struct task& storeTask) {
	// definition
	ITaskDefinition* taskDefinition = nullptr;
	HRESULT hr2 = myTask->get_Definition(&taskDefinition);
	if (FAILED(hr2)) {
		printf("Error get state in task\n");
		return;
	}
	// build defintion struct to add it in the task data
	struct definition myDef;

	// Load actions and add in definition
	std::vector<struct action> storeActions;
	LoadActions(taskDefinition, storeActions);
	myDef.actions = storeActions;

	// Load triggers and add in definition
	std::vector<struct trigger> storeTriggers;
	LoadTriggers(taskDefinition, storeTriggers);
	myDef.triggers = storeTriggers;

	// Load prinicpal and add definition
	struct principal storePrincipal;
	LoadPrincipal(taskDefinition, storePrincipal);
	myDef.princ = storePrincipal;

	// add defintion in task
	storeTask.def = myDef;
	taskDefinition->Release();
}





void TaskScheduler::LoadTasks(ITaskFolder* myFolder) {
	// get collection tasks
	IRegisteredTaskCollection* collectionTask = nullptr;
	HRESULT hr2 = myFolder->GetTasks(TASK_ENUM_HIDDEN, &collectionTask);
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

	// get folderPath for key map
	BSTR folderPath = nullptr;
	hr2 = myFolder->get_Path(&folderPath);
	if (FAILED(hr2)) {
		printf("Error get name folder\n");
		return;
	}

	// create structure for loading
	struct folder storeFolder;
	std::vector<struct task> storeTasks;

	// Loading tasks
	for (LONG j = 1;j <= countTasks;j++) {
		IRegisteredTask* myTask = nullptr;

		// get task
		hr2 = collectionTask->get_Item(_variant_t(j), &myTask);
		if (FAILED(hr2)) {
			printf("Error get item in collection tasks\n");
			continue;
		}
		// loading data of the task
		struct task storeTask;
		LoadBaseTask(myTask, storeTask);
		LoadDefinition(myTask, storeTask);

		// add task in vector of tasks
		storeTasks.push_back(storeTask);

		this->count += 1;

		myTask->Release();
	}
	collectionTask->Release();

	// add vector tasks in folder
	storeFolder.myTasks = storeTasks;
	// add to the data store
	this->taskSchedulerFolders.insert_or_assign(folderPath, storeFolder); // add tasks for this folder

	SysFreeString(folderPath);
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

	// browse collection folders. Recursive until we finish 1 folder then past to the next one.
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

	ITaskService* taskService = initCom();
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
	LoadTasks(rootFolder); // if rootFolder contains tasks

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
	std::wcout << L"TaskScheduler :" << this->count << std::endl;
}

void printAction(const struct action& myAction) {
	std::wcout
		<< L"		Type : " << myAction.type << L"\n";

	for (size_t i = 0; i < myAction.parameters.size(); i++) {
		std::wcout
			<< L"		Parameter " << i + 1
			<< L" : " << myAction.parameters[i] << L"\n";
	}
}

void printTrigger(struct trigger myTrigger) {
	std::wcout
		<< L"		Type : " << myTrigger.type
		<< L"		Enabled : " << myTrigger.enabled
		<< L"		Start : " << myTrigger.start
		<< L"		End : " << myTrigger.end
		<< std::endl;
}

void printPrincipal(struct principal myPrincipal) {
	std::wcout
		<< L"		UserId : " << myPrincipal.userId
		<< L"		LogonType : " << myPrincipal.logonType
		<< L"		RunLevel : " << myPrincipal.runLevel
		<< std::endl;
}


void printTask(const struct task& myTask) {
	std::wcout
		<< L"  Name    : " << myTask.name << L"\n"
		<< L"  Path    : " << myTask.path << L"\n"
		<< L"  Enabled : " << myTask.enabled << L"\n"
		<< L"  State   : " << myTask.state << L"\n";

	std::wcout << L"\n    Actions:\n";

	for (size_t i = 0; i < myTask.def.actions.size(); i++) {
		std::wcout << L"      [" << i + 1 << L"]\n";
		printAction(myTask.def.actions[i]);
	}

	std::wcout << L"\n    Triggers:\n";

	for (size_t i = 0; i < myTask.def.triggers.size(); i++) {
		std::wcout << L"      [" << i + 1 << L"]\n";
		printTrigger(myTask.def.triggers[i]);
	}

	std::wcout << L"\n    Principal:\n";
	printPrincipal(myTask.def.princ);
}


void TaskScheduler::GetAll() {
	for (const auto& [folderPath, folder] : this->taskSchedulerFolders) {

		std::wcout
			<< L"\n==================================================\n"
			<< L"Folder : " << folderPath << L"\n"
			<< L"==================================================\n";

		if (folder.myTasks.empty()) {
			std::wcout << L"  No tasks\n";
			continue;
		}

		for (size_t i = 0; i < folder.myTasks.size(); i++) {
			std::wcout
				<< L"\n  [" << i + 1 << L"] Task\n"
				<< L"  ----------------------------------------------\n";

			printTask(folder.myTasks[i]);
		}
	}
}




std::wstring TaskScheduler::getCount() {
	return std::to_wstring(this->count);
}