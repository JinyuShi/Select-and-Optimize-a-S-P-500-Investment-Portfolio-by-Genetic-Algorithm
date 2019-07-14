#pragma once
#ifndef Utility_h
#define Utility_h

#include <iostream>
#include <vector>
#include <map>
using namespace std;

vector<string> operator-(const vector<string>&u, const vector<string>&v)
{
	vector<string> result;
	for (auto it = u.begin(); it != u.end(); it++)
	{
		if (find(v.begin(), v.end(), *it) == v.end())
		{
			result.push_back(*it);
		}
	}
	return result;
}

vector<string> operator+(const vector<string>&u, const vector<string>&v)
{
	vector<string> result = u;
	for (auto it = v.begin(); it != v.end(); it++)
	{
		if (find(u.begin(), u.end(), *it) == u.end())
		{
			result.push_back(*it);
		}
	}
	return result;
}
#endif
