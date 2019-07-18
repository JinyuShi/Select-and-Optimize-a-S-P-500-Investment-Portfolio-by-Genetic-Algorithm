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

typedef vector<Portfolio> Population;

using namespace std;

#define MUTATION_RATE             0.03
#define POP_SIZE                  100
#define STOCK_NUMBER              10

#define MAX_ALLOWABLE_GENERATIONS   1000

//returns a integer from 0 to 504
#define RANDOM_INT      ((int)rand()%505)
//return a float from 0 to 1
#define RANDOM_FLOAT    ((float)rand()/RAND_MAX)
//return a integer from 0 to 9
#define RANDOM_NUM      ((int)rand()%10)

/////////////////////////////////prototypes/////////////////////////////////////////////////////

vector<string> GetRandomSymbols(const vector<string>&symbol_);
Portfolio GetPortfolio(vector<string>&symbol_, const map<string, Stock>&stocks_);
Population GetFirstGeneration(const vector<string>&symbol_, const map<string, Stock>&stocks_);
void Mutate(Population& population, const vector<string>&symbol_, const map<string, Stock>&stocks_);
void Crossover(Portfolio&portfolio1, Portfolio&portfolio2, const map<string, Stock>&stocks_);
vector<pair<Portfolio, Portfolio>> Selection(Population&population);


