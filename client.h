#ifndef CLIENT_H
#define CLIENT_H

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
using namespace std;

class client{
    private:
        int server_port;
        string server_ip;
        int sock;
    public:
        client(int port, string ip);
        ~client();
        void run();
        void HandleClient(int conn);
        //待补充

};

#endif