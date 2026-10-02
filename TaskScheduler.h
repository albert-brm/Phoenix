#pragma once
#include "Persistance.h"
#include <vector>
#include <string>
#include <map>
#include <unordered_map>
#include "Windows.h"
#include <taskschd.h>
#include <comdef.h>
#include <iostream>


// Doc TaskScheduler

// We take the root Folder of TaskScheduler "\\"
// Then we load it's tasks
// Recursivly we browse each subfolder to load it's tasks
// For each task we load : basic attribut (name,path,enabled,state) et defintion attribut (actions,triggers,principal)


typedef struct action {
	int type;
	std::vector<std::wstring> parameters;
};

typedef struct trigger {
	int type;
	bool enabled;
	std::wstring start;
	std::wstring end;
};

typedef struct principal {
	std::wstring userId;
	int logonType;
	int runLevel;
};

typedef struct definition {
	std::vector<struct action> actions;
	std::vector<struct trigger> triggers;
	struct principal princ;
};

typedef struct task {
	std::wstring name;
	std::wstring path;
	bool enabled;
	int state;
	struct definition def;
};

typedef struct folder {
	std::vector<task> myTasks;
};


class TaskScheduler : public Persistance {
public:
	TaskScheduler();

	void Init() override;
	void GetCount() override;
	void GetAll() override;

	std::wstring getCount();

private:
	ITaskService* initCom();
	void LoadBaseTask(IRegisteredTask* myTask, struct task& storeTask);
	void LoadActions(ITaskDefinition* taskDefinition, std::vector<struct action>& storeActions);
	void LoadTriggers(ITaskDefinition* taskDefinition, std::vector<struct trigger>& storeTriggers);
	void LoadPrincipal(ITaskDefinition* taskDefinition, struct principal& storePrincipal);
	void LoadDefinition(IRegisteredTask* myTask, struct task& storeTask);
	void LoadTasks(ITaskFolder* myTask);
	void BrowseFolderRecurs(ITaskFolderCollection* collectionRootFolder);

	int count;
	std::map<std::wstring, struct folder> taskSchedulerFolders; // key : folderPath | element : folder

};