#pragma once
#include <string>


using namespace std;

class ElapsedTime
{
public:
	ElapsedTime();
	~ElapsedTime();

	void Start();
	void End();
	long GetYears()const;
	long GetMonths()const;
	long GetDays()const;
	long GetHours()const;
	long GetMinutes()const;
	long GetSeconds()const;
	string ToString()const;

private:
	long m_start, m_end;

};


