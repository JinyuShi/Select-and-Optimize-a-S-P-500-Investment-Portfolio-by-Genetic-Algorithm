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
	map<Stock, float> Stocks;
	float fitness;
public:
	Portfolio(map<Stock,float>stocks_): Stocks(stocks_){}
	~Portfolio() {}
	map<string, float> getdailyreturn()
	{
		map<string, float>portfolio_dailyreturn;
		vector<string>date;
		for (map<string, float>::iterator it = (Stocks.begin())->first.getdailyreturn().begin(); it != (Stocks.begin())->first.getdailyreturn().end(); it++)
		{
			date.push_back(it->first);
		}
		for (vector<string>::iterator itr = date.begin(); itr != date.end(); itr++)
		{
			float dailyreturn = 0;
			for (map<Stock, float>::iterator inner = Stocks.begin(); inner != Stocks.end(); inner++)
			{
				dailyreturn += ((inner->first).getdailyreturn())[*itr] * (inner->second);
			}
			portfolio_dailyreturn[*itr] = dailyreturn;
		}
		return portfolio_dailyreturn;
	}
	float CalReturn()
	{
		float portfolio_return = 0;
		for (map<Stock,float>::iterator it = Stocks.begin(); it != Stocks.end(); it++)
		{
			float stock_return = (it->first).CalRet();
			portfolio_return += stock_return*(it->second);
		}
		return portfolio_return;
	}
	float CalRisk()
	{
		float portfolio_risk = 0, result;
		float mean = CalReturn();
		map<string, float> portfolio_dailyreturn = getdailyreturn();
		for (map<string, float>::iterator it = portfolio_dailyreturn.begin(); it != portfolio_dailyreturn.end(); it++)
		{
			float stock_return = it->second;
			portfolio_risk += pow(stock_return - mean, 2)*(it->second);
		}
		result = sqrt(portfolio_risk);
		return result;
	}
	float CalBeta()
	{
		float portfolio_beta = 0;
		for (map<Stock, float>::iterator it = Stocks.begin(); it != Stocks.end(); it++)
		{
			portfolio_beta += ((it->first).getbeta())*(it->second);
		}
		return portfolio_beta;
	}
	float CalSharpeRatio()
	{
		return (CalReturn() - (Stocks.begin()->first).CalRiskfreereturn()) / CalRisk();
	}
	float CalTreynor()
	{
		return  (CalReturn() - (Stocks.begin()->first).CalRiskfreereturn()) / CalBeta();
	}
	float CalDiverIndex()
	{
		//pi for stock i = (weight*sigma)^2/sum of (weight*sigma)^2
		//H = sum of pi^2
		float total_weight = 0, p = 0, H = 0;
		for (map<Stock, float>::iterator it = Stocks.begin(); it != Stocks.end(); it++)
		{
			p = pow(it->second, 2)*pow((it->first).CalStd(), 2);
			total_weight += p;
			H += pow(p, 2);
		}
		H /= pow(total_weight, 2);
		return H;
	}
	void addfitness()
	{
		float fitness_ = 0;
		float a, b, c;
		fitness += a * CalSharpeRatio() + b / CalDiverIndex() + c;
		fitness = fitness_;
	}
	float getfitness()
	{
		return fitness;
	}

};
#endif