#define _CRT_SECURE_NO_WARNINGS

#include <stdlib.h>
#include <windows.h>
#include "ElapsedTime.h"
#include "strutil.h"

ElapsedTime::ElapsedTime()
{
	m_start = m_end = 0;
}

ElapsedTime::~ElapsedTime()
{
}

void ElapsedTime::Start()
{
	m_start = m_end = ::GetTickCount();
}

void ElapsedTime::End()
{
	m_end = ::GetTickCount();
}

long ElapsedTime::GetYears()const
{
	long years = GetMonths() / 12.0;

	return years;
}

long ElapsedTime::GetMonths()const
{
	long months = GetDays() / 30.0;

	return months;
}

long ElapsedTime::GetDays()const
{
	long days = GetHours() / 24.0;

	return days;
}

long ElapsedTime::GetHours()const
{
	long hours = GetMinutes() / 60.0;

	return hours;
}

long ElapsedTime::GetMinutes()const
{
	long minutes = GetSeconds() / 60.0;

	return minutes;
}

long ElapsedTime::GetSeconds()const
{
	return (m_end - m_start)*1e-3;
}

string ElapsedTime::ToString()const
{
	string result;

    if(GetHours()>0)
    {
		result += NumberToString(GetHours()) + " hour(s) ";
    }
	if(GetMinutes()>0)
	{
		result += NumberToString(GetMinutes()-GetHours()*60) + " minute(s) ";
	}
	if(GetSeconds()>0)
	{
		result += NumberToString(GetSeconds()-GetMinutes()*60) + " second(s)\n";
	}

    return result;
}

