#define _CRT_SECURE_NO_WARNINGS
#pragma once
#include <memory>
#include <cassert>
#include <algorithm>
#include <string>
#include <vector>
#include <cstring>
#include "global.h"
using namespace std;
//constexpr int BUFFER_HALF{ BUFFER_SIZE >> 1 };
//constexpr int BUFFER_MASK{ BUFFER_SIZE - 1 };
template<int Size>
class Buffer
{
public:
	char data[Size];
};

template<int Size, int BUFFER_MASK = Size - 1>
class RingBuffer
{
public:
	RingBuffer ()
		:bufptr{ new Buffer< Size > }, datasize{ 0 }, writeptr{ bufptr->data }, readptr{ bufptr->data }
	{
	};

	int write (const char* dataptr, int datalen)
	{
		auto&& baseptr = this->bufptr->data;
		int writelen{ 0 }, leftlen{ 0 };
		constexpr int buflen{ Size };
		// new_assert (dataptr != nullptr, "dataptr is nullptr!!!");
		assert(dataptr!=nullptr);
		if (full ()) {
			return 0;
		}
		// left space for writting
		leftlen = buflen - datasize;
		// real length that can be written
		writelen = min (datalen, leftlen);

		// free space is consistant 
		if (writeptr < readptr) {
			memcpy (writeptr, dataptr, writelen);
		}
		// free space is not consistant
		else {
			// free space between writeptr and end of buf space
			int taillen = buflen - static_cast<int>(writeptr - baseptr);
			if (writelen <= taillen) {
				memcpy (writeptr, dataptr, writelen);
			}
			else {
				// write between writeptr and end of buf space
				memcpy (writeptr, dataptr, taillen);
				// write left data
				memcpy (baseptr, dataptr + taillen, writelen - taillen);
			}
		}
		writeptr = baseptr + ((writeptr - baseptr + writelen) & BUFFER_MASK);
		datasize += writelen;
		return writelen;
	}
	int read (char* dataptr, int datalen)
	{
		auto&& baseptr = this->bufptr->data;
		int readlen{ 0 }, taillen{ 0 };
		constexpr int buflen{ Size };
		// new_assert (dataptr != nullptr, "dataptr is nullptr");
		assert(dataptr != nullptr);
		if (empty ())
			return 0;
		// max data length that can be read
		readlen = min (datalen, datasize);
		// data between readptr and end of buf space
		taillen = buflen - static_cast<int>(readptr - baseptr);
		if (readlen <= taillen) {
			memcpy (dataptr, readptr, readlen);
		}
		else {
			memcpy (dataptr, readptr, taillen);
			memcpy (dataptr + taillen, baseptr, readlen - taillen);
		}
		readptr = baseptr + ((readptr - baseptr + readlen) & BUFFER_MASK);
		datasize -= readlen;

		return readlen;
	}
	vector<char> readall ()
	{
		vector<char> res (datasize);
		read (res.data (), datasize);
		return res;
	}

	string readline ()
	{
		if (empty ())
			return {};
		auto&& baseptr = this->bufptr->data;

		if (readptr < writeptr) {
			for (auto p = readptr; p < writeptr; ++p) {
				if (*p == '\n') {
					int linelen = p - readptr;
					datasize -= linelen + 1;
					string res (readptr, linelen);
					readptr += linelen + 1;
					if (res.back () == '\r')
						res.pop_back ();
					return res;
				}
			}
			auto res = readall ();
			return string (begin (res), end (res));
		}
		else {
			for (auto p = readptr; p < baseptr + Size; ++p) {
				if (*p == '\n') {
					int linelen = p - readptr;
					datasize -= linelen + 1;
					string res (readptr, linelen);
					readptr += linelen + 1;
					if (readptr > baseptr + Size)
						readptr = baseptr;
					if (res.back () == '\r')
						res.pop_back ();
					return res;
				}
			}
			int taillen = baseptr + Size - readptr;
			datasize -= taillen;
			string res (readptr, taillen);
			readptr = baseptr;
			for (auto p = readptr; p < writeptr; ++p) {
				if (*p == '\n') {
					int linelen = p - baseptr;
					datasize -= linelen + 1;
					res += string (readptr, linelen);
					readptr += linelen + 1;
					if (res.back () == '\r')
						res.pop_back ();
					return res;
				}
			}
			res += string (readptr, datasize);
			readptr = writeptr;
			datasize = 0;
			return res;
		}

	}
	bool full ()
	{
		return datasize == Size;
	}

	bool empty ()
	{
		return datasize == 0;
	}

	inline int getSize ()
	{
		return datasize;
	}

private:
	unique_ptr<Buffer<Size>> bufptr;
	int datasize;
	const char* readptr;
	char* writeptr;
};
