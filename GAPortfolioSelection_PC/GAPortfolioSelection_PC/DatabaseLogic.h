#pragma once
#ifndef DatabaseLogic_h
#define DatabaseLogic_h

#include <iostream>
#include <string>
#include <map>
#include <fstream>
#include <stdio.h>

#include "curl_easy.h"
#include "curl_form.h"
#include "curl_ios.h"
#include "curl_exception.h"

#include "json/json.h"
#include <sqlite3.h>
#include "MarketDataLogic.h"
#include "Stock.h"

using namespace std;

int OpenDatabase(const char * name, sqlite3 * & db)
{
	int rc = 0;
	//char *error = NULL;
	// Open Database
	cout << "Opening database: " << name << endl;
	rc = sqlite3_open("Stocks.db", &db);
	if (rc)
	{
		cerr << "Error opening SQLite3 database: " << sqlite3_errmsg(db) << endl;
		sqlite3_close(db);
		system("pause");
		return -1;
	}
	cout << "Opened database: " << name << endl;
	return 0;
}

void CloseDatabase(sqlite3 *db)
{
	cout << "Closing a database ..." << endl;
	sqlite3_close(db);
	cout << "Closed a database" << endl << endl;
}

int DropTable(const char * sql_drop_table, sqlite3 *db)
{
	// Drop the table if exists
	if (sqlite3_exec(db, sql_drop_table, 0, 0, 0) != SQLITE_OK) { // or == -- same effect
		std::cout << "SQLite can't drop sessions table" << std::endl;
		sqlite3_close(db);
		system("pause");
		return -1;
	}
	return 0;
}

int CreateTable(const char *sql_create_table, sqlite3 *db)
{
	int rc = 0;
	char *error = NULL;
	// Create the table
	cout << "Creating a table..." << endl;
	rc = sqlite3_exec(db, sql_create_table, NULL, NULL, &error);
	if (rc)
	{
		cerr << "Error executing SQLite3 statement: " << sqlite3_errmsg(db) << endl << endl;
		sqlite3_free(error);
		system("pause");
		return -1;
	}
	cout << "Created a table." << endl;
	return 0;
}

int InsertTable(const char *sql_insert, sqlite3 *db)
{
	int rc = 0;
	char *error = NULL;
	// Execute SQL
	cout << "Inserting a value into a table ..." << endl;
	rc = sqlite3_exec(db, sql_insert, NULL, NULL, &error);
	if (rc)
	{
		cerr << "Error executing SQLite3 statement: " << sqlite3_errmsg(db) << endl << endl;
		sqlite3_free(error);
		system("pause");
		return -1;
	}
	cout << "Inserted a value into the table." << endl;
	return 0;
}

int DisplayTable(const char *sql_select, sqlite3 *db)
{
	int rc = 0;
	char *error = NULL;

	// Display MyTable
	cout << "Retrieving values in a table ..." << endl;
	char **results = NULL;
	int rows, columns;
	// A result table is memory data structure created by the sqlite3_get_table() interface.
	// A result table records the complete query results from one or more queries.
	sqlite3_get_table(db, sql_select, &results, &rows, &columns, &error);
	if (rc)
	{
		cerr << "Error executing SQLite3 query: " << sqlite3_errmsg(db) << endl << endl;
		sqlite3_free(error);
		system("pause");
		return -1;
	}

	// Display Table
	for (int rowCtr = 0; rowCtr <= rows; ++rowCtr)
	{
		for (int colCtr = 0; colCtr < columns; ++colCtr)
		{
			// Determine Cell Position
			int cellPosition = (rowCtr * columns) + colCtr;

			// Display Cell Value
			cout.width(12);
			cout.setf(ios::left);
			cout << results[cellPosition] << " ";
		}

		// End Line
		cout << endl;

		// Display Separator For Header
		if (0 == rowCtr)
		{
			for (int colCtr = 0; colCtr < columns; ++colCtr)
			{
				cout.width(12);
				cout.setf(ios::left);
				cout << "~~~~~~~~~~~~ ";
			}
			cout << endl;
		}
	}
	// This function properly releases the value array returned by sqlite3_get_table()
	sqlite3_free_table(results);
	return 0;
}

