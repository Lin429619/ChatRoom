#include "client.h"

//构造函数
client::client(int port,string ip):server_port(port),server_ip(ip){}

//析构函数
client::~client(){
    close(sock);
}

void client::run(){
    sock = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(8023);  //服务器端口
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");  //服务器ip

    //连接服务器,成功返回0，错误返回-1
    if(connect(sock, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0){
        perror("connect");
        exit(1);
    };
    cout << "连接服务器成功\n";

    HandleClient(sock);
    return ;
}

void client::SendMsg(int conn) {
    while(1) {
        string str;
        cin >> str;
        //发送消息
        str = "content:" + str;
        int ret = send(conn, str.c_str(), str.length(), 0); //发送
        //输入exit或者对端关闭时结束
        if(str == "content:exit" || ret <= 0) 
            break;
    }
}

 void client::RecvMsg(int conn){
    //接收缓冲区
    char buffer[1024];
    //不断接收数据
    while(1) {
        memset(buffer, 0, sizeof(buffer));
        int len = recv(conn, buffer, sizeof(buffer), 0);
        //recv返回值小于等于0,退出
        if(len <= 0) 
            break;
        cout << buffer << endl;
    }
}

void client::HandleClient(int conn){
    int choice;
    string name, pwd, pwd1;
    bool if_login = false; //记录登录是否成功
    string login_name; //记录登录成功的用户名
    //string logout_name; //记录需要注销的用户名

    cout << " ------------------\n";
    cout << "|                  |\n";
    cout << "| 请输入你要的选项:|\n";
    cout << "|    1.注册        |\n";
    cout << "|    2.登录        |\n";
    cout << "|    3.注销        |\n";
    cout << "|    0.退出        |\n";
    cout << "|                  |\n";
    cout << " ------------------ \n\n";

    while(1){
        if(if_login) 
            break; //已登录，不执行后续
        cin >> choice;
        if (choice == 0)
            break;
        //注册
        else if(choice == 1) {
            cout << "需要注册的用户名：";
            cin >> name;
            while (1)
            {
                cout << "请输入密码:";
                cin >> pwd;
                cout << "请再次确认密码:";
                cin >> pwd1;
                if(pwd == pwd1)
                    break;
                else
                    cout << "两次输入的密码不一致!请重新输入！\n";
            }
            //注册的账号和密码格式化成“name:xxxpwd:yyy”发送到服务器端，
            name = "name:" + name;
            pwd = "pwd:" + pwd;
            string str = name + pwd;
            send(conn, str.c_str(), str.length(), 0);
            cout << "注册成功！\n";
            cout << "\n继续输入你要的选项:";
        }
        //登录
        else if(choice == 2 && !if_login) {
            while(1) {
                cout << "用户名：";
                cin >> name;
                cout << "密码：";
                cin >> pwd;
                //格式化
                string str = "login:" + name;
                str += "pwd:";
                str += pwd;
                send(sock, str.c_str(), str.length(), 0); //发送登录信息
                char buffer[1024];
                memset(buffer, 0, sizeof(buffer));
                recv(sock, buffer, sizeof(buffer), 0); //接受响应
                string recv_str(buffer);
                //登录成功
                if(recv_str.substr(0,2) == "ok") {
                    if_login = true;
                    login_name = name;
                    cout << "登录成功\n\n";
                    break;
                }
                //登录失败(不匹配)
                else if(recv_str.substr(0,5) == "wrong") 
                    cout << "密码或用户名错误！\n请重新登录...\n\n";
                //用户未注册
                else if(recv_str.substr(0,5) == "error") {
                    cout << "当前用户未注册,请先按1进行注册~\n";
                    break;
                }
            }
        }
        //注销
        else if(choice == 3) {
            cout << "需要注销的用户名：" ;
            cin >> name;
            while (1) {
                cout << "请输入密码:";
                cin >> pwd;
                cout << "请再次确认密码:";
                cin >> pwd1;
                if(pwd == pwd1) {
                    //格式化
                    string str = "logout:" + name;
                    str += "pwd:";
                    str += pwd;
                    send(sock, str.c_str(), str.length(), 0); //发送注销请求
                    char buffer[1024];
                    memset(buffer, 0, sizeof(buffer));
                    recv(sock, buffer, sizeof(buffer), 0); //接收响应
                    string recv_str(buffer);
                    if (recv_str.substr(0, 2) == "ok") {
                        //if_login = false;
                        login_name = "";
                        cout << "注销成功\n\n";
                    } else {
                            cout << "注销失败，该用户不存在！\n\n";
                    }
                    break;
                }
                else {
                    cout << "两次输入的密码不一致!注销失败！\n";
                    cout << "\n继续输入你要的选项:";
                    break;
                }
            }
        }
    }
    //登录成功
    while(if_login && 1) {
        if(if_login) {
        system("clear"); //清空界面
        cout << "        欢迎进入聊天室，" << login_name << "~~" << endl;
        cout << " -------------------------------------------\n";
        cout << "|                                           |\n";
        cout << "|            请选择你要的选项：             |\n";
        cout << "|              1.发起单独聊天               |\n";
        cout << "|              2.发起群聊                   |\n";
        cout << "|              0.退出                       |\n";
        cout << "|                                           |\n";
        cout << " ------------------------------------------- \n\n";
        }
        cin >> choice;
        if(choice == 0) {
            break;
        }
        //私聊
        if (choice == 1) {
            cout << "请输入对方的用户名：";
            string target_name, content;
            cin >> target_name;
            string sendstr("targhet:" + target_name + "from:" + login_name);
            send(sock, sendstr.c_str(), sendstr.length(), 0); //向服务器发送信息
            cout << "请输入你想说的话(输入exit退出):\n";
            thread t1(client::SendMsg, conn); //创建发送线程
            thread t2(client::RecvMsg, conn); //创建接收线程
            t1.join();
            t2.join();
        }
    }
    close(sock);
}

int main(){
    client clnt (8023, "127.0.0.1");
    clnt.run();
}
















/* int main()
{
    //定义sockfd
    int sock_cli = socket(AF_INET,SOCK_STREAM, 0);

    //定义sockaddr_in
    struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;//TCP/IP协议族
    servaddr.sin_port = htons(8023);  //服务器端口
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");  //服务器ip

    //连接服务器，成功返回0，错误返回-1
    if (connect(sock_cli, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
    {
        perror("connect");
        exit(1);
    }
    cout<<"连接服务器成功！\n";

    char sendbuf[100];
    char recvbuf[100];
    while (1)
    {
        memset(sendbuf, 0, sizeof(sendbuf));
        cin>>sendbuf;
        send(sock_cli, sendbuf, strlen(sendbuf),0); //发送
        if(strcmp(sendbuf,"exit")==0)
            break;
    }
    close(sock_cli);
    return 0;
} */