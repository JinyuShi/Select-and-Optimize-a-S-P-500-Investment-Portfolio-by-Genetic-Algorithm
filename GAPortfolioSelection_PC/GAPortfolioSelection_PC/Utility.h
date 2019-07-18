#pragma once
#ifndef Utility_h
#define Utility_h

#include <iostream>
#include <vector>
#include <map>

using namespace std;
typedef map<string, float> maps;

ostream & operator << (ostream & out, const maps&m)
{
	for (auto it = m.begin(); it != m.end(); it++)
	{
		out << "date: " << it->first << ", rate: " << it->second << endl;
	}
	return out;
}

maps operator*(const maps&m, float f)
{
	maps result;
	for (auto it = m.begin(); it != m.end(); it++)
	{
		result.insert({ it->first,(it->second)*f });
	}
	return result;
}

maps operator-(const maps&m, float f)
{
	maps result;
	for (auto it = m.begin(); it != m.end(); it++)
	{
		result.insert({ it->first,(it->second)-f });
	}
	return result;
}
maps largestmaps(const vector<maps>&v)
{
	maps result;
	for (auto it = v.begin(); it != v.end(); it++)
	{
		if (it->size() > result.size())
			result = *it;
	}
	return result;
}

maps sum(const vector<maps>&v)
{
	maps result,large;
	vector<string>date;
	float day_return;
	large = largestmaps(v);
	for (auto it = large.begin(); it != large.end(); it++)
	{
		date.push_back(it->first);
	}
	for (auto it = date.begin(); it != date.end(); it++)
	{
		day_return = 0;
		for (auto itr = v.begin(); itr != v.end(); itr++)
		{
			if (itr->find(*it) != itr->end())
				day_return += itr->find(*it)->second;
		}
		result.insert({ *it,day_return });
	}
	return result;
}

float mean(const maps&m)
{
	float result = 0;
	for (auto it = m.begin(); it != m.end(); it++)
	{
		result += it->second;
	}
	result /= m.size();
	return result;
}

float sd(const maps&m)
{
	float result = 0;
	float mean_ = mean(m);
	maps n = m - mean_;
	for (auto it = n.begin(); it != n.end(); it++)
	{
		result += pow(it->second, 2);
	}
	result /= n.size();
	return sqrt(result);
}

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