int GetStockSymbol(const char *sql_select, sqlite3 *db, vector<string> &Symbol)
{
	int rc = 0;
	char *error = NULL;

	char **results = NULL;
	int rows, columns;
	// A result table is memory data structure created by the sqlite3_get_table() interface.
	// A result table records the complete query results from one or more queries.
	sqlite3_get_table(db, sql_select, &results, &rows, &columns, &error);
	if (rc)
	{
		cerr << "Error executing SQLite3 query: " << sqlite3_errmsg(db) << endl << endl;
		sqlite3_free(error);
		system("pause");
		return -1;
	}

	// Retrieve Table
	// Skip the Header
	for (int rowCtr = 1; rowCtr <= rows; ++rowCtr)
	{
		// Determine Cell Position of Symb
		// id symbol name sector weight shares
		int cellPosition = (rowCtr * columns) + 1;

		// Add Stock Symbol to Vector
		Symbol.push_back(results[cellPosition]);
	}
	// This function properly releases the value array returned by sqlite3_get_table()
	sqlite3_free_table(results);
	return 0;
}

int GetFundamental(const char *sql_select, sqlite3 *db, Stock& stock_)
{
	int rc = 0;
	char *error = NULL;

	char **results = NULL;
	int rows, columns;
	float peratio, divyield, beta, high52, low52, ma50, ma200;
	// A result table is memory data structure created by the sqlite3_get_table() interface.
	// A result table records the complete query results from one or more queries.
	sqlite3_get_table(db, sql_select, &results, &rows, &columns, &error);
	if (rc)
	{
		cerr << "Error executing SQLite3 query: " << sqlite3_errmsg(db) << endl << endl;
		sqlite3_free(error);
		system("pause");
		return -1;
	}

	// Retrieve Table
	// Skip the Header
	for (int rowCtr = 1; rowCtr <= rows; ++rowCtr)
	{
		// id symbol peratio diviendyield beta high_52week low_52week ma_50day ma_200day
		int SymbolPosition = (rowCtr * columns) + 1;
		string symbol = results[SymbolPosition];
		if (symbol == stock_.getsymbol())
		{
			peratio = stof(results[SymbolPosition + 1]);
			divyield = stof(results[SymbolPosition + 2]);
			beta = stof(results[SymbolPosition + 3]);
			high52 = stof(results[SymbolPosition + 4]);
			low52 = stof(results[SymbolPosition + 5]);
			ma50 = stof(results[SymbolPosition + 6]);
			ma200 = stof(results[SymbolPosition + 7]);
			//Fundamental(float peratio_, float divyield_, float beta_, float high52_, float low52_, float ma50_, float ma200_) :peratio(peratio_), divyield(divyield_), beta(beta_), high52(high52_), low52(low52_), ma50(ma50_), ma200(ma200_)
			stock_.addFundamental(Fundamental(peratio, divyield, beta, high52, low52, ma50, ma200));
		}

	}
	// This function properly releases the value array returned by sqlite3_get_table()
	sqlite3_free_table(results);
	return 0;
}

int GetTrade(const char *sql_select, sqlite3 *db, Stock& stock_)
{
	int rc = 0;
	char *error = NULL;

	char **results = NULL;
	int rows, columns;
	string date;
	float open, high, low, close, adjusted_close;
	int volume;
	// A result table is memory data structure created by the sqlite3_get_table() interface.
	// A result table records the complete query results from one or more queries.
	sqlite3_get_table(db, sql_select, &results, &rows, &columns, &error);
	if (rc)
	{
		cerr << "Error executing SQLite3 query: " << sqlite3_errmsg(db) << endl << endl;
		sqlite3_free(error);
		system("pause");
		return -1;
	}

	// Retrieve Table
	// Skip the Header
	for (int rowCtr = 1; rowCtr <= rows; ++rowCtr)
	{
		// id symbol date open high low close adjusted_close volume
		int SymbolPosition = (rowCtr * columns) + 1;
		date = string(results[SymbolPosition + 1]);
		open = stof(results[SymbolPosition + 2]);
		high = stof(results[SymbolPosition + 3]);
		low = stof(results[SymbolPosition + 4]);
		close = stof(results[SymbolPosition + 5]);
		adjusted_close = stof(results[SymbolPosition + 6]);
		volume = stoi(results[SymbolPosition + 7]);
		//Trade(string date_, float open_, float high_, float low_, float close_, float adjusted_close_, int volume_) :date(date_), open(open_), high(high_), low(low_), close(close_), adjusted_close(adjusted_close_), volume(volume_)
		Trade aTrade(date, open, high, low, close, adjusted_close, volume);
		stock_.addTrade(aTrade);
	}
	// This function properly releases the value array returned by sqlite3_get_table()
	sqlite3_free_table(results);
	return 0;
}

