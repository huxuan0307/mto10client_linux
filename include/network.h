#define _WINSOCK_DEPRECATED_NO_WARNINGS
#pragma once
#include <iostream>
#include <cstdlib>
#include <cmath>
#include <string>
#include <algorithm>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string>
#include <iomanip>
#include <thread>
#include <future>
#include <chrono>

#include "global.h"
#include "logging.h"
#include "RingBuffer.h"
using namespace std;
using namespace std::chrono;
constexpr int BUFFER_SIZE{ 1024 };



class Network
{
public:
	//DISABLE_COPY (Network);

	Network (string&& ipaddr_, const uint16_t port_);
	Network (const string& ipaddr_, const uint16_t port_);
	Network ();
	void init (const string& ipaddr_, const uint16_t port_);
	void connect ();
	int readFromNet ();
	inline int getBufLen ()
	{
		return rbuf.getSize ();
	}
	void getBytes (char* dataptr, int len);
	string getline ();
	void disconnect ();
	void run ();
	int writeToNet (string&& data);
private:
	void init ();

	string ipaddr;
	uint16_t port;
	int sockclnt;
	int sockfd;
	sockaddr_in server_addr;
	RingBuffer<2048> rbuf;
	RingBuffer<2048> wbuf;
	char buf[1024];
	bool is_init;
protected:
	future<void> th_for_recv;
	bool ready;
	bool shouldClose;
};

class NetworkForMto10 :public Network
{
public:
	NetworkForMto10 (string&& ipaddr_, const uint16_t port_) :Network{ move (ipaddr_), port_ }
	{
		connect();
		start();
	}
	NetworkForMto10 (string const& ipaddr_, const uint16_t port_) :Network{ ipaddr_, port_ }
	{
		connect();
		start();
	}
	NetworkForMto10():Network{}{}
	void start()
	{
		th_for_recv = async (launch::async, &Network::run, this);
	}
	~NetworkForMto10 ()
	{
		disconnect();
		shouldClose = true;
	}
	bool alive(){
		return !shouldClose;
	}
private:
	char buffer[1024];
};