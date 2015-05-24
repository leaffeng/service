#include "id_worker.h"
//g++ -o id_worker test_id.cpp id_worker.cpp -lpthread

//���Լ���ʱ��ɣ���Ҫʱ�䶼�õ�д�ļ�����
/**
* ���ܣ�4��
* thread_nr  num       time
* 16         100000000 1.2s
* 8          100000000 610ms
* 1          100000000 267ms
**/

#include <fstream>
using namespace std;

//fprintf��Ч��Ҫ����ofstream��"\n"Ҫ����endl

static int gen_num ;

void* get_id(void* p)
{
    FILE * fp;
    char path[100];
    int pd = *((int*)p);
    idworker id(pd) ;
    int ret = 0;
    snprintf(path, sizeof(path), "./log.%d", pd);
    puts(path);
    //ofstream out(path);
    fp = fopen(path,"w+");
    //if (!out)
    if(!fp)
    {
        ret = 10;
        printf("opne fail\n");
        pthread_exit(&ret);
    }
    for (int i = 0; i < gen_num; i++)
    {
        //fprintf(fp, "%llu\n", id.next_id());
        //out << id.next_id() << endl;
    }

    fclose(fp);
    //out.close();
    pthread_exit(&ret);

}


int main(int argc,char** argv)
{
    puts("usage: main thread_num[10] , gen_num[1000000]");
    int idx[16];
    pthread_t tid[16];
    int tn = 10;
    gen_num = 1000000;
    if(argc > 1) tn = atoi(argv[1]);
    if(tn >=16 || tn <1) tn= 16;
    if(argc > 2) gen_num = atoi(argv[2]);

    timeval t;
    gettimeofday(&t,0);
    
    for (int i = 0; i < tn; i++)
    {
        idx[i] = i;
        if (pthread_create(&tid[i], 0, get_id, (void*)(&idx[i])))
        {
            printf("create fail \n");
            return -1;
        }
    }
    for (int i = 0; i < tn; i++)
    {
        pthread_join(tid[i], NULL);
    }

    timeval e;
    gettimeofday(&e,0);

    struct timeval res;
    timersub(&e, &t, &res);

    printf("time: %lld s, %lld ms\n", res.tv_sec, (res.tv_usec)/1000);

    return 0;
}