int GetRiskfreerates(const char *sql_select, sqlite3 *db, Stock&stock_)
{
	int rc = 0;
	char *error = NULL;

	char **results = NULL;
	int rows, columns;
	// A result table is memory data structure created by the sqlite3_get_table() interface.
	// A result table records the complete query results from one or more queries.
	sqlite3_get_table(db, sql_select, &results, &rows, &columns, &error);
	if (rc)
	{
		cerr << "Error executing SQLite3 query: " << sqlite3_errmsg(db) << endl << endl;
		sqlite3_free(error);
		system("pause");
		return -1;
	}

	// Retrieve Table
	// Skip the Header
	float risk_free_return;
	for (int rowCtr = 1; rowCtr <= rows; ++rowCtr)
	{
		// id date adjusted_close
		int DatePosition = (rowCtr * columns) + 1;
		string date = results[DatePosition];
		risk_free_return = stof(results[DatePosition + 1]);
		stock_.addRiskFreeReturn(date, risk_free_return);
	}
	// This function properly releases the value array returned by sqlite3_get_table()
	sqlite3_free_table(results);
	return 0;
}

int GetWeight(const char *sql_select, sqlite3 *db, Stock& stock_)
{
	int rc = 0;
	char *error = NULL;

	char **results = NULL;
	int rows, columns;
	float weight = 0.0;
	// A result table is memory data structure created by the sqlite3_get_table() interface.
	// A result table records the complete query results from one or more queries.
	sqlite3_get_table(db, sql_select, &results, &rows, &columns, &error);
	if (rc)
	{
		cerr << "Error executing SQLite3 query: " << sqlite3_errmsg(db) << endl << endl;
		sqlite3_free(error);
		system("pause");
		return -1;
	}

	// Retrieve Table
	// Skip the Header
	for (int rowCtr = 1; rowCtr <= rows; ++rowCtr)
	{
		// id symbol name sector weight shares
		int SymbolPosition = (rowCtr * columns) + 1;
		string symbol = results[SymbolPosition];
		if (symbol == stock_.getsymbol())
		{
			weight = stof(results[SymbolPosition + 3]);
		}

	}
	stock_.addWeight(weight);
	// This function properly releases the value array returned by sqlite3_get_table()
	sqlite3_free_table(results);
	return 0;
}

int GetPortfolio(const char *sql_select, sqlite3 *db, vector<string> &symbol)
{
	int rc = 0;
	char *error = NULL;

	char **results = NULL;
	int rows, columns;
	// A result table is memory data structure created by the sqlite3_get_table() interface.
	// A result table records the complete query results from one or more queries.
	sqlite3_get_table(db, sql_select, &results, &rows, &columns, &error);
	if (rc)
	{
		cerr << "Error executing SQLite3 query: " << sqlite3_errmsg(db) << endl << endl;
		sqlite3_free(error);
		system("pause");
		return -1;
	}

	// Retrieve Table
	for(int n =0;n<columns;n++)
	{
		// Determine Cell Position of Symb
		// id symbol name sector
		int cellPosition = (rows*columns)+n;

		// Add Stock Symbol to Vector
		symbol.push_back(results[cellPosition]);
	}
	// This function properly releases the value array returned by sqlite3_get_table()
	sqlite3_free_table(results);
	return 0;
}

//writing call back function for storing fetched values in memory
static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
	((std::string*)userp)->append((char*)contents, size * nmemb);
	return size * nmemb;
}


