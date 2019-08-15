#pragma once
#ifndef MarketDataLogic_h
#define MarketDataLogic_h

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>

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
	float getClose() { return close; }
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
	float getDivyield() { return divyield; }
	friend ostream & operator << (ostream & out, const Fundamental & f)
	{
		out << "P/E ratio: " << f.peratio << " Dividend Yield: " << f.divyield << " Beta: " << f.beta << " High 52Weeks: " << f.high52 << " Low 52Weeks: " << f.low52 << " MA 50Days: " << f.ma50 << " MA 200Days: " << f.ma200 << endl;
		return out;
	}
};

#endif
