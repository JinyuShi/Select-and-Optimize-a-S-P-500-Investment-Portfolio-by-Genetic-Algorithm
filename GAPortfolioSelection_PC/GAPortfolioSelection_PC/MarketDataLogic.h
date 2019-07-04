#pragma once
#ifndef MarketDataLogic_h
#define MarketDataLogic_h

#include <iostream>
#include <vector>

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
	void addDailyReturns(string date_, float return_)
	{
		dailyreturns.insert({ date_, return_ });
	}
	void addRiskFreeRates(string date_, float riskfreerisk_)
	{
		riskfreerates.insert({ date_, riskfreerisk_ });
	}
	void addFundamental(Fundamental fundamental_)
	{
		fundamental = fundamental_;
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
