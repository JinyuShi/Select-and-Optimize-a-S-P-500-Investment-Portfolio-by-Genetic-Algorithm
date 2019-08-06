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
	vector<Stock> Stocks;
public:
	maps DailyReturn;
	float sharperatio, diverindex, beta, fitness, dividendyield;
	
	Portfolio(vector<string>symbols_, const vector<Stock>stocks_)
	{
		Symbols = symbols_;
		vector<Stock> temp = stocks_;
		for (int n = 0; n < temp.size(); n++)
		{
			maps r = temp[n].getriskfreereturn();
			maps::iterator itr = r.find("2018-12-31");
			r.erase(itr, r.end());
			temp[n].setriskfreereturn(r);
			maps d = temp[n].getdailyreturn();
			maps::iterator it = d.find("2018-12-31");
			d.erase(it, d.end());
			temp[n].setdailyreturn(d);
		}
		float total_weight = 0;
		for (int n = 0;n<10;n++)
		{
			total_weight += stocks_[n].weight;
		}
		for (int n = 0;n<10;n++)
		{
			temp[n].weight /= total_weight;
		}
		Stocks = temp;
	}
	Portfolio() {}
	~Portfolio() {}
	vector<string> GetSymbols() { return Symbols; }
	vector<Stock> GetStocks() { return Stocks; }
	void SetDailyReturn()
	{
		maps dailyreturns = Stocks[0].getdailyreturn()*Stocks[0].weight;
		for (vector<Stock>::iterator itr = Stocks.begin()+1; itr != Stocks.end(); itr++)
		{
			dailyreturns = dailyreturns + itr->getdailyreturn()*itr->weight;
		}
		DailyReturn = dailyreturns;
	}
	void SetSharpeRatio()
	{
		Stock temp = Stocks[0];
		sharperatio = (mean(DailyReturn) - mean(temp.getriskfreereturn())) / sd(DailyReturn);
	}
	void SetDiverIndex()
	{
		//pi for stock i = (weight*sigma)^2/sum of (weight*sigma)^2
		//H = sum of pi^2
		float total_weight = 0, p = 0, H = 0;
		for (vector<Stock>::iterator it = Stocks.begin(); it != Stocks.end(); it++)
		{
			p = pow(it->weight, 2)*pow(it->CalStd(), 2);
			total_weight += p;
			H += pow(p, 2);
		}
		H /= pow(total_weight, 2);
		diverindex = H;
	}
	void SetDividendYield()
	{
		float portfolio_dividendyield = 0;
		for (vector<Stock>::iterator it = Stocks.begin(); it != Stocks.end(); it++)
		{
			portfolio_dividendyield += it->getdivyield()*it->weight;
		}
		dividendyield = portfolio_dividendyield;
	}
	void SetBeta()
	{
		float portfolio_beta = 0;
		for (vector<Stock>::iterator it = Stocks.begin(); it != Stocks.end(); it++)
		{
			portfolio_beta += it->getbeta()*it->weight;
		}
		beta = portfolio_beta;
	}
	void AssignFitness() { fitness = 0.6 * 100 * sharperatio + 0.2 * 100 * dividendyield + 0.2*(1/ diverindex); }//0.1*beta; } 
	friend ostream & operator << (ostream & out, const Portfolio &p)
	{
		out << "fitness = "<<p.fitness<< endl;
		Portfolio temp = p;
		for (vector<string>::iterator it = temp.GetSymbols().begin(); it != temp.GetSymbols().end(); it++)
		{
			out << *it << " ";
		}
		out << endl;
		return out;
	}
	bool sortByFitness(const Portfolio&p1) { return fitness > p1.fitness; }
};

#endif 