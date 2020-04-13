#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <iostream>
#include <cstdlib>
#include <cmath>
#include <string>
#include <algorithm>
#include <thread>
#include "logging.h"
#include "network.h"
#include "RingBuffer.h"
#include "mto10.h"
#include <md5.h>
#include <unistd.h>
#include <signal.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>  

using namespace std;

void Usage(){
puts(
"Usage: mto10client 	{ --base | --competition | --multiconnection }\n"
"                       { --stuno student_no | --passwd password_str }\n"
"                       { --mapid x | --row x | --col x | --timeout ms }\n"
"                       { --stepping }\n"
"\n"
"                       ��ѡ�ָ������ģʽ(��ѡһ)\n"
"                          --base        : ������(ÿ��һ����Ϸ)\n"
"                          --competition : ������(һ��64����Ϸ)\n"
"                          --multiconnection : ��������Ӳ���\n"
"                          ��������Ӳ���ģʽ��ѡ��\n"
"						      --loginfile    : �����û����������tsv�ļ�\n"
"                             --forknum	     : ���ѽ�����������tsv�ļ���ȡǰforknum���û���¼\n"
"                       ��ѡ�(IP��ַ���˿ں�)\n"
"                          --ipaddr      : IP��ַ\n"
"                          --port        : �˿ں�\n"
"                       ��ѡ�\n"
"                          --stuno       : ѧ��(ȱʡ:1752877�������ͻ�������ͬ)\n"
"                          --passwd      : �κ�(ȱʡ:\"xZx3aUL2%kt#-9+&\")\n"
"                       ��ѡ�����--base���ã�\n"
"                          --mapid       : ��ͼID(-1~2147483647��ȱʡ:-1����ʾ���)\n"
"                          --row         : ��ͼ������(-1/5~8��ȱʡ:-1����ʾ���)\n"
"                          --col         : ��ͼ������(-1/5~10��ȱʡ:-1����ʾ���)\n"
"                          --timeout     : Ӧ��ʱ(2~60��ȱʡ5����λs)\n"
"                          --stepping    : ����ִ��(ָ��������--timeoutǿ��Ϊ60�����س�������һ��)\n"
"\n"
"e.g.   mto10client --base                                               : �����棬����ȱʡ\n"
"       mto10client --base --ipaddr 192.168.1.9 --port 1234              : �����棬����192.168.1.9:1234\n"
"       mto10client --base --stuno 2333333 --passwd Hello                : �����棬ѧ��2333333������Hello������ȱʡ\n"
"       mto10client --base --mapid 12345 --row 5 --col 8 --stepping      : �����棬��ͼID:12345��5*8������ִ�У�����ȱʡ\n"
"\n"
"       mto10client --competition                                        : �����棬ѧ������ȱʡ\n"
"       mto10client --competition --ipaddr 192.168.1.9 --port 1234       : �����棬����192.168.1.9:1234\n"
"       mto10client --competition --stuno 2333333 --passwd Hello         : �����棬ѧ��2333333������Hello\n"
"\n"
"       mto10client --multiconnection --loginfile user.tsv --forknum 50  : ��������Ӳ��ԣ�������50\n");
}

void multiconnection(const string& userfile, const int numofprocess, mto10_param_t param){
	fstream fin;
	fin.open(userfile, ios::in);
	if(!fin.is_open()){
		cerr<<"file "<<userfile<<" cannot be opened!"<<endl;
		Usage();
		return;
	}
	get<8>(param) = 2; // competition
	string stuno;
	string passwd;
	pid_t pid;
	for(int i=0;i<numofprocess;){
		fin >> stuno >> passwd;
		pid = fork();
		if(pid < 0){
			this_thread::sleep_for(milliseconds(500));
		}
		if(pid > 0){
			++i;
		}
		else if (pid == 0){
			fin.close();
			break;
		}
	}

	if(pid==0){
		// �ӽ���
		MessageLogger::setLogFile (string("./log/runtimelog")+stuno+".txt");
		// <ipaddr, port, mapid, stuno, passwd, row, col, delay, runmode>
		get<3>(param) = stuno;
		get<4>(param) = passwd;
		MTO10 mto10(param, SolveType::Algo1);
	}else{
		while(waitpid(-1,nullptr,0)){
			if(errno == ECHILD){
				break;
			}
		}
	}
	
}

#if _HAS_CXX17
using var = variant<int, uint16_t, string>;
int main (const int _Argc, char const** const _Argv)
{
	MessageLogger::setLogFile ("./log/runtimelog.txt"s);
	//string ipaddr = "10.60.102.252";
	//uint16_t port = 21345;
	//NetworkForMto10 network (move (ipaddr), port);

	//while (network.getBufLen () < 88)
	//	;
	//auto res = network.getRandStr ();
	//for (char c : res) {
	//	putchar (c);
	//}


	var v = "10.60.102.252"s;
	unordered_map<string, var> m;
	m["ipaddr"] = "10.60.102.252"s;
	m["port"] = static_cast<uint16_t>(21345u);
	m["mapid"] = 0;
	m["stuno"] = "1234567";
	m["passwd"] = "0HhJ)j8JGx+3uq.#";
	m["row"] = 5;
	m["col"] = 5;
	m["delay"] = 60;
	MTO10 mto10 (m);

}

#else

int main (const int _Argc, char const** const _Argv)
{
	if(_Argc == 1){
		Usage();
		exit(0);
	}
	// ���� SIGCHLD (�ӽ����˳�)�ź�
	signal(SIGCHLD, SIG_IGN);
	MessageLogger::setLogFile ("./log/runtimelog.txt");
	// cout << MessageLogger_file << endl;
	ArgForMto10 args (_Argc, _Argv);
	Argument args2(_Argc, _Argv);
	string loginfile;
	int forknum;
	if(args2.hasSet("multiconnection")){
		if(args2.hasSet("loginfile")){
			auto p=args2.getArg("loginfile");
			if(p.first){
				loginfile = p.second;
				p = args2.getArg("forknum");
				if(p.first){
					forknum = stoi(p.second);
				}
				multiconnection(loginfile, forknum, args.getArgs());
			}else{
				Usage();
				exit(0);
			}
		}else{
			Usage();
			exit(0);
		}
	}
	else{
		MTO10 mto10 (args.getArgs (), SolveType::Algo1);
	}
}
#endif

// --base --stuno 1752877 --passwd "xZx3aUL2%kt#-9+&"