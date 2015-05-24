#ifndef __ID_WORKER_HPP__
#define __ID_WORKER_HPP__

#include <time.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <ifaddrs.h>
#include <netinet/in.h> 
#include <string.h> 
#include <arpa/inet.h>
#include <pthread.h>

/** 
* fengye
* 2015-1-6
* twitter snowflake
**/

///////////////////////////////////////////////
// | 42b 0.1 ms ts | 5b ip | 5b pid | 12b seq |
// | 42b 0.1 ms ts | 10b worker id  | 12b seq |

///////////////////////////////////////////////
// 每毫秒可以生成40960个不重复的id


//非线程安全，每个线程维护一个对象，很容易就可以实现线程安全的版本
class idworker
{
public:
	//用户可以自己设计一个10bit的work id
	idworker(uint64_t workerid)
	{
		worker_id = workerid & maxWorkerId;
		sequence = lastTimestamp = 0;
	}

	
	idworker();

	void set_worker_id(uint64_t workerid)
	{
		worker_id = workerid & maxWorkerId;
	}

	uint64_t next_id();


private:
	uint64_t worker_id;
	volatile uint64_t sequence;
	volatile uint64_t lastTimestamp ;


private:
	const static uint64_t twepoch = 1361753741828L;///< 2013-12-19 11:09:25
	const static uint64_t workerIdBits = 10L;
    const static uint64_t maxWorkerId = -1L ^ -1L << workerIdBits;
    const static uint64_t sequenceBits = 12L;

    const static uint64_t workerIdShift = sequenceBits;
    const static uint64_t timestampLeftShift = sequenceBits + workerIdBits;
    const static uint64_t sequenceMask = -1L ^ -1L << sequenceBits;



private:
	uint64_t tilNextMillis(uint64_t lastTimestamp)
	{
		uint64_t timestamp = timeGen();
        while (timestamp <= lastTimestamp)
        {
            timestamp = timeGen();
        }
        return timestamp;
	}

	uint64_t timeGen()//0.1 ms
	{
		struct timeval tv;
		gettimeofday(&tv, NULL);

		return (uint64_t) tv.tv_sec*1000*10+tv.tv_usec/100;
	}

	idworker(const idworker & );

	idworker& operator=( idworker & );
	
};




#endif


