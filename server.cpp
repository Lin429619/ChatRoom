#include "server.h"

vector<bool> server::sock_arr(10000, false);
unordered_map<string, int> server::name_sock_map; //名字和套接字描述符
pthread_mutex_t server::name_sock_mutex; //互斥锁，锁住需要修改name_sock_map的临界区

//构造函数
server::server(int port,string ip):server_port(port),server_ip(ip) {
    pthread_mutex_init(&name_sock_mutex, NULL); //创建互斥锁
}

//析构函数
server::~server(){
    for(int i = 0; i < sock_arr.size(); i++){
        if(sock_arr[i])
            close(i);
    }
    close(server_sockfd);
}

void server::run(){
    //定义sockfd
    server_sockfd = socket(AF_INET, SOCK_STREAM, 0);

    //定义sockaddr_in
    struct sockaddr_in server_sockaddr;
    server_sockaddr.sin_family = AF_INET;
    server_sockaddr.sin_port = htons(server_port);
    server_sockaddr.sin_addr.s_addr = INADDR_ANY; //"127.0.0.1"

    //bind，成功返回0,错误返回-1
    if(bind(server_sockfd, (struct sockaddr *)&server_sockaddr, sizeof(server_sockaddr)) == -1){
        perror("bind");
        exit(1);
    }
    //监听
    if(listen(server_sockfd, 20) == -1){
        perror("listen");
        exit(1);
    }
    struct sockaddr_in client_addr;  //客户端套接字
    socklen_t length = sizeof(client_addr);

    while (1) {
        int conn = accept(server_sockfd, (struct sockaddr*)&client_addr, &length);
        if (conn < 0) {
            perror("connect");
            exit(1);
        }
        cout << "文件描述符为" << conn << "的客户端成功连接\n";
        sock_arr[conn] = true;
        thread t(server::RecvMsg, conn);
        t.detach();
    }
}

void server::RecvMsg(int conn){
    tuple<bool, string, string, int>info; 
    //元组类型，四个成员分别为if_login,login_name,target_name,target_conn

    get<0>(info) = false; //把if_login设置为false
    get<3>(info) = -1;    //把target_conn设置为-1

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
        HandleRequest(conn, str, info);
    }
}

void server::HandleRequest(int conn, string str, tuple<bool, string, string, int>&info){
    char buffer[1024];
    string name, pwd;
    //提取参数
    bool if_login = get<0>(info); //记录当前服务对象是否登录成功
    string login_name = get<1>(info); //记录当前服务对象的名字
    string target_name = get<2>(info); //记录目标对象的名字
    int target_conn = get<3>(info); //目标对象的套接字描述符

    //连接MYAQL数据库,并初始化
    MYSQL *con = mysql_init(NULL);
    //数据库创建失败，错误处理
    if(con == nullptr){
        fprintf(stderr, "%s\n", mysql_error(con));
        exit(EXIT_FAILURE);
    }
    mysql_real_connect(con, "127.0.0.1", "root","123456", 
    "ChatProject", 0, NULL, CLIENT_MULTI_STATEMENTS);
    
    //注册
    if(str.find("name:") != str.npos) {
        int p1 = str.find("name:");
        int p2 = str.find("pwd:");
        name = str.substr(p1 + 5, p2 - 5);
        pwd = str.substr(p2 + 4, str.length() - p2 - 4);
        string search = "INSERT INTO USER VALUES (\"";
        search += name;
        search += "\",\"";
        search += pwd;
        search += "\");";
        cout << "sql语句:" << search << endl << endl;
        mysql_query(con, search.c_str());
    }

    //登录
    else if(str.find("login:") != str.npos) {
        int p1 = str.find("login:");
        int p2 = str.find("pwd:");
        name = str.substr(p1 + 6, p2 - 6);
        pwd = str.substr(p2 + 4, str.length() - p2 - 4);
        string search = "SELECT * FROM USER WHERE NAME=\"";
        search += name;
        search += "\";";
        cout << "sql语句:" << search << endl;
        auto search_res = mysql_query(con, search.c_str());
        auto result = mysql_store_result(con);
        int col = mysql_num_fields(result); //获取列数
        int row = mysql_num_rows(result); //获取行数
        //若查询到用户名
        if(search_res == 0 && row != 0) {
            cout << "查询成功\n";
            auto info = mysql_fetch_row(result); //获取该行信息
            cout << "查询到用户名:" << info[0] << "密码:" << info[1] << endl;
            //密码正确
            if(info[1] == pwd) {
                cout << "登录密码正确\n\n";
                string str1 = "ok";
                if_login = true;
                login_name = name; //记录当前登录的用户名
                send(conn, str1.c_str(), str1.length() + 1, 0);
            }
            //密码错误
            else {
                cout << "登录密码错误\n\n";
                char str1[100] = "wrong";
                send(conn, str1, strlen(str1), 0); 
            }
        }
        //若未查询到用户名
        else {
            cout << "查询失败，未能找到该用户\n\n";
            char str1[100] = "error";
            send(conn, str1, strlen(str1), 0);
        }
    }

    //注销
    else if(str.find("logout:") != str.npos) {
        int p1 = str.find("logout:");
        int p2 = str.find("pwd:");
        name = str.substr(p1 + 7, p2 - 7);
        string dele = "DELETE FROM USER WHERE NAME=\"";
        dele += name;
        dele += "\";";
        cout << "sql语句:" << dele << endl;
        auto delete_res = mysql_query(con, dele.c_str());
        if(delete_res == 0) {
            cout << "用户" << name << "注销成功\n";
            string str1 = "ok";
            login_name = "";
            send(conn, str1.c_str(), str1.length() + 1, 0);
        }
        else {
            cout << "用户" << name << "注销失败\n";
            char str1[100] = "wrong";
            send(conn, str1, strlen(str1), 0); 
        }
    }

    //设定目标的文件描述符
    else if(str.find("target:") != str.npos) {
        int pos1 = str.find("from:");
        string target = str.substr(7, pos1 - 7);
        string from = str.substr(pos1 + 5);
        target_name = target;
        //找不到目标用户
        if(name_sock_map.find(target) == name_sock_map.end())
            cout << "源用户为" << login_name << ",目标用户" << target_name << "尚未登录，无法发起私聊\n";
        //找到目标用户
        else {
            cout << "源用户" << login_name << "向目标用户" << target_name << "发起的私聊即将建立";
            cout << ",目标用户的套接字描述符为" << name_sock_map[target] << endl;
            target_conn = name_sock_map[target];
        }
    }
    //接收到消息，转发
    
}

int main(){
    server serv(8023, "127.0.0.1");  //传入端口号和ip作为构造函数参数
    serv.run();  //启动服务
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