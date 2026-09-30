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

typedef struct task {
	std::wstring name;
	std::wstring path;
	bool enabled;
	int state;
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

private:
	ITaskService* initCom();
	void addFolder(std::wstring parentFolderName, struct folder folderToAdd);
	void LoadTasks(ITaskFolder* myTask);
	void BrowseFolderRecurs(ITaskFolderCollection* collectionRootFolder);

	int count;
	std::map<std::wstring, struct folder> taskSchedulerFolders; // key : pathName

};