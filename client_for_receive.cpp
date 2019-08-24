#include <netinet/in.h>                         // for sockaddr_in
#include <sys/types.h>                          // for socket
#include <sys/socket.h> 
#include <arpa/inet.h>       
#include <unistd.h>
#include <bits/stdc++.h>               // for bzero

using namespace std;

#define BUFFER_SIZE	1024
//定义本机三个端口，模仿三个服务器
int PORT[3]={10003,10004,10005};
//定义是否读了文件的第i部分，读成功后设置read_part[i]变为true
bool read_part[3]={false,false,false};
//返回读到第几部分了
int readNum(){
    int i=0;
    while(read_part[i]) i++;
    return i;
}

int main(int argc, char **argv)
{
	//显示使用方法 
    if (argc != 3)
    {
        printf("Usage: ./%s <ServerIPAddress> <filename>\n", argv[0]);
        exit(1);
    }
    //定义接收端buffer大小
    char buffer[BUFFER_SIZE];
    // 设置一个socket地址结构client_addr, 代表客户机的internet地址和端口
    struct sockaddr_in client_addr;
    bzero(&client_addr, sizeof(client_addr));
    client_addr.sin_family = AF_INET; // internet协议族
    client_addr.sin_addr.s_addr = htons(INADDR_ANY); // INADDR_ANY表示自动获取本机地址
    client_addr.sin_port = htons(0); // auto allocated, 让系统自动分配一个空闲端口
    //得到要读取的文件名称，创建文件并写入
    string file_name=argv[2];
    FILE *fp = fopen(argv[2], "wb");
    if (fp == NULL)
    {
        cout<<"Open file error!"<<endl;
        exit(1);
    }
    int count=0;
    //得到当前读取文件的部分
    int read_machine=readNum();
    while(readNum()!=3){
    		count++;//统计进行了多少次读取了，防止根本无法读取的情况进行无休止的循环。
        // 创建用于internet的流协议(TCP)类型socket，用client_socket代表客户端socket
        int client_socket = socket(AF_INET, SOCK_STREAM, 0);
        if (client_socket < 0)
        {
            printf("Create Socket Failed!\n");
            exit(1);
        }

        // 把客户端的socket和客户端的socket地址结构绑定
        if (bind(client_socket, (struct sockaddr*)&client_addr, sizeof(client_addr)))
        {
            printf("Client Bind Port Failed!\n");
            exit(1);
        }

        // 设置一个socket地址结构server_addr,代表服务器的internet地址和端口
        struct sockaddr_in  server_addr;
        bzero(&server_addr, sizeof(server_addr));
        server_addr.sin_family = AF_INET;

        // 服务器的IP地址来自程序的参数
        if (inet_aton(argv[1], &server_addr.sin_addr) == 0)
        {
            printf("Server IP Address Error!\n");
            exit(1);
        }
        //在这里是换取不同的端口，模拟读取不同的服务器
        server_addr.sin_port = htons(PORT[read_machine]);
        socklen_t server_addr_length = sizeof(server_addr);

        // 向服务器发起连接请求，连接成功后client_socket代表客户端和服务器端的一个socket连接

        if (connect(client_socket, (struct sockaddr*)&server_addr, server_addr_length) < 0)
        {
            printf("Can Not Connect To %s!\n", argv[1]);
            read_machine= (read_machine+2)%3;
            //如果已经100次了还没读取完，证明无法读取，则跳出。
             if(count==100){
		        	printf("Error, Can't Read!\n");
		        	break;
		    }
            continue;//如果读取一个服务器失败了，就不继续执行下面代码,进行下次循环
        }

        //清空buffer
        bzero(buffer, sizeof(buffer));
        //得到要读取文件的名称，readNum()表示了要读的第几部分
        string part=file_name+char(readNum()+'0');
        strncpy(buffer, part.c_str(), part.size() > BUFFER_SIZE ? BUFFER_SIZE : part.size());
        // 向服务器发送buffer中的数据，此时buffer中存放的是客户端需要接收的文件的名字
        send(client_socket, buffer, BUFFER_SIZE, 0);



        // 从服务器端接收数据到buffer中
        bzero(buffer, sizeof(buffer));
        int length = 0;
        while(length = recv(client_socket, buffer, BUFFER_SIZE, 0))
        {
            if (length < 0)
            {
                printf("Recieve Data From Server %s Failed!\n", argv[1]);
                break;
            }

            int write_length = fwrite(buffer, sizeof(char), length, fp);
            if (write_length < length)
            {
                printf("File Write Failed!\n");
                break;
            }
            bzero(buffer, BUFFER_SIZE);
        }

        printf("Recieve File:%s  From Server[%s] Finished!\n", part.c_str(),argv[1]);
        //读取到了就进行更新read_part
        read_part[readNum()]=true;
        read_machine=readNum();
        // 传输完毕，关闭socket
        close(client_socket);
       
    }

    fclose(fp);
    return 0;

}

