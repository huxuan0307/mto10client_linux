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
#include <global.h>
#include "read_config.h"

using std::string;
using std::size_t;
using std::vector;
using std::set;
using std::map;
using std::bitset;
using std::initializer_list;
using std::pair;
using std::stoi;
using std::stoul;
using std::tuple;

class Argument
{
	struct arg_t
	{
		string arg;
		string value;
	};

public:
	Argument () { };
	Argument (const int _Argc, char const** const _Argv);
	~Argument ();
	void Init (const int _Argc, char const** const _Argv);
	pair<bool, string> getArg (string _Arg);	//由参数名获取参数值
	pair<bool, string> getArg (char const* const _Arg);
	bool hasSet (string _Arg);
	pair<bool, string> hasSetOne (vector<string> _Arg);
	bool hasSetAll (vector<string> args_);
	bool hasSet (char const* const _Arg);
	void display (); //for debug
private:
	map<string, string> argv;
};



class ArgForMto10:public Argument
{
public:
	enum class Runmode
	{
		base = 0x01, comp = 0x02, multi = 0x04, none = 0x00
	};
	string to_string (Runmode r);
	ArgForMto10 (const int _Argc, char const** const _Argv);
	void init (std::string file);
	Runmode getRunmode ();
	string getipaddr ();
	uint16_t getport ();

	string getStuno ();
	string getPasswd ();

	int getMapid ();
	int getRow ();
	int getCol ();
	int getTimeout ();
	tuple<string, uint16_t, int, string, string, int, int, int, int>
	getArgs();

	bool isStepping ();

	void display ();

private:
	Argument arg;
	// 运行模式
	Runmode runmode;

	string ipaddr;
	uint16_t port;
	string stuno;
	string passwd;
	int mapid;
	int row;
	int col;
	int timeout;
	bool stepping;
};
