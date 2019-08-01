#pragma once
#ifndef Utility_h
#define Utility_h

#include <iostream>
#include <vector>
#include <map>
#include <numeric>

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
		result[it->first]=(it->second)*f;
	}
	return result;
}

maps operator+(const maps&m, const maps&n)
{
	maps result = m;
	for (auto it = n.begin(); it != n.end(); it++)
	{
		if (result.find(it->first)!=result.end())
			result[it->first] += it->second;
		else
		{
			result[it->first] = it->second;
		}
	}
	return result;
}
maps operator-(const maps&m, float f)
{
	maps result;
	for (auto it = m.begin(); it != m.end(); it++)
	{
		result[it->first] = (it->second) - f;
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
	maps n = m;
	for (auto it = n.begin(); it != n.end(); it++)
	{
		result += pow(it->second-mean_, 2);
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