int RetrieveMarketData(string url_request, Json::Value & root)
{
	std::string readBuffer;

	//global initiliation of curl before calling a function
	curl_global_init(CURL_GLOBAL_ALL);

	//creating session handle
	CURL * myHandle;

	// We’ll store the result of CURL’s webpage retrieval, for simple error checking.
	CURLcode result;

	// notice the lack of major error-checking, for brevity
	myHandle = curl_easy_init();

	curl_easy_setopt(myHandle, CURLOPT_URL, url_request.c_str());
	//curl_easy_setopt(myHandle, CURLOPT_URL, "https://eodhistoricaldata.com/api/eod/AAPL.US?from=2018-01-05&to=2019-02-10&api_token=5ba84ea974ab42.45160048&period=d&fmt=json");

	//adding a user agent
	curl_easy_setopt(myHandle, CURLOPT_USERAGENT, "Mozilla/5.0 (Windows; U; Windows NT 6.1; rv:2.2) Gecko/20110201");
	curl_easy_setopt(myHandle, CURLOPT_SSL_VERIFYPEER, 0);
	curl_easy_setopt(myHandle, CURLOPT_SSL_VERIFYHOST, 0);
	curl_easy_setopt(myHandle, CURLOPT_VERBOSE, 1);

	// send all data to this function  
	curl_easy_setopt(myHandle, CURLOPT_WRITEFUNCTION, WriteCallback);

	// we pass our 'chunk' struct to the callback function */
	curl_easy_setopt(myHandle, CURLOPT_WRITEDATA, &readBuffer);

	//perform a blocking file transfer
	result = curl_easy_perform(myHandle);

	// check for errors 
	if (result != CURLE_OK) {
		fprintf(stderr, "curl_easy_perform() failed: %s\n",
			curl_easy_strerror(result));
	}
	else {
		cout << readBuffer << endl;
		//
		// Now, our chunk.memory points to a memory block that is chunk.size
		// bytes big and contains the remote file.
		//
		// Do something nice with it!
		//

		// https://github.com/open-source-parsers/jsoncpp
		// Using JsonCpp in your project
		// Amalgamated source
		// https ://github.com/open-source-parsers/jsoncpp/wiki/Amalgamated

		//json parsing
		Json::CharReaderBuilder builder;
		Json::CharReader * reader = builder.newCharReader();
		string errors;

		bool parsingSuccessful = reader->parse(readBuffer.c_str(), readBuffer.c_str() + readBuffer.size(), &root, &errors);
		if (not parsingSuccessful)
		{
			// Report failures and their locations
			// in the document.
			cout << "Failed to parse JSON" << std::endl
				<< readBuffer
				<< errors << endl;
			system("pause");
			return -1;
		}
		std::cout << "\nSucess parsing json\n" << root << endl;

	}

	//End a libcurl easy handle.This function must be the last function to call for an easy session
	curl_easy_cleanup(myHandle);
	return 0;
}

int PopulateStockTable(const Json::Value & root, string symbol, sqlite3 *db)
{
	string date;
	float open, high, low, close, adjusted_close;
	unsigned long volume;
	int count = 0;
	for (Json::Value::const_iterator itr = root.begin(); itr != root.end(); itr++)
	{
		//cout << *itr << endl;
		for (Json::Value::const_iterator inner = (*itr).begin(); inner != (*itr).end(); inner++)
		{
			//cout << inner.key() << ": " << *inner << endl;

			if (inner.key().asString() == "adjusted_close")
				adjusted_close = (float)(inner->asDouble());
			else if (inner.key().asString() == "close")
				close = (float)(inner->asDouble());
			else if (inner.key() == "date")
				date = inner->asString();
			else if (inner.key().asString() == "high")
				high = (float)(inner->asDouble());
			else if (inner.key().asString() == "low")
				low = (float)(inner->asDouble());
			else if (inner.key() == "open")
				open = (float)(inner->asDouble());
			else if (inner.key().asString() == "volume")
				volume = (int)(inner->asDouble());
			else
			{
				cout << "Invalid json field" << endl;
				system("pause");
				return -1;
			}
		}
		//Trade aTrade(date, open, high, low, close, adjusted_close, volume);
		//myStock.addTrade(aTrade);
		count++;

		// Execute SQL
		char stockDB_insert_table[512];
		sprintf_s(stockDB_insert_table, "INSERT INTO %s (id, symbol, date, open, high, low, close, adjusted_close, volume) VALUES(%d, \"%s\", \"%s\", %f, %f, %f, %f, %f, %d)", symbol.c_str(), count, symbol.c_str(), date.c_str(), open, high, low, close, adjusted_close, volume);
		if (InsertTable(stockDB_insert_table, db) == -1)
			return -1;
	}
	//cout << myStock;
	return 0;
}

