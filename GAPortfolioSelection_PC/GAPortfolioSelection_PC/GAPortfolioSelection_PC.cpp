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

class Stock
{
private:
	string symbol;
	vector<Trade> trades;

public:
	Stock(string symbol_) :symbol(symbol_)
	{}
	~Stock() {}
	void addTrade(Trade aTrade)
	{
		trades.push_back(aTrade);
	}
	friend ostream & operator << (ostream & out, const Stock & s)
	{
		out << "Symbol: " << s.symbol << endl;
		for (vector<Trade>::const_iterator itr = s.trades.begin(); itr != s.trades.end(); itr++)
			out << *itr;
		return out;
	}
};

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
	int volume;
	Stock myStock(symbol);
	int count = 0;
	for (Json::Value::const_iterator itr = root.begin(); itr != root.end(); itr++)
	{
		//cout << *itr << endl;
		for (Json::Value::const_iterator inner = (*itr).begin(); inner != (*itr).end(); inner++)
		{
			//cout << inner.key() << ": " << *inner << endl;

			if (inner.key().asString() == "adjusted_close")
				adjusted_close = (float)atof(inner->asCString());
			else if (inner.key().asString() == "close")
				close = (float)atof(inner->asCString());
			else if (inner.key() == "date")
				date = inner->asString();
			else if (inner.key().asString() == "high")
				high = (float)atof(inner->asCString());
			else if (inner.key().asString() == "low")
				low = (float)atof(inner->asCString());
			else if (inner.key() == "open")
				open = (float)atof(inner->asCString());
			else if (inner.key().asString() == "volume")
				volume = atoi(inner->asCString());
			else
			{
				cout << "Invalid json field" << endl;
				system("pause");
				return -1;
			}
		}
		Trade aTrade(date, open, high, low, close, adjusted_close, volume);
		myStock.addTrade(aTrade);
		count++;

		// Execute SQL
		char stockDB_insert_table[512];
		sprintf_s(stockDB_insert_table, "INSERT INTO %s (id, symbol, date, open, high, low, close, adjusted_close, volume) VALUES(%d, \"%s\", \"%s\", %f, %f, %f, %f, %f, %d)", symbol.c_str(), count, symbol.c_str(), date.c_str(), open, high, low, close, adjusted_close, volume);
		if (InsertTable(stockDB_insert_table, db) == -1)
			return -1;
	}
	cout << myStock;
	return 0;
}

int PopulateMSITable(const Json::Value & root1, const Json::Value & root2, string symbol, sqlite3 *db)
{
	string date;
	float open, high, low, close, adjusted_close;
	int volume;
	Stock myStock(symbol);
	int count = 0;
	for (Json::Value::const_iterator itr = root1.begin(); itr != root1.end(); itr++)
	{
		//cout << *itr << endl;
		for (Json::Value::const_iterator inner = (*itr).begin(); inner != (*itr).end(); inner++)
		{
			//cout << inner.key() << ": " << *inner << endl;

			if (inner.key().asString() == "adjusted_close")
				adjusted_close = (float)atof(inner->asCString());
			else if (inner.key().asString() == "close")
				close = (float)atof(inner->asCString());
			else if (inner.key() == "date")
				date = inner->asString();
			else if (inner.key().asString() == "high")
				high = (float)atof(inner->asCString());
			else if (inner.key().asString() == "low")
				low = (float)atof(inner->asCString());
			else if (inner.key() == "open")
				open = (float)atof(inner->asCString());
			else if (inner.key().asString() == "volume")
				volume = atoi(inner->asCString());
			else
			{
				cout << "Invalid json field" << endl;
				system("pause");
				return -1;
			}
		}
		Trade aTrade(date, open, high, low, close, adjusted_close, volume);
		myStock.addTrade(aTrade);
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
				adjusted_close = (float)atof(inner->asCString());
			else if (inner.key().asString() == "close")
				close = (float)atof(inner->asCString());
			else if (inner.key() == "date")
				date = inner->asString();
			else if (inner.key().asString() == "high")
				high = (float)atof(inner->asCString());
			else if (inner.key().asString() == "low")
				low = (float)atof(inner->asCString());
			else if (inner.key() == "open")
				open = (float)atof(inner->asCString());
			else if (inner.key().asString() == "volume")
				volume = atoi(inner->asCString());
			else
			{
				cout << "Invalid json field" << endl;
				system("pause");
				return -1;
			}
		}
		Trade aTrade(date, open, high, low, close, adjusted_close, volume);
		myStock.addTrade(aTrade);
		count++;

		// Execute SQL
		char stockDB_insert_table[512];
		sprintf_s(stockDB_insert_table, "INSERT INTO %s (id, symbol, date, open, high, low, close, adjusted_close, volume) VALUES(%d, \"%s\", \"%s\", %f, %f, %f, %f, %f, %d)", symbol.c_str(), count, symbol.c_str(), date.c_str(), open, high, low, close, adjusted_close, volume);
		if (InsertTable(stockDB_insert_table, db) == -1)
			return -1;
	}
	cout << myStock;
	return 0;
}

int PopulateSP500Table(const Json::Value & root, sqlite3 *db, vector<string> & Symbol)
{
	int count = 0;
	string name, symbol, sector;
	for (Json::Value::const_iterator itr = root.begin(); itr != root.end(); itr++)
	{
		cout << *itr << endl;
		for (Json::Value::const_iterator inner = (*itr).begin(); inner != (*itr).end(); inner++)
		{
			//cout << inner.key() << ": " << *inner << endl;

			if (inner.key().asString() == "Name")
				name = inner->asString();
			else if (inner.key().asString() == "Sector")
				sector = inner->asString();
			else if (inner.key() == "Symbol")
				symbol = inner->asString();
			else
			{
				cout << "Invalid json field" << endl;
				system("pause");
				return -1;
			}
		}
		if (symbol == "BRK.B")
			symbol = "BRK-B";
		else if (symbol == "BF.B")
			symbol = "BF-B";

		if (symbol != "LUK" && symbol != "MON")
		{
			count++;
			Symbol.push_back(symbol);

			// Execute SQL
			char sp500_insert_table[512];
			sprintf_s(sp500_insert_table, "INSERT INTO SP500 (id, symbol, name, sector) VALUES(%d, \"%s\", \"%s\", \"%s\")", count, symbol.c_str(), name.c_str(), sector.c_str());
			if (InsertTable(sp500_insert_table, db) == -1)
				return -1;
		}
	}
	return 0;
}

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

	// Close Database
	CloseDatabase(stockDB);

	system("pause");
	return 0;
}
