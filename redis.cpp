/*
 * redis.cpp
 *
 *  Created on: Nov 18, 2019
 *      Author: wenhan
 */

#include <iostream>
#include <string>
#include "redis.h"
#include "logger/qklogger.h"
#include "config.h"

using namespace std;
using namespace GLOBAL;

extern ConfigDB cdb;

RedisMg::RedisMg(std::string ip)
        :
        _ip(ip)
{
        _connect = nullptr;
        _reply = nullptr;

        if (this->connect())
        {
                this->setDB(atoi(cdb.get("redis_index").c_str()));
        }
}

RedisMg::~RedisMg()
{
        freeReplyObject(_reply);
        redisFree(_connect);
        _connect = nullptr;
        _reply = nullptr;
}

Bool RedisMg::connect()
{
        _connect = redisConnect(_ip.c_str(), atoi(cdb.get("redis_port").c_str()));
        if (!_connect || _connect->err)
        {
                QkWarn("connect redis failed: " + string(_connect->errstr));
                return FALSE;
        }
        QkInfo("connect to redis success...");
        return TRUE;
}

void RedisMg::set(std::string key, std::string value)
{
        redisCommand(_connect, "SET %s %s", key.c_str(), value.c_str());
//        freeReplyObject(_reply);
}

string RedisMg::get(const std::string &key)
{
        _reply = (redisReply*) redisCommand(_connect, "GET %s", key.c_str());
        string str = "";
        cout << _reply->len << endl;
        if (_reply->len)
        {     // 查到了
                str = _reply->str;
        }
        freeReplyObject(_reply);
        return str;
}

void RedisMg::hset(const std::string &hash_k, const std::string &hash_f, void *hash_v, size_t size)
{
        _reply = (redisReply*) redisCommand(_connect, "hset %s %s %b", hash_k.c_str(), hash_f.c_str(), hash_v, size);
        freeReplyObject(this->_reply);
}

void* RedisMg::hget(const std::string &hash_k, const std::string &hash_f)
{
        _reply = (redisReply*) redisCommand(_connect, "hget %s %s", hash_k.c_str(), hash_f.c_str());
        void *str = (void*) _reply->str;
        freeReplyObject(this->_reply);
        return str;
}

RedisMg::ARRAYVEC RedisMg::lrange(const std::string &lkey, const int &start, const int &end)
{
        _reply = (redisReply*) redisCommand(_connect, "lrange %s %d %d", lkey.c_str(), start, end);
        ARRAYVEC v;
        for (int i = _reply->elements - 1; i >= 0; i--)
        {
                v.push_back(string(_reply->element[i]->str));
        }
        freeReplyObject(this->_reply);
        return v;
}

vector<string> RedisMg::fuzzykeys(const string &key)
{
        vector<string> v;
        string sentence = "keys *";
        _reply = (redisReply*) redisCommand(this->_connect, sentence.c_str());
        for (size_t i = 0; i < this->_reply->elements; i++)
        {
                redisReply *childReply = this->_reply->element[i];
                v.push_back(childReply->str);
        }
        freeReplyObject(this->_reply);
        return v;
}

