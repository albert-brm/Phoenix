#pragma once

class Persistance {
public:
	virtual void Init() = 0;
	virtual void GetCount() = 0;
	virtual void GetAll() = 0;
};