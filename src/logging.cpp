#define _CRT_SECURE_NO_WARNINGS


#include <logging.h>


string MessageLogger_file;


static inline ostream& operator<<(ostream& os, const chrono::time_point<chrono::system_clock>& t)
{
	const auto tt (chrono::system_clock::to_time_t (t));
	const auto loct (localtime (&tt));
	char ms[4];
	sprintf (ms, "%03lld", t.time_since_epoch ().count () % 10000000LL / 10000);
	os<<loct->tm_year<<"年"
	<<loct->tm_mon<<"月"
	<<loct->tm_mday<<"日 "
	<<setw(2)<<loct->tm_hour<<":"
	<<setw(2)<<loct->tm_min<<":"
	<<setw(2)<<loct->tm_sec<<"."
	<<setw(3)<<ms;
	// os << put_time (loct, "%Y年%m月%d日 %H:%M:%S") << '.' << setw (3) << ms;
	return os;
}



Debug_inner MessageLogger::debug () const
{
#if _HAS_CXX17
	Debug_inner d (this->file);
	d.stream->fs << chrono::system_clock::now () << ' ' << setw (11) << "["s + MessageLogger::type[static_cast<int>(d.stream->type)] + "]";
#else
	Debug_inner d (MessageLogger_file);

	time_t tt;
	char tmp[24];
	tt = time(nullptr);
	strftime(tmp, sizeof(tmp), "%Y年%m月%d日 %H:%M:%S", localtime(&tt));
	d.stream->fs << tmp << " [" << setw(15) <<  MessageLogger_type[static_cast<int>(d.stream->type)] + ":" + to_string(getpid()) + "]";

#endif
	return d;
}

Debug_inner MessageLogger::info () const
{
#if _HAS_CXX17
	Debug_inner d (this->file);
	d.setMsgType (MsgType::InfoMsg);
	d.stream->fs << chrono::system_clock::now () << ' ' << setw (11) << "["s + MessageLogger::type[static_cast<int>(d.stream->type)] + "]";
#else
	Debug_inner d (MessageLogger_file);
	d.setMsgType (MsgType::InfoMsg);
	time_t tt;
	char tmp[24];
	tt = time(nullptr);
	strftime(tmp, sizeof(tmp), "%Y年%m月%d日 %H:%M:%S", localtime(&tt));
	d.stream->fs << tmp << " [" << setw(15) <<  MessageLogger_type[static_cast<int>(d.stream->type)] + ":" + to_string(getpid()) + "]";
#endif
	return d;
}

Debug_inner MessageLogger::warning () const
{
#if _HAS_CXX17
	Debug_inner d (this->file);
	d.setMsgType (MsgType::WarningMsg);
	d.stream->fs << chrono::system_clock::now () << ' ' << setw (11) << "["s + MessageLogger::type[static_cast<int>(d.stream->type)] + "]";
#else
	Debug_inner d (MessageLogger_file);
	d.setMsgType (MsgType::WarningMsg);
	time_t tt;
	char tmp[24];
	tt = time(nullptr);
	strftime(tmp, sizeof(tmp), "%Y年%m月%d日 %H:%M:%S", localtime(&tt));
	d.stream->fs << tmp << " [" << setw(15) <<  MessageLogger_type[static_cast<int>(d.stream->type)] + ":" + to_string(getpid()) + "]";
#endif
	return d;
}

Debug_inner MessageLogger::critical () const
{
#if _HAS_CXX17
	Debug_inner d (this->file);
	d.setMsgType (MsgType::CriticalMsg);
	d.stream->fs << chrono::system_clock::now () << ' ' << setw (11) << "["s + MessageLogger::type[static_cast<int>(d.stream->type)] + "]";
#else
	Debug_inner d (MessageLogger_file);
	d.setMsgType (MsgType::CriticalMsg);
	time_t tt;
	char tmp[24];
	tt = time(nullptr);
	strftime(tmp, sizeof(tmp), "%Y年%m月%d日 %H:%M:%S", localtime(&tt));
	d.stream->fs << tmp << " [" << setw(15) <<  MessageLogger_type[static_cast<int>(d.stream->type)] + ":" + to_string(getpid()) + "]";
#endif
	return d;
}

Debug_inner MessageLogger::fatal () const
{
#if _HAS_CXX17
	Debug_inner d (this->file);
	d.setMsgType (MsgType::FatalMsg);
	d.stream->fs << chrono::system_clock::now () << ' ' << setw (11) << "["s + MessageLogger::type[static_cast<int>(d.stream->type)] + "]";
#else
	Debug_inner d (MessageLogger_file);
	d.setMsgType (MsgType::FatalMsg);
	time_t tt;
	char tmp[24];
	tt = time(nullptr);
	strftime(tmp, sizeof(tmp), "%Y年%m月%d日 %H:%M:%S", localtime(&tt));
	d.stream->fs << tmp << " [" << setw(10) <<  MessageLogger_type[static_cast<int>(d.stream->type)] + ":" + to_string(getpid()) + "]";
#endif
	return d;
}

string bitstreamToString_help(int num, const char *str, int n);

string bitstreamToString(const char* s, const char* p_end)
{
	string res;

	int num = 0;
	const char* str = s;
	int n = 0;
	for (; str + 0x10 < p_end; str += 0x10)	{
		res+=bitstreamToString_help(num, str, 16);
		num += 0x10;
	}
	res+=bitstreamToString_help(num, str, p_end - str);
	return res;
}

string bitstreamToString(const string&s)
{
	return bitstreamToString(s.data(), s.data()+s.size());
}

string bitstreamToString_help(int num, const char *str, int n)
{
	stringstream ss;
	ss << setfill('0') << setw(8) << hex << num << " ";
	ss << setfill(' ');
	ss.unsetf(ios::hex);
	for (int i = 0; i < n; i++)
	{
		if (i % 8 == 0 && i)
			ss << " -";

		ss << " " << setw(2) << setfill('0') << hex << (((unsigned int) str[i]) & 0xff);

	}
	for (int i = n; i < 16; i++)
		ss << "   ";
	ss << "\t";
	for (int i = 0; i < n; i++)
	{
		if (str[i] > 32 && str[i] <= 126)
			ss << str[i];
		else
			ss << '.';
	}
	ss << endl;
	return ss.str();
}