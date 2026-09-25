#pragma once

class Persistance {
public:
	// New
	virtual void Init() = 0;
	virtual int GetCount() = 0;
	virtual void GetAll() = 0;
	//virtual bool GetOnce() = 0;
};