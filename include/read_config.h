#pragma once

#include <iostream>
#include <algorithm>
#include <iomanip>
#include <cstring>
#include <vector>
#include <set>
#include <map>
#include <bitset>
#include <string>
#include <exception>
#include <initializer_list>
#include <fstream>
#include <unordered_map>

using std::unordered_map;
using std::string;
using std::size_t;
using std::vector;
using std::set;
using std::map;
using std::bitset;
using std::initializer_list;
using std::pair;
using std::fstream;
using std::ios;
using std::getline;
using std::find;
using std::cout;
using std::endl;
class ReadConfig
{
public:
	ReadConfig (const string& filepath_)
	{
		filepath = filepath_;
		readConfig_help ();
	}
	string getValue (string);
	void display ();	// for debug
private:
	void readConfig_help ();
	unordered_map<string, string> args;
	string filepath;
};