#pragma once
#ifndef MarketDataLogic_h
#define MarketDataLogic_h

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include "Utility.h"

#define NUM_OF_STOCKS 505

using namespace std;

class Trade
{
private:
	string date;
	float open;
	float high;
	float low;
	float close;
	float adjusted_close;
	int volume;
public:
	Trade(string date_, float open_, float high_, float low_, float close_, float adjusted_close_, int volume_) :
		date(date_), open(open_), high(high_), low(low_), close(close_), adjusted_close(adjusted_close_), volume(volume_)
	{}
	Trade() {}
	~Trade() {}
	float getOpen() { return open; }
	string getDate() { return date; }
	float getAdjClose() { return adjusted_close; }
	friend ostream & operator << (ostream & out, const Trade & t)
	{
		out << "Date: " << t.date << " Open: " << t.open << " High: " << t.high << " Low: " << t.low << " Close: " << t.close << " Adjusted_Close: " << t.adjusted_close << " Volume: " << t.volume << endl;
		return out;
	}
};

class Fundamental
{
private:
	float peratio;
	float divyield;
	float beta;
	float high52;
	float low52;
	float ma50;
	float ma200;
public:
	Fundamental() {}
	Fundamental(float peratio_, float divyield_, float beta_, float high52_, float low52_, float ma50_, float ma200_) :
		peratio(peratio_), divyield(divyield_), beta(beta_), high52(high52_), low52(low52_), ma50(ma50_), ma200(ma200_)
	{}
	~Fundamental() {}
	float getBeta() { return beta; }
	float getdivyield() { return divyield; }
	friend ostream & operator << (ostream & out, const Fundamental & f)
	{
		out << "P/E ratio: " << f.peratio << " Dividend Yield: " << f.divyield << " Beta: " << f.beta << " High 52Weeks: " << f.high52 << " Low 52Weeks: " << f.low52 << " MA 50Days: " << f.ma50 << " MA 200Days: " << f.ma200 << endl;
		return out;
	}
};

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
				float return_ = ((itr->getAdjClose() / (itr - 1)->getAdjClose()) - 1)*100;
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
	maps getriskfreereturn() { return riskfreereturn; }
	string getsymbol() { return symbol; }
	float getbeta() { return fundamental.getBeta(); }
	float getdivyield() { return fundamental.getdivyield(); }
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
			out << "date: " << it->first << ", daily return: " << it->second << ", risk free rate: "<<s.riskfreereturn.find(it->first)->second<< endl;
		}
		return out;
	}
	bool operator<(const Stock&stock1) const
	{
		return (this->symbol < stock1.symbol);
	}
};

#endif
