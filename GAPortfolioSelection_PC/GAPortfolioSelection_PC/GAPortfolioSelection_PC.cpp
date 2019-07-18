#include <iostream>
#include <string>
#include <map>
#include <fstream>
#include <vector>
#include <stdio.h>
#include<algorithm>

#include "curl_easy.h"
#include "curl_form.h"
#include "curl_ios.h"
#include "curl_exception.h"

#include "json/json.h"
#include <sqlite3.h>

#include "MarketDataLogic.h"
#include "DatabaseLogic.h"
#include "Portfolio.h"
#include "GALogic.h"
#include "Utility.h"

#define NUM_OF_STOCKS 505

using namespace std;

int main(void)
{
	
	
	//seed the random number generator
	srand((int)time(NULL));

	const char * stockDB_name = "Stocks.db";
	sqlite3 * stockDB = NULL;
	if (OpenDatabase(stockDB_name, stockDB) == -1)
		return -1;

	/*
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
	//cout << json_SPY_All_holdings << endl;
	std::string spy_drop_table = "DROP TABLE IF EXISTS SPY;";
	if (DropTable(spy_drop_table.c_str(), stockDB) == -1)
		return -1;

	string spy_create_table = "CREATE TABLE SPY (id INT PRIMARY KEY NOT NULL, symbol CHAR(20) NOT NULL, name CHAR(20) NOT NULL, sector CHAR(20) NOT NULL, weight REAL NOT NULL, shares INT NOT NULL);";

	if (CreateTable(spy_create_table.c_str(), stockDB) == -1)
		return -1;
	if (PopulateSPYTable(json_SPY_All_holdings,stockDB) == -1)
		return -1;

	//----------Create SP500 Table----------
	
	std::string sp500_drop_table = "DROP TABLE IF EXISTS SP500;";
	if (DropTable(sp500_drop_table.c_str(), stockDB) == -1)
		return -1;

	string sp500_create_table = "CREATE TABLE SP500 (id INT PRIMARY KEY NOT NULL, symbol CHAR(20) NOT NULL, name CHAR(20) NOT NULL, sector CHAR(20) NOT NULL);";

	if (CreateTable(sp500_create_table.c_str(), stockDB) == -1)
		return -1;

	string sp500_data_request = "https://pkgstore.datahub.io/core/s-and-p-500-companies/constituents_json/data/64dd3e9582b936b0352fdd826ecd3c95/constituents_json.json";
	//string sp500_data_request = "https://datahub.io/core/s-and-p-500-companies/r/0.html";
	Json::Value sp500_root;   // will contains the root value after parsing.
	if (RetrieveMarketData(sp500_data_request, sp500_root) == -1)
		return -1;
	if (PopulateSP500Table(sp500_root, stockDB) == -1)
		return -1;

	string sp500_select_table = "SELECT * FROM SP500;";
	if (DisplayTable(sp500_select_table.c_str(), stockDB) == -1)
		return -1;
	*/
	
	vector<string> Symbol;
	map<string, Stock> Stocks;

	string spy_select_table = "SELECT * FROM SP500_;";
	if (GetStockSymbol(spy_select_table.c_str(), stockDB, Symbol) == -1)
		return -1;

	bool done = true;

	while (done)
	{
		string input;
		cout << "Please select a option number: " << endl << endl
			<< "1: Retrieve SP500 Stock Symbol " << endl
			<< "2: Retrieve Stock Trade Data and Save to Database " << endl
			<< "3: Retrieve Stock Fundamental Data and Save to Database " << endl
			<< "4: Retrieve Risk Free Rate Data and Save to Database " << endl
			<< "5: Construct All Stock Objects " << endl
			<< "6: Create First Generation and Populate 1000 Generations" << endl
			<< "7: Start Testing " << endl
			<< "8: Exit " << endl << endl;
		cin >> input;
		int option = stoi(input);
		if (option == 1)
		{
			string sp500_select_table = "SELECT * FROM SP500_;";
			if (GetStockSymbol(sp500_select_table.c_str(), stockDB, Symbol) == -1)
				return -1;
			for (vector<string>::iterator it = Symbol.begin(); it != Symbol.end(); it++)
			{
				cout << *it << endl;
			}
			cout << endl;
		}
		else if (option == 2)
		{
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
					string stock_end_date = "2019-06-06";
					string api_token = "5ba84ea974ab42.45160048";
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
					string stock_url_common = "https://eodhistoricaldata.com/api/eod/";
					string stock_start_date1 = "2008-01-01";
					string stock_end_date1 = "2017-11-20";
					string stock_start_date2 = "2018-01-02";
					string stock_end_date2 = "2019-06-06";
					string api_token = "5ba84ea974ab42.45160048";
					string stockDB_data_request1 = stock_url_common + *it + ".US?" +
						"from=" + stock_start_date1 + "&to=" + stock_end_date1 + "&api_token=" + api_token + "&period=d&fmt=json";
					string stockDB_data_request2 = stock_url_common + *it + ".US?" +
						"from=" + stock_start_date2 + "&to=" + stock_end_date2 + "&api_token=" + api_token + "&period=d&fmt=json";

					Json::Value stockDB_root1;   // will contains the root value after parsing.
					Json::Value stockDB_root2;   // will contains the root value after parsing.
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
		else if (option == 3)
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
		else if (option == 4)
		{
			std::string riskfreereturn_drop_table = "DROP TABLE IF EXISTS RiskFreeReturn;";
			if (DropTable(riskfreereturn_drop_table.c_str(), stockDB) == -1)
				return -1;
			string riskfreereturn_create_table = "CREATE TABLE RiskFreeReturn (id INT PRIMARY KEY NOT NULL, date CHAR(20) NOT NULL, adjusted_close REAL NOT NULL);";
			if (CreateTable(riskfreereturn_create_table.c_str(), stockDB) == -1)
				return -1;

			string riskfreereturn_data_request = "https://eodhistoricaldata.com/api/eod/TNX.INDX?from=2008-01-01&to=2019-07-10&api_token=5ba84ea974ab42.45160048&period=d&fmt=json";
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
			string weight_select_table = "SELECT * FROM SPY;";
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
				
				(it->second).addDailyReturns();

				if (GetRiskfreerates(riskfreereturn_select_table.c_str(), stockDB, it->second) == -1)
					return -1;
				cout << "Constructed the Stock " << it->first << endl;
			}
		}
		else if (option == 6)
		{
			Population population = GetFirstGeneration(Symbol, Stocks);
			int portfolio_number = 1;
			ofstream myfile;
			myfile.open("portfolios.txt");
			for (Population::iterator it = population.begin(); it != population.end(); it++)
			{
				cout << "Portfolio " << portfolio_number << ": " << it->fitness << endl;
				myfile << it->sharperatio << ", " << 1 / (it->diverindex) << endl;
				portfolio_number += 1;
			}
			cout << "Created the First Generation" << endl << endl;
			myfile.close();
			//sort population by fitness score with descending order
			sort ( population.begin(), population.end(), greater<Portfolio>());

			int generation_number = 1;
			vector<pair<Portfolio, Portfolio>> portfolio_pool;
			cout << "Strat Populating 1000 Generations" << endl;
			while (generation_number <MAX_ALLOWABLE_GENERATIONS)
			{
				Population temp;
				//keep the top 2 portfolios
				temp.push_back(*population.begin());
				temp.push_back(*(population.begin() + 1));
				portfolio_pool = Selection(population);
				for (vector<pair<Portfolio, Portfolio>>::iterator it = portfolio_pool.begin(); it != portfolio_pool.end(); it++)
				{
					Portfolio p1 = it->first;
					Portfolio p2 = it->second;
					//crossover two portfolios
					Crossover(p1, p2, Stocks);
					temp.push_back(p1);
					temp.push_back(p2);
				} 
				//mutate the population
				Mutate(temp, Symbol, Stocks);
				//sort the population
				sort(temp.begin(), temp.end(), greater<Portfolio>());

				population = temp;
				generation_number++;
			}
			cout << "The Best Portfolio is " << endl;
			cout << *population.begin() << endl;
        }
		else if (option == 7)
		{
			
		}
		else if (option == 8)
		{
			done = false;
		}
	}

	//----------Create New SP500 Table----------
	/*
	std::string sp500_drop_table = "DROP TABLE IF EXISTS SP500_;";
	if (DropTable(sp500_drop_table.c_str(), stockDB) == -1)
		return -1;

	string sp500_create_table = "CREATE TABLE SP500_ (id INT PRIMARY KEY NOT NULL, symbol CHAR(20) NOT NULL, name CHAR(20) NOT NULL, sector CHAR(20) NOT NULL);";

	if (CreateTable(sp500_create_table.c_str(), stockDB) == -1)
		return -1;

	string sp500_data_request = "http://www.json-generator.com/api/json/get/cgeCcApIOa?indent=2";
	//string sp500_data_request = "https://datahub.io/core/s-and-p-500-companies/r/0.html";
	Json::Value sp500_root;   // will contains the root value after parsing.
	if (RetrieveMarketData(sp500_data_request, sp500_root) == -1)
		return -1;
	if (PopulateNewSP500Table(sp500_root, stockDB) == -1)
		return -1;

	string sp500_select_table = "SELECT * FROM SP500_;";
	if (DisplayTable(sp500_select_table.c_str(), stockDB) == -1)
		return -1;
	*/

	//----------Retrieve Stock Symbols----------
	/*
	vector<string> Symbol;
	string sp500_select_table = "SELECT * FROM SP500;";
	if (GetStockSymbol(sp500_select_table.c_str(), stockDB, Symbol) == -1)
		return -1;
	*/

	//----------Populate Risk Free Return Table----------
	/*
	std::string riskfreereturn_drop_table = "DROP TABLE IF EXISTS RiskFreeReturn;";
	if (DropTable(riskfreereturn_drop_table.c_str(), stockDB) == -1)
		return -1;
	string riskfreereturn_create_table = "CREATE TABLE RiskFreeReturn (id INT PRIMARY KEY NOT NULL, date CHAR(20) NOT NULL, adjusted_close REAL NOT NULL, risk_free_return REAL NOT NULL);";
	if (CreateTable(riskfreereturn_create_table.c_str(), stockDB) == -1)
		return -1;

	string riskfreereturn_data_request = "https://eodhistoricaldata.com/api/eod/TNX.INDX?from=2008-01-01&to=2019-07-10&api_token=5ba84ea974ab42.45160048&period=d&fmt=json";
	Json::Value riskfreereturn_root;
	if (RetrieveMarketData(riskfreereturn_data_request, riskfreereturn_root) == -1)
		return -1;
	if (PopulateRiskFreeReturnTable(riskfreereturn_root, stockDB) == -1)
		return -1;
	*/

	//----------Pupolate Fundamental Table----------
	/*
	vector<Stock> Stocks;
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
		Stock myStock(*it);
		string stockfundamental_data_request = "https://eodhistoricaldata.com/api/fundamentals/" + *it + ".US?api_token=5ba84ea974ab42.45160048";
		Json::Value stockfundamental_root;
		if (RetrieveMarketData(stockfundamental_data_request, stockfundamental_root) == -1)
			return -1;
		if (PopulateFundamentalTable(stockfundamental_root, count, *it, myStock, stockDB) == -1)
			return -1;
		count++;
		//Stocks.push_back(myStock);
	}
	*/

	//----------Populate Stock Table----------
	/*
	for (vector<string>::iterator it = Symbol.begin(); it != Symbol.end(); it++)
	{
		Stock myStock(*it);
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
			string stock_end_date = "2019-06-06";
			string api_token = "5ba84ea974ab42.45160048";
			string stockDB_data_request = stock_url_common + *it + ".US?" +
				"from=" + stock_start_date + "&to=" + stock_end_date + "&api_token=" + api_token + "&period=d&fmt=json";

			Json::Value stockDB_root;   // will contains the root value after parsing.
			if (RetrieveMarketData(stockDB_data_request, stockDB_root) == -1)
				return -1;
			if (PopulateStockTable(stockDB_root, stockDB_symbol, myStock, stockDB) == -1)
				return -1;

			string stockfundamental_data_request = "https://eodhistoricaldata.com/api/fundamentals/" + *it + ".US?api_token=5ba84ea974ab42.45160048";
			Json::Value stockfundamental_root;
			if (RetrieveMarketData(stockfundamental_data_request, stockfundamental_root) == -1)
				return -1;
			if (PopulateFundamentalTable(stockfundamental_root, stockDB_symbol, myStock) == -1)
				return - 1;
			myStock.addDailyReturns();
			Stocks.push_back(myStock);

		}
		else
		{
			string stock_url_common = "https://eodhistoricaldata.com/api/eod/";
			string stock_start_date1 = "2008-01-01";
			string stock_end_date1 = "2017-11-20";
			string stock_start_date2 = "2018-01-02";
			string stock_end_date2 = "2019-06-06";
			string api_token = "5ba84ea974ab42.45160048";
			string stockDB_data_request1 = stock_url_common + *it + ".US?" +
				"from=" + stock_start_date1 + "&to=" + stock_end_date1 + "&api_token=" + api_token + "&period=d&fmt=json";
			string stockDB_data_request2 = stock_url_common + *it + ".US?" +
				"from=" + stock_start_date2 + "&to=" + stock_end_date2 + "&api_token=" + api_token + "&period=d&fmt=json";

			Json::Value stockDB_root1;   // will contains the root value after parsing.
			Json::Value stockDB_root2;   // will contains the root value after parsing.
			if (RetrieveMarketData(stockDB_data_request1, stockDB_root1) == -1)
				return -1;
			if (RetrieveMarketData(stockDB_data_request2, stockDB_root2) == -1)
				return -1;
			if (PopulateMSITable(stockDB_root1, stockDB_root2, stockDB_symbol, myStock, stockDB) == -1)
				return -1;


			string stockfundamental_data_request = "https://eodhistoricaldata.com/api/fundamentals/" + *it + ".US?api_token=5ba84ea974ab42.45160048";
			Json::Value stockfundamental_root;
			if (RetrieveMarketData(stockfundamental_data_request, stockfundamental_root) == -1)
				return -1;
			if (RetrieveFundamental(stockfundamental_root, myStock) == -1)
				return -1;
			myStock.addDailyReturns();
			Stocks.push_back(myStock);

		}

		string stockDB_select_table = "SELECT * FROM " + stockDB_symbol + ";";
		if (DisplayTable(stockDB_select_table.c_str(), stockDB) == -1)
			return -1;
	}
	*/

	// Close Database
	CloseDatabase(stockDB);

	system("pause");
	return 0;
}
