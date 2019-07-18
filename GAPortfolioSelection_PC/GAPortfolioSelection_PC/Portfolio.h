#pragma once
#ifndef Portfolio_h
#define Portfolio_h

#include <iostream>
#include <vector>
#include <map>
#include<algorithm>

#include "MarketDataLogic.h"
#include "DatabaseLogic.h"
#include "Utility.h"

using namespace std;

class Portfolio
{
private:
	vector<string> Symbols;
	map<Stock,float> Stocks;
public:
	maps DailyReturn;
	float sharperatio, diverindex, fitness;
	
	Portfolio(vector<string>symbols_,map<Stock,float>stocks_) : Symbols(symbols_),Stocks(stocks_) {}
	~Portfolio() {}
	vector<string> GetSymbols() { return Symbols; }
	void SetDailyReturn()
	{
		vector<maps> dailyreturns;
		for (map<Stock, float>::const_iterator it = Stocks.begin(); it != Stocks.end(); it++)
		{
			Stock temp = it->first;
			dailyreturns.push_back((temp.getdailyreturn())*(it->second));
		}
		DailyReturn = sum(dailyreturns);
	}
	float CalReturn() { return mean(DailyReturn); }
	float CalRisk() { return sd(DailyReturn); }
	float CalBeta()
	{
		float portfolio_beta = 0;
		for (map<Stock,float>::iterator it = Stocks.begin(); it != Stocks.end(); it++)
		{
			Stock stock = it->first;
			portfolio_beta += (stock.getbeta())*(it->second);
		}
		return portfolio_beta;
	}
	float CalSharpeRatio()
	{
		vector<maps>riskfreerates;
		for (map<Stock, float>::iterator it = Stocks.begin(); it != Stocks.end(); it++)
		{
			Stock temp = Stocks.begin()->first;
			riskfreerates.push_back(temp.getriskfreerates());
		}
		return (CalReturn() - mean(largestmaps(riskfreerates))) / CalRisk();
	}
	float CalTreynor()
	{
		vector<maps>riskfreerates;
		for (map<Stock, float>::iterator it = Stocks.begin(); it != Stocks.end(); it++)
		{
			Stock temp = Stocks.begin()->first;
			riskfreerates.push_back(temp.getriskfreerates());
		}
		return  (CalReturn() - mean(largestmaps(riskfreerates))) / CalBeta();
	}
	float CalDiverIndex()
	{
		//pi for stock i = (weight*sigma)^2/sum of (weight*sigma)^2
		//H = sum of pi^2
		float total_weight = 0, p = 0, H = 0;
		for (map<Stock,float>::iterator it = Stocks.begin(); it != Stocks.end(); it++)
		{
			Stock stock = it->first;
			p = pow(it->second, 2)*pow(stock.CalStd(), 2);
			total_weight += p;
			H += pow(p, 2);
		}
		H /= pow(total_weight, 2);
		return H;
	}
	void SetSharpeRatio() { sharperatio = CalSharpeRatio(); }
	void SetDiverIndex() { diverindex = CalDiverIndex(); }
	void AssignFitness()
	{
		float a = 0, b = 0, c = 0;
		fitness = a * sharperatio + b / diverindex + c;
	}
	friend ostream & operator << (ostream & out, const Portfolio & p)
	{
		for (vector<string>::const_iterator it = p.Symbols.begin(); it != p.Symbols.end(); it++)
		{
			out << *it << " ";
		}
		out << endl;
		out << "Sharpe Ratio = " << p.sharperatio << ", Diver Index = " << p.diverindex << endl;
		return out;
	}
	bool operator>(const Portfolio&p) const { return fitness > p.fitness; }
};

#endif