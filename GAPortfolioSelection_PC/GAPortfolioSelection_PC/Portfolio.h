#pragma once
#ifndef Portfolio_h
#define Portfolio_h

#include <iostream>
#include <vector>
#include <map>

#include "MarketDataLogic.h"
#include "DatabaseLogic.h"

using namespace std;

class Portfolio
{
private:
	vector<Stock> Stock_List;
public:
	Portfolio() {}
	~Portfolio() {}


};
#endif