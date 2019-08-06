#pragma once
#ifndef GALogic_h
#define GALogic_h

#include <string>
#include <stdlib.h>
#include <iostream>
#include <time.h>
#include <math.h>
#include <string>
#include <vector>
#include <map>
#include <algorithm>

#include "MarketDataLogic.h"
#include "Portfolio.h"
#include "Utility.h"

#include <chrono>
typedef vector<Portfolio> Population;

using namespace std;
using namespace std::chrono;

#define MUTATION_RATE             0.03
#define POP_SIZE                  100
#define STOCK_NUMBER              10

#define MAX_ALLOWABLE_GENERATIONS   1000

//returns a integer from 0 to 504
#define RANDOM_INT      (int)rand()%505
//return a float from 0 to 1
#define RANDOM_FLOAT    (float)((rand()%100)+1)/100
//return a integer from 0 to 9
#define RANDOM_NUM      (int)rand()%10

/////////////////////////////////prototypes/////////////////////////////////////////////////////

vector<string> GetRandomSymbols(const vector<string>&symbol_);
Portfolio GetPortfolio(vector<string>&symbol_, const map<string, Stock>&stocks_);
Population GetFirstGeneration(const vector<string>&symbol_, const map<string, Stock>&stocks_);
void Mutate(Population& population, const vector<string>&symbol_, const map<string, Stock>&stocks_);
void Crossover(Portfolio&portfolio1, Portfolio&portfolio2, const map<string, Stock>&stocks_);
vector<pair<int, int>> Selection();
bool sortByFitness(const Portfolio&p1, const Portfolio&p2) { return p1.fitness > p2.fitness; }


//---------------------------------GetRandomSymbols-----------------------------------------
//
//  This function returns a list of 10 random stock symbols from given symbol vector
//
//-----------------------------------------------------------------------------------------
vector<string> GetRandomSymbols(const vector<string>&symbol_)
{
	vector<string>randomsymbols(STOCK_NUMBER);
	randomsymbols[0] = symbol_[RANDOM_INT];
	int count = 1;
	string symbol;
	while (count < STOCK_NUMBER)
	{
		symbol = symbol_[RANDOM_INT];
		if (find(randomsymbols.begin(),randomsymbols.end(),symbol) == randomsymbols.end())
		{
			randomsymbols[count] = symbol;
			count += 1;
		}
	}
	return randomsymbols;
}

//--------------------------------- GetPortfolio -----------------------------------------
//
//  This function returns a portfolio
//
//-----------------------------------------------------------------------------------------
Portfolio GetPortfolio(vector<string>&symbol, const map<string, Stock>&stocks_)
{
	vector<Stock> temp;
	for (int n = 0;n<symbol.size();n++)
	{
		temp.push_back(stocks_.find(symbol[n])->second);
	}
	Portfolio portfolio(symbol,temp);

	portfolio.SetDailyReturn();
	
	portfolio.SetSharpeRatio();
	
	portfolio.SetDiverIndex();
	
	//portfolio.SetBeta();

	portfolio.SetDividendYield();
	
	portfolio.AssignFitness();

	return portfolio;
}

//---------------------------------GetFirstGeneration-----------------------------------------
//
//  This function returns the first portfolio generation
//
//-----------------------------------------------------------------------------------------
Population GetFirstGeneration(const vector<string>&symbol_, const map<string, Stock>&stocks_)
{
	Population first_generation;
	vector<string>symbol;
	int count = 1;
	for (int i = 0; i < POP_SIZE; i++)
	{
		symbol = GetRandomSymbols(symbol_);
		Portfolio portfolio = GetPortfolio(symbol, stocks_);
		cout << "Portfolio " << count << ": " << portfolio;
		first_generation.push_back(portfolio);
		count++;
	}
	return first_generation;
}

