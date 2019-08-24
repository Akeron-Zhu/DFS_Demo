    /* File Name: client.c */  

#include<stdio.h>  
#include<stdlib.h>  
#include<string.h>  
#include<errno.h>  
#include<sys/types.h>  
#include<sys/socket.h>  
#include<netinet/in.h>  
#include<unistd.h>
#include <arpa/inet.h>
#include<bits/stdc++.h>

using namespace std;
#define BUFFER_SIZE 4096  
#define PORT 10006
void data_send(int sockfd,char sendline[],int len ){
    if( send(sockfd, sendline, len, 0) < 0)  
        {  
            printf("send msg error: %s(errno: %d)\n", strerror(errno), errno);  
            exit(0);  
        } 
}

int main(int argc, char** argv)  
{  
    if( argc != 4){  
        printf("usage: ./client <ipaddress> <filename> <port>\n");  
        exit(0);  
    }  
    int    sockfd, n,rec_len;    
    char    buf[BUFFER_SIZE];  
    struct sockaddr_in    servaddr;  
    FILE* in;
    in=fopen(argv[2],"rb");
    if(!in) {
        cout<<"Read file error!"<<endl;
        return 0;
    }
    memset(&servaddr, 0, sizeof(servaddr));  
    servaddr.sin_family = AF_INET;  

    bzero(buf,sizeof(buf));//若不清零，则下次传保名字出错。
    if( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){  
        printf("create socket error: %s(errno: %d)\n", strerror(errno),errno);  
        exit(0);  
    } 
    int port=atoi(argv[3]);
    servaddr.sin_port = htons(port); 
    if(inet_pton(AF_INET, argv[1], &servaddr.sin_addr) <= 0){  
        printf("inet_pton error for %s\n",argv[1]);  
        exit(0);  
    }  

    if( connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0){  
        printf("connect error: %s(errno: %d)\n",strerror(errno),errno);  
        exit(0);  
    }  
    strncpy(buf,argv[2],strlen(argv[2]));
    printf("send msg to server: \n"); 
    data_send(sockfd,buf,BUFFER_SIZE);
    //fgets(sendline, 40, stdin);
    int read_length=0;
    while(read_length=fread(buf,sizeof(char),BUFFER_SIZE,in)){
        //printf("read_lenght=%d\n",read_length);
        data_send(sockfd,buf,read_length);
        bzero(buf,sizeof(buf));
    }
    
     printf("Transfer %s Finished!\n",argv[2]);
    close(sockfd);
    fclose(in);
     return 0;
}  