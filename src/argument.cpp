#include <argument.h>

using Runmode = ArgForMto10::Runmode;
using std::fstream;
using std::ios;
Argument::Argument (const int _Argc, char const** const _Argv)
{
	Init (_Argc, _Argv);
}
Argument::~Argument ()
{ }

void Argument::Init (const int _Argc, char const** const _Argv)
{
	argv.clear ();
	string arg, value;
	for (int i = 1; i < _Argc;) {
		if (_Argv[i][0] == '-' && _Argv[i][1] == '-' && _Argv[i][2]) {
			// 长参数
			arg = _Argv[i] + 2;
			if (i + 1 < _Argc && _Argv[i + 1][0] != '-') {
				// 跟着值
				value = _Argv[i + 1];
				argv.emplace (move (arg), move (value));
				i += 2;
			}
			else {
				argv.emplace (move (arg), string (" "));
				++i;
			}
		}
	}
}

pair<bool, string> Argument::getArg (string _Arg)
{
	return { hasSet (_Arg), argv[_Arg] };
}

pair<bool, string> Argument::getArg (char const* const _Arg)
{
	return { hasSet (_Arg), argv[_Arg] };
}

bool Argument::hasSet (string _Arg)
{
	return !argv[_Arg].empty ();
}

pair<bool, string> Argument::hasSetOne (vector<string> _Args)
{
	bool res = false;
	const string* res2 = nullptr;
	for (const auto& arg : _Args) {
		if (!argv[arg].empty ()) {
			if (!res) {
				res = true;
				res2 = &arg;
			}
			else {
				return{ false,"" };
			}
		}
	}
	return { res, res2 == nullptr ? string{} : *res2 };
}

bool Argument::hasSetAll (vector<string> args_)
{
	for (const auto& arg : args_) {
		if (argv[arg].empty ())
			return false;
	}
	return true;
}

bool Argument::hasSet (char const* const _Arg)
{

	return !argv[_Arg].empty ();
}

void Argument::display ()
{
	for (auto& arg : argv) {
		std::cout << arg.first << ":" << arg.second << std::endl;
	}
}

string ArgForMto10::to_string (Runmode r)
{
	switch (r) {
	case ArgForMto10::Runmode::base:
		return "base";
	case ArgForMto10::Runmode::comp:
		return "comp";
	case ArgForMto10::Runmode::none:
		return "none";
	default:
		return "";
	}
}

