#pragma once

#include <string>
#include <fstream>
#include <assert.h>
#include <memory>
#include <iomanip>
#include <chrono>
#include <ctime>
#include <iostream>
#include "global.h"


using namespace std;
enum class MsgType
{
	InfoMsg, DebugMsg, WarningMsg, CriticalMsg, FatalMsg
};

class Debug_inner
{
	friend class MessageLogger;
	struct Stream
	{
		fstream fs;
		MsgType type;
		bool space;
		Stream (const string& fileName) :fs (fileName, ios::out | ios::app),
			type (MsgType::DebugMsg), space (true)
		{
			// new_assert (fs.is_open (), "File can't be opened!");
			if(!fs.is_open())
				cerr<<"file: "<<fileName<<" can't be opened"<<endl;
			assert(fs.is_open());
		}
		~Stream ()
		{
			fs << endl;
			fs.close ();
		}
	};
	shared_ptr<Stream> stream;
public:
	inline Debug_inner (const string& str) : stream (new Stream (str)) { }
	inline Debug_inner (const Debug_inner& d) : stream{ d.stream } { }
	inline Debug_inner (Debug_inner&& d)noexcept : stream{ d.stream } { }
	~Debug_inner ()
	{

	}
	inline Debug_inner& setMsgType (MsgType t) { this->stream->type = t; return *this; }
	inline Debug_inner& space () { stream->space = true; stream->fs << ' '; return *this; }
	inline Debug_inner& nospace () { stream->space = false; return *this; }
	inline Debug_inner& maybeSpace () { if (stream->space) stream->fs << ' '; return *this; }

	bool autoInsertSpaces () const { return stream->space; }
	void setAutoInsertSpaces (bool b) { stream->space = b; }

	inline Debug_inner& operator<<(bool t) { stream->fs << (t ? "true" : "false"); return maybeSpace (); }
	inline Debug_inner& operator<<(char t) { stream->fs << t; return maybeSpace (); }
	inline Debug_inner& operator<<(signed short t) { stream->fs << t; return maybeSpace (); }
	inline Debug_inner& operator<<(unsigned short t) { stream->fs << t; return maybeSpace (); }
	inline Debug_inner& operator<<(signed int t) { stream->fs << t; return maybeSpace (); }
	inline Debug_inner& operator<<(unsigned int t) { stream->fs << t; return maybeSpace (); }
	inline Debug_inner& operator<<(signed long t) { stream->fs << t; return maybeSpace (); }
	inline Debug_inner& operator<<(unsigned long t) { stream->fs << t; return maybeSpace (); }
	// inline Debug_inner& operator<<(int64_t t) { stream->fs << t; return maybeSpace (); }
	// inline Debug_inner& operator<<(uint64_t t) { stream->fs << t; return maybeSpace (); }
	inline Debug_inner& operator<<(float t) { stream->fs << t; return maybeSpace (); }
	inline Debug_inner& operator<<(double t) { stream->fs << t; return maybeSpace (); }
	inline Debug_inner& operator<<(const char* t) { stream->fs << t; return maybeSpace (); }
	inline Debug_inner& operator<<(const string& t) { stream->fs << t; return maybeSpace (); }
	inline Debug_inner& operator<<(const void* t) { stream->fs << t; return maybeSpace (); }

};
