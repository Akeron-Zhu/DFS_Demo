#include <errno.h>  
#include <sys/types.h>  
#include <sys/socket.h>  
#include <netinet/in.h>  
#include <unistd.h>
#include <arpa/inet.h>
#include <bits/stdc++.h>

using namespace std;
#define BUFFER_SIZE 4096  
//socket发送数据
void data_send(int sockfd,char sendline[],int len ){
    if( send(sockfd, sendline, len, 0) < 0)
    {
        printf("send msg error: %s(errno: %d)\n", strerror(errno), errno);
        exit(0);
    }
}
//定义了三个端品，在本地设立了三个进程，模仿三个服务器
int PORT[3]={10000,10001,10002};

int main(int argc, char** argv)  
{  
    //如果输入的参数不对，则提示使用方法 
    if( argc != 3){
        printf("usage: ./client_for_send <ipaddress> <filename>\n");
        exit(0);
    }
    //定义Socket的一些基本变量，
    int    sockfd,rec_len;
    struct sockaddr_in    servaddr;
    //定义客户端这边接收的buffer的大小。
    char    buf[BUFFER_SIZE];
    //用来得到当前的时间，精确到秒，使服务器端存储的名字无重复
    time_t t = time(0);
    char sendName[128];
    bzero(sendName,sizeof(sendName));
    //得到时间，精确到秒,并将其写入sendName中。
    strftime( sendName, sizeof(sendName), "%Y%m%d%H%M%S",localtime(&t) );
    //将原来的文件名加入到sendName中，形成最后文件名。
    int len=strlen(sendName);
    for(int i=0;i<strlen(argv[2]);i++){
        sendName[len+i]=argv[2][i];
    }
    sendName[strlen(sendName)+strlen(argv[2])]='\0';
    len=strlen(sendName);
    //以二进制读取方式打开文件
    FILE* in;
    in=fopen(argv[2],"rb");
    //如果不能打开则代表出错
    if(!in) {
        cout<<"Read file error!"<<endl;
        return 0;
    }
    //得到文件的总大小，即字节数
    fseek(in,0,2);     //定位到文件末
    long long size = ftell(in);
    //因为要将文件分为三份，所以在这里除3
    long long t_size=size/3;
    //跳回文件最开始的地方 
    fseek(in,0,0);
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;

    

    for(int i=0;i<3;i++){
        bzero(buf,sizeof(buf));//若不清零，则下次传送名字出错。
        //得到sockfd用来通信
        if( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
            printf("create socket error: %s(errno: %d)\n", strerror(errno),errno);
            exit(0);
        }
        //选取不同的端口，因为这里是本地模拟，如果是在不同的服务器，则要将端口与IP都进行更换。
        //若同时需要更换IP，只需更换argv[1]为其它IP即可
        servaddr.sin_port = htons(PORT[i]);
        if(inet_pton(AF_INET, argv[1], &servaddr.sin_addr) <= 0){
            printf("inet_pton error for %s\n",argv[1]);
            exit(0);
        }
        //进行连接
        if( connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0){
            printf("connect error: %s(errno: %d)\n",strerror(errno),errno);
            exit(0);
        }
        //因为要将一个文件分为三部分发送到不同的服务器端，所以在文件名里加上是第几部分
        sendName[len]=i+'0'; //变为char
        sendName[len+1]='\0';
        //将文件名先发送给服务器
        strncpy(buf,sendName,strlen(sendName));
        printf("send msg to server: \n");
        data_send(sockfd,buf,BUFFER_SIZE);
        //发送数据，read_length表示每次读到多少，count用来计数判断是否超过了文件三分之一
        int read_length=0;
        long long count=0;
        //读取文件并发送
        while(read_length=fread(buf,sizeof(char),BUFFER_SIZE,in)){
            //printf("read_lenght=%d\n",read_length);
            data_send(sockfd,buf,read_length);
            count+=read_length;
            if(count>t_size||count==size) break;
        }

        printf("Part %d transfer Finished!\n",i);
        close(sockfd);

    }
    //显示发送的文件名
    sendName[len]='\0';
    printf("%s\n",sendName );
    fclose(in);
    return 0;
}  
