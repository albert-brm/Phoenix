#pragma once
#include "Persistance.h"
#include <vector>
#include <string>
#include "Windows.h"
#include <taskschd.h>
#include <comdef.h>
#include <iostream>

typedef struct task {
	std::wstring name;
	std::wstring path;
	bool enabled;
	int state;
};

typedef struct folder {
	std::wstring folderName;
	std::vector<task> myTasks;
	std::vector<folder> myFolders;
};


class TaskScheduler : public Persistance {
public:
	TaskScheduler();

	void Init() override;
	void GetCount() override;
	void GetAll() override;

private:
	ITaskService* initComTaskScheduler();
	void LoadTasks(ITaskFolder* myTask);
	void BrowseFolderRecurs(ITaskFolderCollection* collectionRootFolder);

	int count;
	struct folder rootFolder;

};