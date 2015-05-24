#include "id_worker.h"


//hash(ip)                                                                                                                                                                                                                                                                      
inline unsigned int BKDRHash(char *str)
{
    unsigned int seed = 131; // 31 131 1313 13131 131313 etc..
    unsigned int hash = 0;

    while (*str)
    {
	hash = hash * seed + (*str++);
    }

    return (hash & 0x7FFFFFFF);
}

inline int getip(char *ip,int iplen)
{
    struct ifaddrs * ifAddrStruct=NULL;
    void * tmpAddrPtr=NULL;

    getifaddrs(&ifAddrStruct);

    while (ifAddrStruct!=NULL) 
    {
        if (ifAddrStruct->ifa_addr->sa_family==AF_INET && strcmp(ifAddrStruct->ifa_name,"eth1") )
        {
            tmpAddrPtr=&((struct sockaddr_in *)ifAddrStruct->ifa_addr)->sin_addr;
            //char addressBuffer[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, tmpAddrPtr, ip, iplen);
        } 
        ifAddrStruct=ifAddrStruct->ifa_next;
    }
    return 0;
}


idworker::idworker()
{
	char ip[100];
    getip(ip,sizeof(ip));
    uint64_t haship  = BKDRHash(ip) % 31; 
    uint64_t pd = getpid();
    uint64_t threadid = pthread_self();
	uint64_t machine_id = ( pd << 2 | threadid ) % 31;

	worker_id = ((haship << (workerIdBits - 5)) |  machine_id) & (maxWorkerId) ;
	sequence=0;
	lastTimestamp=0;

	//printf("hash(ip)=%llu, machine_id=%llu,worker_id=%llu\n",haship,machine_id,worker_id);
}

//时间回滚了 return 0
//否则，return other
uint64_t idworker::next_id()
{
    uint64_t timestamp = timeGen();
    if (lastTimestamp == timestamp)
    {
        
        sequence = (sequence + 1) & sequenceMask;
        if (sequence == 0)
        {
            //printf("########### %llu\n" , sequenceMask);
            timestamp = tilNextMillis(lastTimestamp);
        }
    }
    else
    {
        sequence = 0;
    }
    if (timestamp < lastTimestamp)
    {
        return 0;
    }

    lastTimestamp = timestamp;
    uint64_t nextId = ((timestamp - twepoch << timestampLeftShift))
            | (worker_id << workerIdShift) | (sequence);

    //printf( "timestamp:%llu ,timestampLeftShift:%llu,nextId:%llu,workerId:%llu,sequence:%llu\n"
      //   , timestamp,timestampLeftShift,nextId,worker_id,sequence);
    return nextId;
}




