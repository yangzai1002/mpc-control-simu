#ifndef __param_GLOBALQUE_H__
#define __param_GLOBALQUE_H__

#include <queue>
#include <iostream>
#include "macros.h"
#include "Sem.h"
#include "param_protocol.h"

using namespace std;

class CParamGlobalQue
{
public:
    CParamGlobalQue() {}
    
    //req
    void  pushLogQue(string &msg)
    {
        std::unique_lock<std::mutex> locker(param_mutex);
        if (param_que.size() <= 1000)
        {
            param_que.push(msg);
        }
    }

    string popLogQue()
    {
        std::unique_lock<std::mutex> locker(param_mutex);
        string log="";
        if(param_que.size()>=1)
        {
            log = param_que.front();
            param_que.pop();
        }
        return log;
    }
    std::mutex param_mutex;
    queue<string> param_que;

    Sem param_sem;

    //rep 
    void pushResQue(string &msg)
    {
        std::unique_lock<std::mutex> locker(param_rep_mutex);
        if (param_rep_que.size() <= 1000)
        {
            param_rep_que.push(msg);
        }
    }

    string popResQue()
    {
        std::unique_lock<std::mutex> locker(param_rep_mutex);
        string log = "";
        if (param_rep_que.size() >= 1)
        {
            log = param_rep_que.front();
            param_rep_que.pop();
        }
        return log;
    }
    std::mutex param_rep_mutex;
    queue<string> param_rep_que;

    Sem param_rep_sem;

    BASE_DECLARE_SINGLETON(CParamGlobalQue)
};

#endif
