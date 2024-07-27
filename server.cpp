#include "server.h"

void server::RecvMsg(int conn){
    //负责接收的缓冲区
    char buffer[1024];
    //不断接收数据
    while(1){
        memset(buffer, 0, sizeof(buffer));
        int len = recv(conn, buffer, sizeof(buffer), 0);
        //客户端发送exit或者异常节结束时，退出循环
        if(strcmp(buffer, "exit") == 0 || len <= 0){
            close(conn);
            sock_arr[conn] = false;
            break;
        }
        cout << "收到套接字描述符为" << conn << "发来的信息：" << buffer << endl;
        string str(buffer);
        HandleRequest(conn ,str);
    }
}

void server::HandleRequest(int conn, string str){
    char buffer[1024];
    string name, pwd;
    
    //连接MYAQL数据库,并初始化
    MYSQL *con = mysql_init(NULL);
    mysql_real_connect(con, "127.0.0.1", "root", "ChatProject",
     0, NULL, CLIENT_MULTI_STATEMENTS);

    
    
}

/* int main()
{
    int server_sockfd = socket(AF_INET,SOCK_STREAM, 0);

    //定义sockaddr_in
    struct sockaddr_in server_sockaddr;
    server_sockaddr.sin_family = AF_INET;//TCP/IP协议族
    server_sockaddr.sin_port = htons(8023);//端口号
    server_sockaddr.sin_addr.s_addr = INADDR_ANY;

    //bind，成功返回0，出错返回-1
    if(bind(server_sockfd,(struct sockaddr *)&server_sockaddr,sizeof(server_sockaddr)) == -1)
    {
        perror("bind");
        exit(1);
    }

    //listen，成功返回0，出错返回-1
    if(listen(server_sockfd,20) == -1)
    {
        perror("listen");
        exit(1);
    }

    //客户端套接字
    struct sockaddr_in client_addr;
    socklen_t length = sizeof(client_addr);

    //成功返回非负描述字，出错返回-1
    int conn = accept(server_sockfd, (struct sockaddr*)&client_addr, &length);
    if(conn < 0)
    {
        perror("connect");
        exit(1);
    }
    cout << "客户端成功连接\n";

    //接收缓冲区
    char buffer[1000];

    //不断接收数据
    while(1)
    {
        memset(buffer,0,sizeof(buffer));
        int len = recv(conn, buffer, sizeof(buffer),0);
        //客户端发送exit或者异常结束时，退出
        if(strcmp(buffer,"exit")==0 || len<=0)
            break;
        cout << "收到客户端信息：" << buffer << endl;
    }
    close(conn);
    close(server_sockfd);
    return 0;
} */