#include<time.h>
#include<sys/time.h>
#include<unistd.h>
#include<stdio.h>
#include<stdlib.h>
#include <sys/types.h>
#include <ifaddrs.h>
#include <netinet/in.h> 
#include <string.h> 
#include <arpa/inet.h>


//ip地址hash
unsigned int BKDRHash(char *str)
{
    unsigned int seed = 131; // 31 131 1313 13131 131313 etc..
    unsigned int hash = 0;

    while (*str)
    {
	hash = hash * seed + (*str++);
    }

    return (hash & 0x7FFFFFFF);
}

//获取ip地址
int getip(char *ip,int iplen)
{
    struct ifaddrs * ifAddrStruct=NULL;
    void * tmpAddrPtr=NULL;

    getifaddrs(&ifAddrStruct);

    while (ifAddrStruct!=NULL) 
    {
        if (ifAddrStruct->ifa_addr->sa_family==AF_INET && strcmp(ifAddrStruct->ifa_name,"eth1") )
        {
            tmpAddrPtr=&((struct sockaddr_in *)ifAddrStruct->ifa_addr)->sin_addr;
            char addressBuffer[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, tmpAddrPtr, ip, iplen);
        } 
        ifAddrStruct=ifAddrStruct->ifa_next;
    }
    return 0;
}



int main()
{
    int i = 0;
    int status;
    char ip[100];
    getip(ip,sizeof(ip));
    unsigned long haship  = BKDRHash(ip)%31; 

    pid_t pid;
    for(; i< 8; i++)
    {
	pid = fork();
	if(pid>0)
    {
        FILE *fp;
        unsigned long  pd=getpid();
        pd = pd % 31;
        char path[100];
        snprintf(path, sizeof(path),"./log.%d",pd);
        fp = fopen(path,"w+");
        if(fp == NULL)
        {
            printf("open file failed\n");
            exit(1);
        }
        static short sequence=0;
        static unsigned long lasttime=0;
        int j=0;
        for(; j<1000000; j++)
        {
            struct timeval tv;
            gettimeofday(&tv, NULL);

            unsigned long time =(unsigned long) tv.tv_sec*1000*10+tv.tv_usec/100;

            if(time < lasttime && lasttime != 0)
            {
                printf("时间回滚了!\n");
                exit(1);
            }
           
            if(lasttime == time)
            {
                sequence = sequence + 1;
                if(!(sequence&0x0fff))
                {
                    sequence = 0; //回绕置为零

                    while(lasttime == time)
                    {
                        gettimeofday(&tv, NULL);
                        time =(unsigned long) tv.tv_sec*1000*10+tv.tv_usec/100;
                    }
                }

            }

            lasttime = time;

            time = time -24*3600*365*(unsigned long)10000*44;
           // time = sequence|pd<<12|haship<<17|time<<22;

            fprintf(fp,"%ld %d\n",time,sequence);

        }

        exit(1);
    }
    }


    while((pid = wait(status, NULL)) > 0)
    {
        printf("wait pid = %d \n",pid);
    }

    return 0;

}
