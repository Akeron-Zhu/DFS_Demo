    /* File Name: server.c */  
#include <stdio.h>  
#include <stdlib.h>  
#include <string.h>  
#include <errno.h>  
#include <sys/types.h>  
#include <sys/socket.h>  
#include <netinet/in.h>  
#include <unistd.h>
#include <bits/stdc++.h>
#define BUFFER_SIZE 4096  

int main(int argc, char** argv)  
{  
    int    socket_fd, connect_fd;
    char fileName[128];
    if( argc != 2){  
        printf("usage: ./server_get_server <port>\n用来得到另一个服务器传送的数\n");  
        exit(0);  
    }  
    int  port=atoi(argv[1]);
    struct sockaddr_in     servaddr;
    memset(&servaddr, 0, sizeof(servaddr));  
    servaddr.sin_family = AF_INET;  
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);//IP地址设置成INADDR_ANY,让系统自动获取本机的IP地址。  
    servaddr.sin_port = htons(port);//设置的端口为DEFAULT_PORT  
    char    buff[BUFFER_SIZE]; 
        //初始化Socket  
    if( (socket_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1 ){  
        printf("create socket error: %s(errno: %d)\n",strerror(errno),errno);  
        exit(0);  
    }  
        //初始化  
    
    //将本地地址绑定到所创建的套接字上  
    if( bind(socket_fd, (struct sockaddr*)&servaddr, sizeof(servaddr)) == -1){  
        printf("bind socket error: %s(errno: %d)\n",strerror(errno),errno);  
        exit(0);  
    }  
    //开始监听是否有客户端连接  
    if( listen(socket_fd, 10) == -1){  
        printf("listen socket error: %s(errno: %d)\n",strerror(errno),errno);  
        exit(0);  
    }   


    while(1){  
        printf("======waiting for client's request======\n"); 
        //阻塞直到有客户端连接，不然多浪费CPU资源。  
        if( (connect_fd = accept(socket_fd, (struct sockaddr*)NULL, NULL)) == -1){  
            printf("accept socket error: %s(errno: %d)",strerror(errno),errno);  
            continue;  
        }  
        else{
            printf("Connect success\n");
        }
        bzero(buff,sizeof(buff));
        bzero(fileName,sizeof(fileName));
        printf("fileName=%s\n",fileName );
        int recv_len=recv(connect_fd,buff,BUFFER_SIZE,0);
        if(recv_len<0) printf("Error occur!\n");
        strncpy(fileName, buff,  strlen(buff));
        fileName[strlen(buff)]='\0'; 
    		printf("%s\n",fileName );

        FILE* out=fopen(fileName,"wb");
        int count=0,cnt=0,length=0;
        while(length = recv(connect_fd, buff, BUFFER_SIZE, 0)){
            if (length < 0)  
            {  
                printf("Recieve Data From Server  Failed!\n");  
                break;  
            }  
            count+=length;
            cnt++;
            //printf("length=%d cnt=%d\n",length,cnt);
            int write_len=fwrite(buff,sizeof(char),length,out);
            if(write_len<length){
                printf("File Write Failed!\n");  
                break; 
            }
            bzero(buff,BUFFER_SIZE);
        }
        fclose(out);
        
    }  
    close(socket_fd);  
}  