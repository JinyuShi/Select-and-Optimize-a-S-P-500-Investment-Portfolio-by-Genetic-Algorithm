#pragma once
#ifndef Stock_h
#define Stock_h

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include "MarketDataLogic.h""
#include "Utility.h"

using namespace std;

class Stock
{
private:
	string symbol;
	vector<Trade> trades;
	maps dailyreturns;
	maps riskfreereturn;
	Fundamental fundamental;

public:
	float weight;
	Stock(string symbol_) :symbol(symbol_) {}
	Stock() {}
	~Stock() {}
	void addWeight(float weight_) { weight = weight_; }
	void addTrade(Trade aTrade) { trades.push_back(aTrade); }
	void addDailyReturns()
	{
		for (vector<Trade>::iterator itr = trades.begin(); itr != trades.end(); itr++)
		{
			if (itr == trades.begin())
			{
				dailyreturns.insert({ itr->getDate(),0.0 });
			}
			else
			{
				float return_ = ((itr->getAdjClose() / (itr - 1)->getAdjClose()) - 1) * 100;
				dailyreturns.insert({ itr->getDate(), return_ });
			}
		}
		for (maps::iterator it = riskfreereturn.begin(); it != riskfreereturn.end(); it++)
		{
			if (dailyreturns.find(it->first) == dailyreturns.end())
				dailyreturns.insert({ it->first,0.0 });
		}
	}
	void addRiskFreeReturn(string date_, float return_) { riskfreereturn.insert({ date_,return_ }); }
	void addFundamental(Fundamental fundamental_) { fundamental = fundamental_; }
	float CalStd() { return sd(dailyreturns); }
	vector<Trade> gettrade() { return trades; }
	maps getdailyreturn() { return dailyreturns; }
	void setdailyreturn(maps m) { dailyreturns = m; }
	maps getriskfreereturn() { return riskfreereturn; }
	void setriskfreereturn(maps m) { riskfreereturn = m; }
	string getsymbol() { return symbol; }
	float getdivyield() { return fundamental.getDivyield(); }
	friend ostream & operator << (ostream & out, const Stock & s)
	{
		out << "Symbol: " << s.symbol << endl;
		out << s.fundamental << endl;
		for (std::vector<Trade>::const_iterator itr = s.trades.begin(); itr != s.trades.end(); itr++)
		{
			out << *itr << endl;
		}
		for (maps::const_iterator it = s.dailyreturns.begin(); it != s.dailyreturns.end(); it++)
		{
			out << "date: " << it->first << ", daily return: " << it->second << ", risk free rate: " << s.riskfreereturn.find(it->first)->second << endl;
		}
		return out;
	}
	bool operator<(const Stock&stock1) const
	{
		return (this->symbol < stock1.symbol);
	}
};

#endif