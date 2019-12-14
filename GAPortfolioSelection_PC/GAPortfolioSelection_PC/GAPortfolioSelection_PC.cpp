#include <iostream>
#include <string>
#include <map>
#include <fstream>
#include <vector>
#include <stdio.h>
#include <algorithm>

#include "curl_easy.h"
#include "curl_form.h"
#include "curl_ios.h"
#include "curl_exception.h"

#include "json/json.h"
#include <sqlite3.h>

#include "MarketDataLogic.h"
#include "DatabaseLogic.h"
#include "Stock.h"
#include "Portfolio.h"
#include "GALogic.h"
#include "Utility.h"

#define NUM_OF_STOCKS 505

using namespace std;

int main(void)
{
	//seed the random number generator
	srand((int)time(NULL));

	//open database
	const char * stockDB_name = "Stocks.db";
	sqlite3 * stockDB = NULL;
	if (OpenDatabase(stockDB_name, stockDB) == -1)
		return -1;

	//initiate required containers to store stock symbols, stocks, and target portfolio
	vector<string> Symbol;
	map<string, Stock> Stocks;
	Portfolio target_portfolio;

	//get symbols of all stocks
	string spy_select_table = "SELECT * FROM SP500;";
	if (GetStockSymbol(spy_select_table.c_str(), stockDB, Symbol) == -1)
		return -1;

	//get trade of SPY
	Stock SPY("SPY");
	string trade_select_table = "SELECT * FROM Stock_SPY;";
	if (GetTrade(trade_select_table.c_str(), stockDB, SPY) == -1)
		return -1;
	
	bool done = true;
	while (done)
	{
		string input;
		cout << "Please select a option number: " << endl << endl
			<< "1: Retrieve SP500 Stock Symbol " << endl
			<< "2: Retrieve Stock Fundamental Data and Save to Database " << endl
			<< "3: Retrieve Stock Trade Data and Save to Database " << endl
			<< "4: Retrieve Risk Free Rate Data and Save to Database " << endl
			<< "5: Construct All Stock Objects " << endl
			<< "6: Create First Generation and Populate 1000 Generations" << endl
			<< "7: Back Testing " << endl
			<< "8: Probation Testing " << endl
			<< "9: Exit " << endl << endl;
		cin >> input;
		int option = stoi(input);
		if (option == 1)
		{
			int count = 0;
			Json::Value json_SPY_All_holdings;
			ifstream  fin("SPY_All_Holdings.txt");
			string keys[] = { "Symbol", "Name", "Weight", "Sector", "Shares" };
			string line;
			while (getline(fin, line, '\r'))
			{
				const char *begin = line.c_str();
				int index = 0;
				while (const char *end = strchr(begin, ','))
				{
					string column(begin, end - begin);
					begin = end + 1;
					json_SPY_All_holdings[count][keys[index++]] = column;
				}
				string column(begin);
				json_SPY_All_holdings[count++][keys[index]] = column;
			}

			string spy_drop_table = "DROP TABLE IF EXISTS SP500;";
			if (DropTable(spy_drop_table.c_str(), stockDB) == -1)
				return -1;
			string spy_create_table = "CREATE TABLE SP500 (id INT PRIMARY KEY NOT NULL, symbol CHAR(20) NOT NULL, name CHAR(20) NOT NULL, sector CHAR(20) NOT NULL, weight REAL NOT NULL, shares INT NOT NULL);";
			if (CreateTable(spy_create_table.c_str(), stockDB) == -1)
				return -1;
			if (PopulateSPYTable(json_SPY_All_holdings, stockDB) == -1)
				return -1;

			string spy_select_table = "SELECT * FROM SP500;";
			if (GetStockSymbol(spy_select_table.c_str(), stockDB, Symbol) == -1)
				return -1;

		}
		else if (option == 2)
		{
			int count = 1;
			std::string stockDB_drop_table = "DROP TABLE IF EXISTS Fundamental;";
			if (DropTable(stockDB_drop_table.c_str(), stockDB) == -1)
				return -1;
			string stockDB_create_table = "CREATE TABLE Fundamental (id INT PRIMARY KEY NOT NULL, symbol CHAR(20) NOT NULL, PERatio REAL NOT NULL, DividendYield REAL NOT NULL, Beta REAL NOT NULL, High_52Week REAL NOT NULL, Low_52Week REAL NOT NULL, MA_50Day REAL NOT NULL, MA_200Day REAL NOT NULL);";
			if (CreateTable(stockDB_create_table.c_str(), stockDB) == -1)
				return -1;

			for (vector<string>::iterator it = Symbol.begin(); it != Symbol.end(); it++)
			{
				cout << *it << endl;

				string stockfundamental_data_request = "https://eodhistoricaldata.com/api/fundamentals/" + *it + ".US?api_token=5ba84ea974ab42.45160048";
				Json::Value stockfundamental_root;
				if (RetrieveMarketData(stockfundamental_data_request, stockfundamental_root) == -1)
					return -1;
				if (PopulateFundamentalTable(stockfundamental_root, count, *it, stockDB) == -1)
					return -1;
				count++;
			}
			
		}
		else if (option == 3)
		{
			//Populate Stock_SPY Table
			std::string stockDB_drop_table = "DROP TABLE IF EXISTS Stock_SPY;";
			if (DropTable(stockDB_drop_table.c_str(), stockDB) == -1)
				return -1;
			string stockDB_create_table = "CREATE TABLE Stock_SPY (id INT PRIMARY KEY NOT NULL, symbol CHAR(20) NOT NULL, date CHAR(20) NOT NULL, open REAL NOT NULL, high REAL NOT NULL, low REAL NOT NULL, close REAL NOT NULL, adjusted_close REAL NOT NULL, volume INT NOT NULL);";
			if (CreateTable(stockDB_create_table.c_str(), stockDB) == -1)
				return -1;
			string stockDB_data_request = "https://eodhistoricaldata.com/api/eod/SPY.USfrom=2008-01-01&to=2019-07-31&api_token=5ba84ea974ab42.45160048&period=d&fmt=json";

			Json::Value stockDB_root;   // will contains the root value after parsing.
			if (RetrieveMarketData(stockDB_data_request, stockDB_root) == -1)
				return -1;
			if (PopulateStockTable(stockDB_root, "Stock_SPY", stockDB) == -1)
				return -1;
			string stockDB_select_table = "SELECT * FROM Stock_SPY;";
			if (DisplayTable(stockDB_select_table.c_str(), stockDB) == -1)
				return -1;

			for (vector<string>::iterator it = Symbol.begin(); it != Symbol.end(); it++)
			{
				string stockDB_symbol = "Stock_" + *it;
				if (stockDB_symbol == "Stock_BRK-B")
					stockDB_symbol = "Stock_BRK_B";
				else if (stockDB_symbol == "Stock_BF-B")
					stockDB_symbol = "Stock_BF_B";
				std::string stockDB_drop_table = "DROP TABLE IF EXISTS " + stockDB_symbol + ";";
				if (DropTable(stockDB_drop_table.c_str(), stockDB) == -1)
					return -1;

				string stockDB_create_table = "CREATE TABLE " + stockDB_symbol
					+ "(id INT PRIMARY KEY NOT NULL,"
					+ "symbol CHAR(20) NOT NULL,"
					+ "date CHAR(20) NOT NULL,"
					+ "open REAL NOT NULL,"
					+ "high REAL NOT NULL,"
					+ "low REAL NOT NULL,"
					+ "close REAL NOT NULL,"
					+ "adjusted_close REAL NOT NULL,"
					+ "volume INT NOT NULL);";

				if (CreateTable(stockDB_create_table.c_str(), stockDB) == -1)
					return -1;

				if (stockDB_symbol != "Stock_MSI")
				{
					string stock_url_common = "https://eodhistoricaldata.com/api/eod/";
					string stock_start_date = "2008-01-01";
					string stock_end_date = "2019-07-31";
					string api_token = "";
					string stockDB_data_request = stock_url_common + *it + ".US?" +
						"from=" + stock_start_date + "&to=" + stock_end_date + "&api_token=" + api_token + "&period=d&fmt=json";

					Json::Value stockDB_root;   // will contains the root value after parsing.
					if (RetrieveMarketData(stockDB_data_request, stockDB_root) == -1)
						return -1;
					if (PopulateStockTable(stockDB_root, stockDB_symbol, stockDB) == -1)
						return -1;
				}
				else
				{
					// MSI's trading has several months gap, still want to keep it 
					string stock_url_common = "https://eodhistoricaldata.com/api/eod/";
					string stock_start_date1 = "2008-01-01";
					string stock_end_date1 = "2017-11-20";
					string stock_start_date2 = "2018-01-02";
					string stock_end_date2 = "2019-07-31";
					string api_token = "";
					string stockDB_data_request1 = stock_url_common + *it + ".US?" +
						"from=" + stock_start_date1 + "&to=" + stock_end_date1 + "&api_token=" + api_token + "&period=d&fmt=json";
					string stockDB_data_request2 = stock_url_common + *it + ".US?" +
						"from=" + stock_start_date2 + "&to=" + stock_end_date2 + "&api_token=" + api_token + "&period=d&fmt=json";

					Json::Value stockDB_root1;
					Json::Value stockDB_root2; 
					if (RetrieveMarketData(stockDB_data_request1, stockDB_root1) == -1)
						return -1;
					if (RetrieveMarketData(stockDB_data_request2, stockDB_root2) == -1)
						return -1;
					if (PopulateMSITable(stockDB_root1, stockDB_root2, stockDB_symbol, stockDB) == -1)
						return -1;
				}

				string stockDB_select_table = "SELECT * FROM " + stockDB_symbol + ";";
				if (DisplayTable(stockDB_select_table.c_str(), stockDB) == -1)
					return -1;
			}
		}
		else if (option == 4)
		{
			std::string riskfreereturn_drop_table = "DROP TABLE IF EXISTS RiskFreeReturn;";
			if (DropTable(riskfreereturn_drop_table.c_str(), stockDB) == -1)
				return -1;
			string riskfreereturn_create_table = "CREATE TABLE RiskFreeReturn (id INT PRIMARY KEY NOT NULL, date CHAR(20) NOT NULL, adjusted_close REAL NOT NULL);";
			if (CreateTable(riskfreereturn_create_table.c_str(), stockDB) == -1)
				return -1;

			string riskfreereturn_data_request = "https://eodhistoricaldata.com/api/eod/TNX.INDX?from=2008-01-01&to=2019-07-31&api_token=5ba84ea974ab42.45160048&period=d&fmt=json";
			Json::Value riskfreereturn_root;
			if (RetrieveMarketData(riskfreereturn_data_request, riskfreereturn_root) == -1)
				return -1;
			if (PopulateRiskFreeReturnTable(riskfreereturn_root, stockDB) == -1)
				return -1;
		}
		else if (option == 5)
		{
			for (vector<string>::iterator it = Symbol.begin(); it != Symbol.end(); it++)
			{
				Stocks.insert({ *it,Stock(*it) });
			}

			string fundamental_select_table = "SELECT * FROM Fundamental;";
			string riskfreereturn_select_table = "SELECT * FROM RiskFreeReturn;";
			string weight_select_table = "SELECT * FROM SP500;";
			int count = 1;
			for (map<string, Stock>::iterator it = Stocks.begin(); it != Stocks.end(); it++)
			{
				if (GetFundamental(fundamental_select_table.c_str(), stockDB, it->second) == -1)
					return -1;
				if (GetWeight(weight_select_table.c_str(), stockDB, it->second) == -1)
					return -1;

				string stockDB_symbol = "Stock_" + it->first;
				if (stockDB_symbol == "Stock_BRK-B")
					stockDB_symbol = "Stock_BRK_B";
				else if (stockDB_symbol == "Stock_BF-B")
					stockDB_symbol = "Stock_BF_B";
				string trade_select_table = "SELECT * FROM " + stockDB_symbol + ";";

				if (GetTrade(trade_select_table.c_str(), stockDB, it->second) == -1)
					return -1;
				if (GetRiskfreerates(riskfreereturn_select_table.c_str(), stockDB, it->second) == -1)
					return -1;
				(it->second).addDailyReturns();

				cout << "Constructed the Stock " << it->first << endl;
			}
		}
		else if (option == 6)
		{
			Population population = GetFirstGeneration(Symbol, Stocks);
			cout << "Created the First Generation" << endl;
			//sort population by fitness score with descending order
			std::sort(population.begin(), population.end(), sortByFitness);

			int generation_number = 1;
			vector<pair<int, int>> portfolio_pool;
			cout << "Strat Populating 1000 Generations" << endl;
			while (generation_number < 40)
			{
				cout << "Start Generation " << generation_number + 1 << endl;
				Population temp;
				portfolio_pool = Selection();
				cout << "Selection done" << endl;
				//keep the top 2 portfolios
				temp.push_back(population[0]);
				cout << "Portfolio 1: " << temp[0];
				temp.push_back(population[1]);
				cout << "Portfolio 2: " << temp[1];
				int count = 3;
				for (vector<pair<int, int>>::iterator it = portfolio_pool.begin(); it != portfolio_pool.end(); it++)
				{
					Portfolio p1 = population[it->first];
					Portfolio p2 = population[it->second];
					//crossover two portfolios
					Crossover(p1, p2, Stocks);
					cout << "Portfolio " << count << ": " << p1;
					temp.push_back(p1);
					count++;
					cout << "Portfolio " << count << ": " << p2;
					temp.push_back(p2);
					count++;
				}
				//mutate the population
				cout << "Mutating the Population" << endl;
				Mutate(temp, Symbol, Stocks);
				cout << "Mutated the Population" << endl;

				population = temp;
				//sort the population
				std::sort(population.begin(), population.end(), sortByFitness);

				generation_number++;
				cout << "Finish Generation " << generation_number << endl;
				if (population[0].fitness == population[99].fitness)
				{
					generation_number = MAX_ALLOWABLE_GENERATIONS;
					cout << "All portfolios have the same fitness number. Population ends!" << endl;
				}
			}
			target_portfolio = population[0];
			cout << "The Best Portfolio: " << target_portfolio;

			vector<string> portfolio_symbol = target_portfolio.GetSymbols();
			//insert the symbols of portfolio's stocks into table
			char portfolio_insert_table[512];
			sprintf_s(portfolio_insert_table, "INSERT INTO Best_Portfolios (stock_1, stock_2, stock_3,stock_4,stock_5,stock_6,stock_7,stock_8,stock_9,stock_10 VALUES(\"%s\", \"%s\", \"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\")", portfolio_symbol[0].c_str(), portfolio_symbol[1].c_str(), portfolio_symbol[2].c_str(), portfolio_symbol[3].c_str(), portfolio_symbol[4].c_str(), portfolio_symbol[5].c_str(), portfolio_symbol[6].c_str(), portfolio_symbol[7].c_str(), portfolio_symbol[8].c_str(), portfolio_symbol[9].c_str());
			if (InsertTable(portfolio_insert_table, stockDB) == -1)
				return -1;
		}
		else if (option == 7)
		{
			// the mondays and fridays from 2018-12-31 to 2019-07-01
			string buy_day[] = { "2018-12-31","2019-01-07","2019-01-14","2019-01-22","2019-01-28","2019-02-04","2019-02-11","2019-02-19","2019-02-25","2019-03-04","2019-03-11","2019-03-18","2019-03-25","2019-04-01","2019-04-08","2019-04-15","2019-04-22","2019-04-29","2019-05-06","2019-05-13","2019-05-20","2019-05-28","2019-06-03","2019-06-10","2019-06-17","2019-06-24" };
			string sell_day[] = { "2019-01-04","2019-01-11","2019-01-18","2019-01-25","2019-02-01","2019-02-08","2019-02-15","2019-02-22","2019-03-01","2019-03-08","2019-03-15","2019-03-22","2019-03-29","2019-04-05","2019-04-12","2019-04-18","2019-04-26","2019-05-03","2019-05-10","2019-05-17","2019-05-24","2019-05-31","2019-06-07","2019-06-14","2019-06-21","2019-06-28" };
			
			vector<string>target_symbol;
			vector<Stock> target_stock;
			string portfolio_select_table = "SELECT * FROM Best_Portfolios;";
			if (GetPortfolio(portfolio_select_table.c_str(), stockDB, target_symbol) == -1)
			return -1;
			for (vector<string>::iterator it = target_symbol.begin(); it != target_symbol.end(); it++)
			{
				string weight_select_table = "SELECT * FROM SP500;";
				Stock temp = Stock(*it);
				if (GetWeight(weight_select_table.c_str(), stockDB, temp) == -1)
					return -1;
				string stockDB_symbol = "Stock_" + *it;
				if (stockDB_symbol == "Stock_BRK-B")					
					stockDB_symbol = "Stock_BRK_B";
				else if (stockDB_symbol == "Stock_BF-B")
					stockDB_symbol = "Stock_BF_B";
				string trade_select_table = "SELECT * FROM " + stockDB_symbol + ";";
				if (GetTrade(trade_select_table.c_str(), stockDB, temp) == -1)
					return -1;
				cout << "Constructed the Stock " << *it << endl;
				target_stock.push_back(temp);
			}
			target_portfolio = Portfolio(target_symbol, target_stock);

			vector<float> Weeks;
			vector<float> SPY_Weeks;
			vector<Stock> stocks = target_portfolio.GetStocks();
			for (int n = 0; n < (int)(sizeof(buy_day) / sizeof(*buy_day)); n++)
			{
				float end_price1 = 0.0;
				float end_price2 = 0.0;
				for (vector<Stock>::iterator it = stocks.begin(); it != stocks.end(); it++)
				{
					float number = 0.0;
					vector<Trade> trades = it->gettrade();
					for (vector<Trade>::iterator itr = trades.begin(); itr != trades.end(); itr++)
					{
						if (itr->getDate() == buy_day[n])
						{
							number = 1000000 * it->weight / itr->getClose();
						}
						else if (itr->getDate() == sell_day[n] && number != 0.0)
						{
							end_price1 += number * itr->getClose();
						}
					}
				}
				Weeks.push_back(end_price1-1000000);
				float number = 0.0;
				vector<Trade> spy_trade = SPY.gettrade();
				for (vector<Trade>::iterator itr = spy_trade.begin(); itr != spy_trade.end(); itr++)
				{
					if (itr->getDate() == buy_day[n])
					{
						number = 1000000 / itr->getClose();
					}
					else if (itr->getDate() == sell_day[n] && number != 0.0)
					{
						end_price2 = number * itr->getClose();
					}
				}
				SPY_Weeks.push_back(end_price2-1000000);
			}

			float earning1 = 0.0;
			float earning2 = 0.0;
			//display header
			cout.width(20); cout.setf(ios::left); cout << "Weeks ";
			cout.width(20); cout.setf(ios::left); cout << "This Portfolio ";
			cout.width(20); cout.setf(ios::left); cout<<"SPY Portfolio ";
			cout.width(20); cout.setf(ios::left); cout << "beats or not ";
			cout.width(20); cout.setf(ios::left); cout << "Current Position Compared to SPY" << endl;
			//display backtesting result
			for (int n = 0; n < Weeks.size(); n++)
			{
				cout.width(20); cout.setf(ios::left); cout << "Week " + to_string(n + 1);
				cout.width(20); cout.setf(ios::left); cout << Weeks[n];
				cout.width(20); cout.setf(ios::left); cout << SPY_Weeks[n];
				cout.width(20); cout.setf(ios::left); cout << bool(Weeks[n] >= SPY_Weeks[n]);
				earning1 += Weeks[n];
				earning2 += SPY_Weeks[n];
				cout.width(20); cout.setf(ios::left); cout << earning1 - earning2 << endl;
			}
			cout.width(20); cout.setf(ios::left); cout << "Final position ";
			cout.width(20); cout.setf(ios::left); cout << earning1;
			cout.width(20); cout.setf(ios::left); cout << earning2;
			cout.width(20); cout.setf(ios::left); cout << bool(earning1 >= earning2);
			cout.width(20); cout.setf(ios::left); cout << earning1 - earning2 << endl;
		}
		else if (option == 8)
		{
			string July_Monday[] = { "2019-07-01", "2019-07-08" ,"2019-07-15","2019-07-22" };
			string July_Friday[] = { "2019-07-05" ,"2019-07-12" ,"2019-07-19","2019-07-26" };
			vector<float> Weeks;
			vector<float> SPY_Weeks;
			vector<Stock> stocks = target_portfolio.GetStocks();
			for (int n = 0; n < (int)(sizeof(July_Monday) / sizeof(*July_Monday)); n++)
			{
				float end_price = 0.0;
				float end_price2 = 0.0;
				for (vector<Stock>::iterator it = stocks.begin(); it != stocks.end(); it++)
				{
					float number = 0.0;
					vector<Trade> trades = it->gettrade();
					for (vector<Trade>::iterator itr = trades.begin(); itr != trades.end(); itr++)
					{
						if (itr->getDate() == July_Monday[n])
						{
							number = 1000000 * it->weight / itr->getClose();
						}
						else if (itr->getDate() == July_Friday[n] && number != 0.0)
						{
							end_price += number * itr->getClose();
						}
					}
				}
				Weeks.push_back(end_price-1000000);
				float number = 0.0;
				vector<Trade> spy_trade = SPY.gettrade();
				for (vector<Trade>::iterator itr = spy_trade.begin(); itr != spy_trade.end(); itr++)
				{
					if (itr->getDate() == July_Monday[n])
					{
						number = 1000000 / itr->getClose();
					}
					else if (itr->getDate() == July_Friday[n] && number != 0.0)
					{
						end_price2 = number * itr->getClose();
					}
				}
				SPY_Weeks.push_back(end_price2-1000000);
			}
			float earning1 = 0.0;
			float earning2 = 0.0;
			//display header
			cout.width(20); cout.setf(ios::left); cout << "Weeks ";
			cout.width(20); cout.setf(ios::left); cout << "This Portfolio ";
			cout.width(20); cout.setf(ios::left); cout << "SPY Portfolio ";
			cout.width(20); cout.setf(ios::left); cout << "beats or not ";
			cout.width(20); cout.setf(ios::left); cout << "Current Position Compared to SPY" << endl;
			//display probation test result
			for (int n = 0; n < Weeks.size(); n++)
			{
				cout.width(20); cout.setf(ios::left); cout << "Week " + to_string(n + 1);
				cout.width(20); cout.setf(ios::left); cout << Weeks[n];
				cout.width(20); cout.setf(ios::left); cout << SPY_Weeks[n];
				cout.width(20); cout.setf(ios::left); cout << bool(Weeks[n] >= SPY_Weeks[n]);
				earning1 += Weeks[n];
				earning2 += SPY_Weeks[n];
				cout.width(20); cout.setf(ios::left); cout << earning1 - earning2 << endl;
			}
			cout.width(20); cout.setf(ios::left); cout << "Final position ";
			cout.width(20); cout.setf(ios::left); cout << earning1;
			cout.width(20); cout.setf(ios::left); cout << earning2;
			cout.width(20); cout.setf(ios::left); cout << bool(earning1 >= earning2);
			cout.width(20); cout.setf(ios::left); cout << earning1 - earning2 << endl;
		}
		else if (option == 9)
		{
			done = false;
		}
		else
		{
			cout << "Wrong Option! Please input option number from 1 to 8." << endl;
		}
	}
	
	// Close Database
	CloseDatabase(stockDB);

	system("pause");
	return 0;
}