//------------------------------------ Mutate ---------------------------------------
//
//  Mutates a population dependent on the  portfolio MUTATION_RATE
//
//-------------------------------------------------------------------------------------
void Mutate(Population& population, const vector<string>&symbol_, const map<string,Stock>&stocks_)
{
	for (int it = 0; it < population.size(); it++)
	{
		if (RANDOM_FLOAT<MUTATION_RATE)
		{
			vector<string> new_symbol = population[it].GetSymbols();
			string to_add = " ";
			while (to_add == " ")
			{
				string symbol = symbol_[RANDOM_INT];
				if (find(new_symbol.begin(), new_symbol.end(), symbol) == new_symbol.end())
					to_add = symbol;
			}
			new_symbol[RANDOM_NUM] = to_add;
			population[it] = GetPortfolio(new_symbol, stocks_);
		}
	}
	return;
}

//---------------------------------- Crossover ---------------------------------------
//
//  Updatng two new portfolios with mixing and unique stocks(no several same stocks in one portfolio)
//
//------------------------------------------------------------------------------------
void Crossover(Portfolio&portfolio1, Portfolio&portfolio2, const map<string, Stock>&stocks_)
{
	vector<string>symbol1 = portfolio1.GetSymbols();
	vector<string>symbol2 = portfolio2.GetSymbols();

	//find stocks in both portfolio1 and portfolio2, keep them in original portfolio without crossover to prevent several same stocks in one portfolio
	vector<string>in1only = symbol1 - symbol2;
	vector<string>in2only = symbol2 - symbol1;
	vector<string>intersection = symbol1 - in1only;
	//the stock symbols which will crossover
	vector<string>pool = in1only + in2only;

	vector<string>newsymbol1, newsymbol2;
	string symbol;
	int count = 0;
	//randomly select stock symbols such that plus intersection part there will be 10 stock symbols
	while (count < STOCK_NUMBER - intersection.size())
	{
		symbol = pool[(int)(rand() % pool.size())];
		if (find(newsymbol1.begin(), newsymbol1.end(), symbol) == newsymbol1.end())
		{
			newsymbol1.push_back(symbol);
			count++;
		}
	}
	//finally settled stock symbols for two portfolios
	newsymbol2 = (pool - newsymbol1) + intersection;
	newsymbol1 = newsymbol1 + intersection;
	//update two portfolio
	portfolio1 = GetPortfolio(newsymbol1, stocks_);
	portfolio2 = GetPortfolio(newsymbol2, stocks_);
}

//---------------------------------- Selection ---------------------------------------
//
//  Selecting paris of portfolios' index numbers to crossover
//
//------------------------------------------------------------------------------------
vector<pair<int, int>> Selection()
{
	vector<pair<int,int>> portfolio_pairs;
	vector<int> portfolio_numbers;
	//vector of number of portfolios which could have children [0,0,1,1,2,2,...,28,28,29,30,31,...,68]
	for (int i = 0;i<69;i++)
	{
		//top 29 portfolios could have crossover two times
		if (i < 29)
		{
			portfolio_numbers.push_back(i);
			portfolio_numbers.push_back(i);
		}
		//portfolios from order 30 to 69 coulde have crossover one time
		else
		{
			portfolio_numbers.push_back(i);
		}
	}
	//generate pairs
	int count = 0;
	while (count<48)
	{
		int number1 = portfolio_numbers[(int)rand() % portfolio_numbers.size()];
		int number2 = portfolio_numbers[(int)rand() % portfolio_numbers.size()];
		if (number1 != number2)
		{
			portfolio_pairs.push_back(make_pair(number1, number2));
			portfolio_numbers.erase(find(portfolio_numbers.begin(), portfolio_numbers.end(), number1));
			portfolio_numbers.erase(find(portfolio_numbers.begin(), portfolio_numbers.end(), number2));
			count++;
		}
	}
	//add the remaining two portfolios
	portfolio_pairs.push_back(make_pair(portfolio_numbers[0],portfolio_numbers[1]));

	//return pairs of portfolio's index numbers
	return portfolio_pairs;
}

#endif