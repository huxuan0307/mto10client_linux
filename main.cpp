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
"                       必选项：指定运行模式(二选一)\n"
"                          --base        : 基础版(每次一局游戏)\n"
"                          --competition : 竞赛版(一次64局游戏)\n"
"                          --multiconnection : 多进程连接测试\n"
"                          多进程连接测试模式必选项\n"
"						      --loginfile    : 存有用户名和密码的tsv文件\n"
"                             --forknum	     : 分裂进程数，将在tsv文件里取前forknum个用户登录\n"
"                       可选项：(IP地址及端口号)\n"
"                          --ipaddr      : IP地址\n"
"                          --port        : 端口号\n"
"                       可选项：\n"
"                          --stuno       : 学号(缺省:1752877，竞赛和基本版相同)\n"
"                          --passwd      : 课号(缺省:\"xZx3aUL2%kt#-9+&\")\n"
"                       可选项：（仅--base可用）\n"
"                          --mapid       : 地图ID(-1~2147483647，缺省:-1，表示随机)\n"
"                          --row         : 地图的行数(-1/5~8，缺省:-1，表示随机)\n"
"                          --col         : 地图的列数(-1/5~10，缺省:-1，表示随机)\n"
"                          --timeout     : 应答超时(2~60，缺省5，单位s)\n"
"                          --stepping    : 单步执行(指定此项则--timeout强制为60，按回车继续下一步)\n"
"\n"
"e.g.   mto10client --base                                               : 基础版，其余缺省\n"
"       mto10client --base --ipaddr 192.168.1.9 --port 1234              : 基础版，连接192.168.1.9:1234\n"
"       mto10client --base --stuno 2333333 --passwd Hello                : 基础版，学号2333333，密码Hello，其余缺省\n"
"       mto10client --base --mapid 12345 --row 5 --col 8 --stepping      : 基础版，地图ID:12345，5*8，单步执行，其余缺省\n"
"\n"
"       mto10client --competition                                        : 竞赛版，学号密码缺省\n"
"       mto10client --competition --ipaddr 192.168.1.9 --port 1234       : 竞赛版，连接192.168.1.9:1234\n"
"       mto10client --competition --stuno 2333333 --passwd Hello         : 竞赛版，学号2333333，密码Hello\n"
"\n"
"       mto10client --multiconnection --loginfile user.tsv --forknum 50  : 多进程连接测试，进程数50\n");
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
		// 子进程
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
	// 处理 SIGCHLD (子进程退出)信号
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