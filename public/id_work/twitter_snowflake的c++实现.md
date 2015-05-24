#twitter snowflake的c++实现
**不是线程安全的版本，但是很容易改成线程安全的**

#id生成的规则
///////////////////////////////////////////////  
| 42b 0.1 ms ts | 5b ip | 5b pid | 12b seq |  
| 42b 0.1 ms ts | 10b worker id  | 12b seq |  

///////////////////////////////////////////////

高42比特的时间戳，精度是0.1毫秒  
接下来10比特的worker id,默认是5比特的hash(ip)和5比特pid  
最后是12比特的序号

worker id可以用构造函数idworker或者set_worker_id指定

#用法
    idworker worker;
    uint64_t id = worker.next_id();
    //do something
    
    worker.set_worker_id(distributed_worker_id);
    id = worker.next_id();
    //do something

    