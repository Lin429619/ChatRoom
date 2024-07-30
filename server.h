#ifndef SERVER_H
#define SERVER_H

#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <iostream>
#include <thread>
#include <vector>
#include <mysql/mysql.h>
#include <unordered_map>
#include <pthread.h>
using namespace std;

class server {
    private:
        int server_port;
        int server_sockfd;
        string server_ip;
        static vector<bool> sock_arr;
        static unordered_map<string, int> name_sock_map; //名字和套接字描述符
        static pthread_mutex_t name_sock_mutex; //互斥锁，锁住需要修改name_sock_map的临界区
    public:
        server(int port, string ip);
        ~server();
        void run();
        static void RecvMsg(int coon);
        static void HandleRequest(int conn, string str, tuple<bool, string, string, int>&info);
        //待补充
};

#endif