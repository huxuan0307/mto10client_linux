#pragma once
#include <chrono>
#include <string>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <unistd.h>
#include <assert.h>
#include "global.h"
#include "debug.h"
using namespace std;

class Debug_inner;
#if ! _HAS_CXX17
extern string MessageLogger_file;
const string MessageLogger_type[5]{ "Info","Debug","Warning","Critical","Fatal" };
#endif

class MessageLogContext
{
	DISABLE_COPY (MessageLogContext)
public:
	constexpr MessageLogContext () :file{ nullptr }, function{ nullptr }, category{} { };
	constexpr MessageLogContext (const char* fileName, const char* functionName, const char* categoryName)
		: file{ fileName }, function{ functionName }, category{ categoryName }
	{}
	const char* file;
	const char* function;
	const char* category;
private:
	friend class MessageLogger;
	friend class Debug_inner;
};

class MessageLogger
{
	DISABLE_COPY (MessageLogger)
public:
	constexpr MessageLogger () : context () { }
	constexpr MessageLogger (const char* file, const char* function)
		: context (file, function, "default")
	{ }
	constexpr MessageLogger (const char* file, const char* function, const char* category)
		: context (file, function, category)
	{ }

	Debug_inner debug ()const;
	Debug_inner info ()const;
	Debug_inner warning ()const;
	Debug_inner critical ()const;
	Debug_inner fatal ()const;
#if _HAS_CXX17
	inline static void setLogFile (string file) { MessageLogger::file = file; }

#else
	static void setLogFile (string file) { MessageLogger_file = file; }


#endif
private:
	MessageLogContext context;
#if _HAS_CXX17
	inline static string file;
	inline static const string type[5]{ "Info","Debug","Warning","Critical","Fatal" };
#endif
	friend class Debug_inner;

};

#define Debug MessageLogger().debug
#define Info MessageLogger().info
#define Warning MessageLogger().warning
#define Critical MessageLogger().critical
#define Fatal MessageLogger().fatal
