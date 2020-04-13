#define _CRT_SECURE_NO_WARNINGS
#include "network.h"

Network::Network (string&& ipaddr_, const uint16_t port_)
	:ipaddr{ move (ipaddr_) }, port{ port_ }, sockfd{ -1 }, shouldClose{ false }, ready{ false }, is_init{ true }
{
	server_addr.sin_addr.s_addr = inet_addr (ipaddr.data ());
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons (port);
	init ();
}

Network::Network (const string& ipaddr_, const uint16_t port_)
	:ipaddr{ ipaddr_ }, port{ port_ }, sockfd{ -1 }, shouldClose{ false }, ready{ false }, is_init{ true }
{
	server_addr.sin_addr.s_addr = inet_addr (ipaddr.data ());
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons (port);
	init ();
}

Network::Network ()
	:sockfd{ -1 }, shouldClose{ false }, ready{ false }, is_init{ false }
{
	
}

void Network::init (const string& ipaddr_, const uint16_t port_)
{
	this->ipaddr = ipaddr_;
	this->port = port_;
	server_addr.sin_addr.s_addr = inet_addr (ipaddr.data ());
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons (port);
	is_init = true;
	init ();
}


void Network::init ()
{
	assert(is_init);
	// WORD wVersionRequested;
	// WSADATA wsaData;
	// int err;
	// wVersionRequested = MAKEWORD (1, 1);
	// err = WSAStartup (wVersionRequested, &wsaData);
	// if (err != 0) {
	// 	perror ("\n\n-----TCP WSAStartup failed------\n\n");
	// }
	sockclnt = socket (AF_INET, SOCK_STREAM, 0);
}

void Network::connect ()
{
	Info()<<"sock:"<<sockclnt<<"addr:"<<ipaddr<<"port:"<<port;
	while (-1 == (::connect (sockclnt, (sockaddr*)&server_addr, sizeof (sockaddr)))) {
		this_thread::sleep_for (seconds(1));
		perror ("connect");
		Warning () << "Connect with " << ipaddr << ':' << port << "failed, try again...";
	}
	Info () << "Connection estiblished succeccfully!";
	cout << "pid:" << getpid() << " Connection estiblished succeccfully!" << endl;
}

int Network::readFromNet ()
{
	int ret = ::recv (sockclnt, buf, BUFFER_SIZE, 0);
	if (ret == -1) {
		if (errno == EWOULDBLOCK) {
			this_thread::sleep_for (milliseconds(20));
			Warning () << "SOCKET_ERROR:WSAEWOULDBLOCK";
		}
		else if (errno == ETIMEDOUT || errno == ENETDOWN
			|| errno == ECONNRESET) {
			Warning () << "connention interupted!";
		}
		//Critical () << "recv error! errcode=" << err;
		return -1;
	}
	else if (ret == 0) {
		shouldClose = true;
		Info () << "Connection closed by server";
		return 0;
	}
	Info () << "readFromNet" << "get" << ret << "Bytes from Net";
	rbuf.write (buf, ret);
	return ret;
}

int Network::writeToNet (string&& data)
{
	int ret = ::send (sockclnt, data.data (), data.size (), 0);
	if (ret == -1) {
		if (errno == EWOULDBLOCK) {
			this_thread::sleep_for (milliseconds(20));
			Warning () << "SOCKET_ERROR:WSAEWOULDBLOCK";
		}
		return -1;
	}
	else if (ret == 0) {
		shouldClose = true;
		Info () << "Connection closed by server";
		cout << "pid:" << getpid() << ": Connection closed by server";
		// cout<<"!!!"<<endl;
		return -1;
	}
	Info () << "writeToNet" << "put" << ret << "Bytes to Net";
	return ret;
}

void Network::getBytes (char* dataptr, int len)
{
#undef min
	if (len > rbuf.getSize ())
		Warning () << "buffer length less than required";
	rbuf.read (dataptr, min (len, rbuf.getSize ()));
}

string Network::getline ()
{
	return rbuf.readline ();
}

void Network::disconnect ()
{
	close (sockfd);
	Info () << "Disconnected from" << ipaddr << ':' << port;
	cout << "pid:" << getpid() << " Disconnected from" << ipaddr << ':' << port << endl;
}

void Network::run ()
{
	fd_set sockSet;
	FD_ZERO (&sockSet);
	FD_SET (this->sockclnt, &sockSet);
	timeval time = { 1,0 }, time_tmp;
	char buf[2048];

	fd_set readSet, writeSet;
	FD_ZERO (&readSet);
	FD_ZERO (&writeSet);
	ready = true;
	while (true) {
		if (shouldClose) {
			break;
		}
		readSet = sockSet;
		writeSet = sockSet;
		time_tmp = time;
		int nRetAll = select (sockclnt+1, &readSet, nullptr, nullptr, &time_tmp);
		if (nRetAll > 0) {
			if (FD_ISSET (sockclnt, &readSet)) {
				readFromNet ();
			}
		}
	}
}