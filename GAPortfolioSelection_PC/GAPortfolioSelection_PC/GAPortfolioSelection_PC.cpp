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

#define NUM_OF_STOCKS 505

using namespace std;

int main(void)
{
	const char * stockDB_name = "Stocks.db";
	sqlite3 * stockDB = NULL;
	if (OpenDatabase(stockDB_name, stockDB) == -1)
		return -1;

	std::string sp500_drop_table = "DROP TABLE IF EXISTS SP500;";
	if (DropTable(sp500_drop_table.c_str(), stockDB) == -1)
		return -1;

	string sp500_create_table = "CREATE TABLE SP500 (id INT PRIMARY KEY NOT NULL, symbol CHAR(20) NOT NULL, name CHAR(20) NOT NULL, sector CHAR(20) NOT NULL);";

	if (CreateTable(sp500_create_table.c_str(), stockDB) == -1)
		return -1;

	vector<string> Symbol;
	string sp500_data_request = "https://pkgstore.datahub.io/core/s-and-p-500-companies/constituents_json/data/64dd3e9582b936b0352fdd826ecd3c95/constituents_json.json";
	//string sp500_data_request = "https://datahub.io/core/s-and-p-500-companies/r/0.html";
	Json::Value sp500_root;   // will contains the root value after parsing.
	if (RetrieveMarketData(sp500_data_request, sp500_root) == -1)
		return -1;
	if (PopulateSP500Table(sp500_root, stockDB, Symbol) == -1)
		return -1;

	string sp500_select_table = "SELECT * FROM SP500;";
	if (DisplayTable(sp500_select_table.c_str(), stockDB) == -1)
		return -1;

	vector<Stock> Stocks;
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
			if (RetrieveFundamental(stockfundamental_root, myStock) == -1)
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

	// Close Database
	CloseDatabase(stockDB);

	system("pause");
	return 0;
}
