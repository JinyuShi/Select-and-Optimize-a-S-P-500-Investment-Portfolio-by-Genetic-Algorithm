#pragma once
#ifndef MarketDataLogic_h
#define MarketDataLogic_h

#include <iostream>
#include <vector>
#include <map>

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
	~Trade() {}
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
	float getBeta()
	{
		return beta;
	}
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
	map<string, float> dailyreturns;
	map<string,float> riskfreerates;
	Fundamental fundamental;

public:
	Stock(string symbol_) :symbol(symbol_)
	{}
	~Stock() {}
	void addTrade(Trade aTrade)
	{
		trades.push_back(aTrade);
	}
	void addDailyReturns()
	{
		for (vector<Trade>::iterator itr = trades.begin() + 1; itr != trades.end(); itr++)
		{
			string date_ = (*itr).getDate();
			float return_ = ((*itr).getAdjClose() - (*(itr - 1)).getAdjClose()) / ((*(itr - 1)).getAdjClose());
			dailyreturns.insert({ date_, return_ });
		}
	}
	void addRiskFreeRates(string date_, float riskfreerate_)
	{
		riskfreerates.insert({ date_, riskfreerate_ });
	}
	void addFundamental(Fundamental fundamental_)
	{
		fundamental = fundamental_;
	}
	map<string,float> getdailyreturn()
	{
		return dailyreturns;
	}
	map<string, float>getriskfreerates()
	{
		return riskfreerates;
	}
	float getbeta()
	{
		return fundamental.getBeta();
	}
	float CalRiskfreereturn()
	{
		float mean_riskfreereturn = 0;
		int count = 0;
		for (map<string, float>::iterator it = riskfreerates.begin(); it != riskfreerates.end(); it++)
		{
			mean_riskfreereturn += it->second;
			count++;
		}
		mean_riskfreereturn /= count;
		return mean_riskfreereturn;
	}
	float CalRet()
	{
		float mean_return = 0;
		int count = 0;
		for (map<string, float>::iterator it = dailyreturns.begin(); it != dailyreturns.end(); it++)
		{
			mean_return += it->second;
			count++;
		}
		mean_return /= count;
		return mean_return;
	}
	float CalStd()
	{
		float mean_return = CalRet();
		float std = 0;
		int count = 0;
		for (map<string, float>::iterator it = dailyreturns.begin(); it != dailyreturns.end(); it++)
		{
			std += pow(it->second - mean_return, 2);
			count++;
		}
		std /= count;
		return sqrt(std);
	}
	friend ostream & operator << (ostream & out, const Stock & s)
	{
		out << "Symbol: " << s.symbol << endl;
		for (vector<Trade>::const_iterator itr = s.trades.begin(); itr != s.trades.end(); itr++)
			out << *itr;
		out << s.fundamental;
		for (map<string, float>::const_iterator itr = s.dailyreturns.begin(); itr != s.dailyreturns.end(); itr++)
			out << itr->first << " : " << itr->second << endl;
		return out;
	}
};

#endif