int PopulateMSITable(const Json::Value & root1, const Json::Value & root2, string symbol, sqlite3 *db)
{
	string date;
	float open, high, low, close, adjusted_close;
	int volume;
	//Stock myStock(symbol);
	int count = 0;
	for (Json::Value::const_iterator itr = root1.begin(); itr != root1.end(); itr++)
	{
		//cout << *itr << endl;
		for (Json::Value::const_iterator inner = (*itr).begin(); inner != (*itr).end(); inner++)
		{
			//cout << inner.key() << ": " << *inner << endl;

			if (inner.key().asString() == "adjusted_close")
				adjusted_close = (float)(inner->asDouble());
			else if (inner.key().asString() == "close")
				close = (float)(inner->asDouble());
			else if (inner.key() == "date")
				date = inner->asString();
			else if (inner.key().asString() == "high")
				high = (float)(inner->asDouble());
			else if (inner.key().asString() == "low")
				low = (float)(inner->asDouble());
			else if (inner.key() == "open")
				open = (float)(inner->asDouble());
			else if (inner.key().asString() == "volume")
				volume = (int)(inner->asDouble());
			else
			{
				cout << "Invalid json field" << endl;
				system("pause");
				return -1;
			}
		}
		//Trade aTrade(date, open, high, low, close, adjusted_close, volume);
		//myStock.addTrade(aTrade);
		count++;

		// Execute SQL
		char stockDB_insert_table[512];
		sprintf_s(stockDB_insert_table, "INSERT INTO %s (id, symbol, date, open, high, low, close, adjusted_close, volume) VALUES(%d, \"%s\", \"%s\", %f, %f, %f, %f, %f, %d)", symbol.c_str(), count, symbol.c_str(), date.c_str(), open, high, low, close, adjusted_close, volume);
		if (InsertTable(stockDB_insert_table, db) == -1)
			return -1;
	}
	for (Json::Value::const_iterator itr = root2.begin(); itr != root2.end(); itr++)
	{
		//cout << *itr << endl;
		for (Json::Value::const_iterator inner = (*itr).begin(); inner != (*itr).end(); inner++)
		{
			//cout << inner.key() << ": " << *inner << endl;

			if (inner.key().asString() == "adjusted_close")
				adjusted_close = (float)(inner->asDouble());
			else if (inner.key().asString() == "close")
				close = (float)(inner->asDouble());
			else if (inner.key() == "date")
				date = inner->asString();
			else if (inner.key().asString() == "high")
				high = (float)(inner->asDouble());
			else if (inner.key().asString() == "low")
				low = (float)(inner->asDouble());
			else if (inner.key() == "open")
				open = (float)(inner->asDouble());
			else if (inner.key().asString() == "volume")
				volume = (int)(inner->asDouble());
			else
			{
				cout << "Invalid json field" << endl;
				system("pause");
				return -1;
			}
		}
		//Trade aTrade(date, open, high, low, close, adjusted_close, volume);
		//myStock.addTrade(aTrade);
		count++;

		// Execute SQL
		char stockDB_insert_table[512];
		sprintf_s(stockDB_insert_table, "INSERT INTO %s (id, symbol, date, open, high, low, close, adjusted_close, volume) VALUES(%d, \"%s\", \"%s\", %f, %f, %f, %f, %f, %d)", symbol.c_str(), count, symbol.c_str(), date.c_str(), open, high, low, close, adjusted_close, volume);
		if (InsertTable(stockDB_insert_table, db) == -1)
			return -1;
	}
	//cout << myStock;
	return 0;
}

