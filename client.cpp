#include "client.h"

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

void client::HandleClient(int conn){
    int choice;
    string name, pwd, pwd1;
    bool if_login = false; //记录登录是否成功

    cout<<" ------------------\n";
    cout<<"|                 |\n";
    cout<<"| 请输入你要的选项: |\n";
    cout<<"|    1:登录        |\n";
    cout<<"|    2:注册        |\n";
    cout<<"|    0:退出        |\n";
    cout<<"|                  |\n";
    cout<<" ------------------ \n\n";

    while(1){
        if(if_login) 
            break;
        cin >> choice;
        if (choice == 0)
            break;
        //注册
        else if(choice == 2){
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
                    cout << "两次输入的密码不一致!\n" << "请重新注册.";
            }
            //注册的账号和密码格式化成“name:xxxpwd:yyy”发送到服务器端，
            name = "name:" + name;
            pwd = "pwd:" + pwd;
            string str = name + pwd;
            send(conn, str.c_str(), str.length(), 0);
            cout << "注册成功！\n";
            cout << "\n继续输入你要的选项:";
        }
    }
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