ArgForMto10::ArgForMto10 (const int _Argc, char const** const _Argv)
{
	arg.Init (_Argc, _Argv);
	this->init ("config/network.config");
#if _HAS_CXX17
	if (auto [res, res2] = arg.hasSetOne ({ "base","competition" });
		res) {
		// 参数不冲突
		if (res2 == "base") {
			runmode = Runmode::base;
		}
		else if (res2 == "competition") {
			runmode = Runmode::comp;
		}
	}
	else {
		return;
	}
	if (auto [b, str] = arg.getArg ("ipaddr");
		b) {
		ipaddr = str;
	}
	if (auto [b, str] = arg.getArg ("port");
		b) {
		port = static_cast<uint16_t>(stoul (str));
	}
	if (auto [b, str] = arg.getArg ("stuno");
		b) {
		stuno = str;
	}
	if (auto [b, str] = arg.getArg ("passwd");
		b) {
		passwd = str;
	}
	if (auto [b, str] = arg.getArg ("mapid");
		b) {
		mapid = static_cast<int>(stoul (str));
	}
	if (auto [b, str] = arg.getArg ("row");
		b) {
		if (auto row_ = static_cast<int>(stoul (str)); row_ == -1 || row_ >= 5 && row_ <= 8)
			row = row_;
	}
	if (auto [b, str] = arg.getArg ("col");
		b) {
		if (auto col_ = static_cast<int>(stoul (str)); col_ == -1 || col_ >= 5 && col_ <= 10)
			col = col_;
	}
	if (auto [b, str] = arg.getArg ("timeout");
		b) {
		if (auto timeout_ = static_cast<int>(stoul (str)); timeout_ >= 2 && timeout_ <= 60)
			timeout = timeout_;
	}
#else
	auto p = arg.hasSetOne ({ "base","competition","multiconnection" });
	if (p.first) {
		if (p.second == "base") {
			runmode = Runmode::base;
		}
		else if (p.second == "competition") {
			runmode = Runmode::comp;
		}
		else if(p.second == "multiconnection"){
			runmode = Runmode::multi;
		}
	}
	else {
		return;
	}
	if(runmode == Runmode::multi){
		
	}
	else
	{
		p = arg.getArg("ipaddr");
		if (p.first)
		{
			ipaddr = p.second;
		}
		p = arg.getArg("port");
		if (p.first)
		{
			port = static_cast<uint16_t>(stoul(p.second));
		}
		p = arg.getArg("stuno");
		if (p.first)
		{
			stuno = p.second;
		}
		p = arg.getArg("passwd");
		if (p.first)
		{
			passwd = p.second;
		}
		p = arg.getArg("mapid");
		if (p.first)
		{
			mapid = static_cast<int>(stoul(p.second));
		}
		p = arg.getArg("row");
		if (p.first)
		{
			auto row_ = static_cast<int>(stoul(p.second));
			if (row_ == -1 || row_ >= 5 && row_ <= 8)
				row = row_;
		}
		p = arg.getArg("col");
		if (p.first)
		{
			auto col_ = static_cast<int>(stoul(p.second));
			if (col_ == -1 || col_ >= 5 && col_ <= 10)
				col = col_;
		}
		p = arg.getArg("timeout");
		if (p.first)
		{
			auto timeout_ = static_cast<int>(stoul(p.second));
			if (timeout_ >= 2 && timeout_ <= 60)
				timeout = timeout_;
		}
#endif
		if (arg.hasSet("stepping"))
		{
			timeout = 60;
			stepping = true;
		}
	}
	// this->display();
}

void ArgForMto10::init (std::string file)
{
#define getStr(x) this->x = rc.getValue(  #x  )
#define getNum(x) this->x = static_cast<decltype(this->x)>(stoul(rc.getValue(  #x  )))
	// default value
	ReadConfig rc (file);
	this->runmode = Runmode::none;
	getStr (ipaddr);
	getStr (stuno);
	getStr (passwd);
	getNum (port);
	getNum (mapid);
	getNum (row);
	getNum (col);
	getNum (timeout);
	this->stepping = false;
}


Runmode ArgForMto10::getRunmode ()
{
	return this->runmode;
}

string ArgForMto10::getipaddr ()
{
	return this->ipaddr;
}

uint16_t ArgForMto10::getport ()
{
	return this->port;
}

string ArgForMto10::getStuno ()
{
	return this->stuno;
}

string ArgForMto10::getPasswd ()
{
	return this->passwd;
}

int ArgForMto10::getMapid ()
{
	return this->mapid;
}

int ArgForMto10::getRow ()
{
	return this->row;
}

int ArgForMto10::getCol ()
{
	return this->col;
}

int ArgForMto10::getTimeout ()
{
	return this->timeout;
}

tuple<string, uint16_t, int, string, string, int, int, int, int>
ArgForMto10::getArgs ()
{
	return std::make_tuple (
		getipaddr (), getport (),
		getMapid (), getStuno (),
		getPasswd (), getRow (),
		getCol (), getTimeout (),
		static_cast<int>(getRunmode ()));
}

bool ArgForMto10::isStepping ()
{
	return this->stepping;
}

void ArgForMto10::display ()
{
	cout << "console argument: " << endl;
	arg.display ();
#define print(x) cout<<#x<<":"<<x<<endl;
	print (ipaddr);
	print (port);
	print (stuno);
	print (passwd);
	print (mapid);
	print (row);
	print (col);
	print (timeout);
	print (stepping);
	print (to_string (runmode));
}