int PopulateFundamentalTable(const Json::Value & root, int &count, string symbol, sqlite3 *db)
{
	float peratio, divyield, beta, high52, low52, ma50, ma200;
	for (Json::Value::const_iterator itr = root.begin(); itr != root.end(); itr++)
	{
		//cout << *itr << endl;
		if (itr.key().asString() == "Highlights")

			for (Json::Value::const_iterator inner = (*itr).begin(); inner != (*itr).end(); inner++)
			{
				if (inner.key().asString() == "DividendYield")
				{
					if (inner->asString().empty())
						divyield = 0.0;
					else
						divyield = (float)atof(inner->asCString());
				}
				else if (inner.key().asString() == "PERatio")
				{
					if ((inner->asString()).empty())
						peratio = 0.0;
					else
						peratio = (float)atof(inner->asCString());
				}
			}
		else if (itr.key().asString() == "Technicals")
			for (Json::Value::const_iterator inner = (*itr).begin(); inner != (*itr).end(); inner++)
			{
				if (inner.key().asString() == "52WeekHigh")
				{
					if ((inner->asString()).empty())
						high52 = 0.0;
					else
						high52 = (float)atof(inner->asCString());
				}
				else if (inner.key().asString() == "52WeekLow")
				{
					if ((inner->asString()).empty())
						low52 = 0.0;
					else
						low52 = (float)atof(inner->asCString());
				}
				else if (inner.key().asString() == "Beta")
				{
					if ((inner->asString()).empty())
						beta = 0.0;
					else
						beta = (float)atof(inner->asCString());
				}
				else if (inner.key().asString() == "50DayMA")
				{
					if ((inner->asString()).empty())
						ma50 = 0.0;
					else
						ma50 = (float)atof(inner->asCString());
				}
				else if (inner.key().asString() == "200DayMA")
				{
					if ((inner->asString()).empty())
						ma200 = 0.0;
					else
						ma200 = (float)atof(inner->asCString());
				}

			}

	}
	//Fundamental fundamental(peratio, divyield, beta, high52, low52, ma50, ma200);
	//myStock.addFundamental(fundamental);

	// Execute SQL
	char stockDB_insert_table[512];
	sprintf_s(stockDB_insert_table, "INSERT INTO FUNDAMENTAL (id, symbol, PERatio, DividendYield, Beta, High_52Week, Low_52Week, MA_50Day, MA_200Day) VALUES(%d, \"%s\", %f, %f, %f, %f, %f, %f, %f)", count, symbol.c_str(), peratio, divyield, beta, high52, low52, ma50, ma200);
	if (InsertTable(stockDB_insert_table, db) == -1)
		return -1;

	return 0;
}

int PopulateRiskFreeReturnTable(const Json::Value & root, sqlite3 *db)
{
	string date;
	float adjusted_close;
	float close_prices[3000] = {};
	int count = 1;
	for (Json::Value::const_iterator itr = root.begin(); itr != root.end(); itr++)
	{
		//cout << *itr << endl;
		for (Json::Value::const_iterator inner = (*itr).begin(); inner != (*itr).end(); inner++)
		{
			//cout << inner.key() << ": " << *inner << endl;
			{
				if (inner.key() == "date")
					date = inner->asString();
				else if (inner.key().asString() == "adjusted_close")
					adjusted_close = (float)(inner->asDouble()) / 252;
			}
		}
		
		// Execute SQL
		char stockDB_insert_table[512];
		sprintf_s(stockDB_insert_table, "INSERT INTO RiskFreeReturn (id, date, adjusted_close) VALUES(%d, \"%s\", %f)", count, date.c_str(), adjusted_close);
		if (InsertTable(stockDB_insert_table, db) == -1)
			return -1;
		count++;
	}
	return 0;
}

int PopulateSPYTable(const Json::Value & root, sqlite3 *db)
{
	int count = 0;
	string name, symbol, sector;
	float weight;
	int shares;
	for (Json::Value::const_iterator itr = root.begin(); itr != root.end(); itr++)
	{
		cout << *itr << endl;
		for (Json::Value::const_iterator inner = (*itr).begin(); inner != (*itr).end(); inner++)
		{
			if (inner.key().asString() == "Name")
				name = inner->asString();
			else if (inner.key().asString() == "Sector")
				sector = inner->asString();
			else if (inner.key().asString() == "Symbol")
				symbol = inner->asString();
			else if (inner.key().asString() == "Weight")
				weight = (float)stof(inner->asString());
			else if (inner.key().asString() == "Shares")
				shares = (int)atoi(inner->asCString());
		}
		if (symbol == "BRK.B")
			symbol = "BRK-B";
		else if (symbol == "BF.B")
			symbol = "BF-B";

		if (symbol != "LUK" && symbol != "MON")
		{
			count++;

			// Execute SQL
			char sp500_insert_table[512];
			sprintf_s(sp500_insert_table, "INSERT INTO SPY (id, symbol, name, sector, weight, shares) VALUES(%d, \"%s\", \"%s\", \"%s\", %f, %d)", count, symbol.c_str(), name.c_str(), sector.c_str(), weight, shares);
			if (InsertTable(sp500_insert_table, db) == -1)
				return -1;
		}
	}
	return 0;
}

#endif