//---------------------------------GetRandomSymbols-----------------------------------------
//
//  This function returns a list of 10 random stock symbols from given symbol vector
//
//-----------------------------------------------------------------------------------------
vector<string> GetRandomSymbols(const vector<string>&symbol_)
{
	vector<string>randomsymbols;
	string symbol;
	int count = 0;
	while (count < STOCK_NUMBER)
	{
		int temp = RANDOM_INT;
		symbol = *(symbol_.begin() + temp);
		if (find(randomsymbols.begin(), randomsymbols.end(), symbol) == randomsymbols.end())
		{
			randomsymbols.push_back(symbol);
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
Portfolio GetPortfolio(vector<string>&symbol,const map<string, Stock>&stocks_)
{
	map<Stock,float>stocks;
	for (vector<string>::iterator it = symbol.begin(); it != symbol.end(); it++)
	{
		if (stocks_.find(*it)!=stocks_.end())
			stocks.insert({ stocks_.find(*it)->second,stocks_.find(*it)->second.weight });
	}
	Portfolio portfolio(symbol,stocks);
	portfolio.SetDailyReturn();
	portfolio.SetSharpeRatio();
	portfolio.SetDiverIndex();
	//portfolio.AssignFitness();
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
	for (int i = 0; i < POP_SIZE; i++)
	{
		symbol = GetRandomSymbols(symbol_);
		Portfolio portfolio = GetPortfolio(symbol, stocks_);
		first_generation.push_back(portfolio);
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
	for (auto it = population.begin(); it != population.end(); it++)
	{
		if (RANDOM_FLOAT < MUTATION_RATE)
		{
			vector<string> new_symbol = it->GetSymbols();
			string to_add = " ";
			while (to_add == " ")
			{
				int temp = RANDOM_INT;
				string symbol = *(symbol_.begin() + temp);
				if (find(new_symbol.begin(), new_symbol.end(), symbol) == new_symbol.end())
					to_add = symbol;
			}
			new_symbol.at(RANDOM_NUM) = to_add;
			(*it) = GetPortfolio(new_symbol, stocks_);
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
		symbol = *(pool.begin() + (int)(rand() % pool.size()));
		if (find(newsymbol1.begin(), newsymbol1.end(), symbol) == newsymbol1.end())
		{
			newsymbol1.push_back(symbol);
			count++;
		}
	}
	//finally settled stock symbols for two portfolios
	newsymbol2 = pool - newsymbol1 + intersection;
	newsymbol1 = newsymbol1 + intersection;
	//update two portfolio
	portfolio1 = GetPortfolio(newsymbol1, stocks_);
	portfolio2 = GetPortfolio(newsymbol2, stocks_);
}

//---------------------------------- Selection ---------------------------------------
//
//  Selecting paris of portfolios to crossover
//
//------------------------------------------------------------------------------------
vector<pair<Portfolio, Portfolio>> Selection(Population&population)
{
	vector<pair<Portfolio, Portfolio>> portfolio_pairs;
	vector<int> numbers;
	//vector of number of portfolios which could have children
	for (int i = 1; i < 70; i++)
	{
		//top 29 portfolios could have crossover two times
		if (i < 30)
		{
			numbers.push_back(i);
			numbers.push_back(i);
		}
		//portfolios from order 30 to 69 coulde have crossover one time
		else
		{
			numbers.push_back(i);
		}
	}
	//generate pairs
	while (numbers.size() != 2)
	{
		int number1 = *(numbers.begin() + (int)rand % numbers.size());
		int number2 = *(numbers.begin() + (int)rand % numbers.size());
		if (number1 != number2)
		{
			portfolio_pairs.push_back(make_pair(*(population.begin() + number1 - 1), *(population.begin() + number2 - 1)));
			numbers.erase(find(numbers.begin(), numbers.end(), number1));
			numbers.erase(find(numbers.begin(), numbers.end(), number2));
		}
	}
	//add the remaining two portfolios
	portfolio_pairs.push_back(make_pair(*(population.begin() + *numbers.begin()), *(population.begin() + *(numbers.begin() + 1))));
	return portfolio_pairs;
}

/*
//-------------------------------main--------------------------------------------------
//
//-------------------------------------------------------------------------------------
int main()
{
	//seed the random number generator
	srand((int)time(NULL));

	//just loop endlessly until user gets bored :0)
	while (true)
	{
		//storage for our population of chromosomes.
		vector<chromo_typ> Population(POP_SIZE);

		//get a target number from the user. (no error checking)
		float Target;
		cout << "\nInput a target number: ";
		cin >> Target;
		cout << endl << endl;

		// End the program
		if (Target > 1000)
			break;

		//first create a random population, all with zero fitness.
		for (int i = 0; i < POP_SIZE; i++)
		{
			// first generation
			chromo_typ sample(GetRandomBits(CHROMO_LENGTH), 0.0f);
			Population.push_back(sample);
		}

		int GenerationsRequiredToFindASolution = 0;

		//we will set this flag if a solution has been found
		bool bFound = false;

		//enter the main GA loop
		while (!bFound)
		{
			//this is used during roulette wheel sampling
			float TotalFitness = 0.0f;

			// test and update the fitness of every chromosome in the 
			// population
			for (std::vector<chromo_typ>::iterator i = Population.begin(); i != Population.end(); i++)
			{
				i->fitness = AssignFitness(i->bits, Target);

				TotalFitness += i->fitness;
			}

			// check to see if we have found any solutions (fitness will be 999)
			for (std::vector<chromo_typ>::iterator i = Population.begin(); i != Population.end(); i++)
			{
				if (i->fitness == 999.0f)
				{
					cout << "\nSolution found in " << GenerationsRequiredToFindASolution << " generations!" << endl << endl;;
					cout << "Result: ";
					PrintChromo(i->bits);

					bFound = true;

					break;
				}
			}

			// create a new population by selecting two parents at a time and creating offspring
			// by applying crossover and mutation. Do this until the desired number of offspring
			// have been created. 

			//sort population by fitness
			std::sort(Population.begin(), Population.end());

			//define some temporary storage for the new population we are about to create
			chromo_typ temp[(int)(POP_SIZE*CROSSOVER_RATE)];

			int cPop = 0;

			//loop until we have created POP_SIZE new chromosomes
			while (cPop < POP_SIZE*CROSSOVER_RATE)
			{
				// we are going to create the new population by grabbing members of the old population
				// two at a time via roulette wheel selection.
				string offspring1 = Roulette(TotalFitness, Population);
				string offspring2 = Roulette(TotalFitness, Population);

				//add crossover dependent on the crossover rate
				Crossover(offspring1, offspring2);

				//now mutate dependent on the mutation rate
				Mutate(offspring1);
				Mutate(offspring2);

				//add these offspring to the new population. (assigning zero as their
				//fitness scores)
				temp[cPop++] = chromo_typ(offspring1, 0.0f);
				temp[cPop++] = chromo_typ(offspring2, 0.0f);

			}//end loop

			 //copy temp population into main population array
			vector<chromo_typ>::iterator it = Population.begin();
			for (int i = 0; i < POP_SIZE*CROSSOVER_RATE; i++)
			{
				*it = temp[i];
				it += 1;
			}

			++GenerationsRequiredToFindASolution;

			// exit app if no solution found within the maximum allowable number
			// of generations
			if (GenerationsRequiredToFindASolution > MAX_ALLOWABLE_GENERATIONS)
			{
				cout << "No solutions found this run!";

				bFound = true;
			}

		}

		cout << "\n\n\n";

	}//end while

	return 0;
}
*/
#endif