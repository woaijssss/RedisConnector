/*
 * redis.h
 *
 *  Created on: Nov 8, 2018
 *      Author: wenhan
 */

#ifndef INCLUDE_MEM_CACHE_REDIS_H_
#define INCLUDE_MEM_CACHE_REDIS_H_

#include <string>
#include <mutex>
#include <vector>

#include <hiredis/hiredis.h>

#include "global.h"

/* redis访问管理：
 *  主要负责读写数据，支持数据类型应包括但不限于：字符串、二进制、hash值等；
 * 若使用redis做消息队列：
 *      首先，redis设计用来做缓存的，但是由于它自身的某种特性（几个阻塞式的API），让其有能力做消息队列；
 *      另外，做消息队列的其他特性例如FIFO（先入先出）也很容易实现，只需要一个list对象从头取数据，从尾部塞数据即可；
 *      redis能做消息队列还得益于其list对象blpop brpop接口以及Pub/Sub（发布/订阅）的某些接口，它们都是阻塞版的，所以可以用来做消息队列。
 */
class RedisMg
{
public:
        typedef std::vector<std::string> ARRAYVEC;

        RedisMg(std::string ip = "127.0.0.1");

        ~RedisMg();

        /* 连接到redis server */
        GLOBAL::Bool connect();

public:
        // 不同类型的数据存储，有不同的接口
        /********************字符串***********************/
        /* 保存字符串类型K-V */
        void set(std::string key, std::string value);

        /* 获取字符串类型数据 */
        std::string get(const std::string& key);
        /***********************************************/

        /*********************hash值********************/
        /* 保存hash类型数据 */
        void hset(const std::string& hash_k, const std::string& hash_f, void* hash_v, size_t size);

        /* 获取hash类型数据 */
        void* hget(const std::string& hash_k, const std::string& hash_f);
        /***********************************************/

        /*********************list值*********************/
        /* 获取list值 */
        ARRAYVEC lrange(const std::string& lkey, const int& start, const int& end = -1);
        /***********************************************/

        /* 模糊查询key的集合 */
        std::vector<std::string> fuzzykeys(const std::string& key);
public:
        void setDB(const int& dbIndex)
        {
                redisCommand(_connect, "select %d", dbIndex);
                freeReplyObject(_reply);
        }
private:
        std::string _ip;

        redisContext* _connect; //用于连接
        redisReply* _reply; //收发数据
};

#endif /* INCLUDE_MEM_CACHE_REDIS_H_ */
