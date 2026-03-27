#ifndef __PARAM_MODULE_H__
#define __PARAM_MODULE_H__

#include <queue>
#include <iostream>
#include "macros.h"
#include "websocketclient.h"

using namespace std;


class avos_param
{
public:
    avos_param() {}
    ~avos_param()
    {
        stop=true;
    }

    void Init(const string &module_name)
    {
        node_name=module_name;
        SHMParamApi::Instance()->AddModuleParam(module_name);

        web_loop = std::thread([&]()
            {
            CAvosParamWebSocketClient websocket_(node_name);
            while (!stop)
            {
                websocket_.wait_n_us = wait_n_us;
                websocket_.Init();
                websocket_.Close();
                sleep(3);
            } });
        web_loop.detach();
    }

    string node_name;
    std::thread web_loop;
    bool stop=false;
    int wait_n_us = 1000*1000*3;
};

class avos_param_interface
{
public:
    avos_param_interface()
    {
        shm_ptr = SHMParamApi::Instance();
        param_cmd_que = CParamGlobalQue::Instance();
    }

    template <typename T>
    string vecPrintf(const vector<T> & vec_data)
    {
        string ret_string;
        ostringstream streamStr1;  //构造一个输出字符串流，流内容为空
        string str_dou=",";
        for (int var = 0; var < vec_data.size(); ++var)
        {
            streamStr1 << vec_data[var];
            if(var<vec_data.size()-1)
            {
                streamStr1 << str_dou;
            }
        }
        ret_string = streamStr1.str();
        return ret_string;
    }

    bool pushCmd(string cmd)
    {
        param_cmd_que->pushLogQue(cmd);
        param_cmd_que->param_sem.Post();
        return true;
    }

    void Print()
    {
        string cmd = string(PARAM_CMD)+string(PARAM_LIST_CMD);
        pushCmd(cmd);
    }

    bool SetDoubleModuleParam( const std::string &key, const double value,const std::string module="global")
    {
        string cmd = string(PARAM_CMD)+string(PARAM_SET_CMD)+module+":"+key+":"+"-d:"+to_string(value);
        if(shm_ptr->FindKeyInThisDataPool(module,key))
        {
            shm_ptr->SetDoubleModuleParam(key,value,module);
        }
        return pushCmd(cmd);
    }
    bool QueryDoubleModuleParam( const std::string &key, double &value,const std::string module="global")
    {
        string cmd = string(PARAM_CMD)+string(PARAM_GET_CMD)+module+":"+key+":"+"-d";
        if(shm_ptr->FindKeyInThisDataPool(module,key))
        {
            return shm_ptr->QueryDoubleModuleParam(key,value,module);
        }
        pushCmd(cmd);
        return false;
    }

    bool SetIntModuleParam( const std::string &key, const int value,const std::string module="global")
    {
        string cmd = string(PARAM_CMD)+string(PARAM_SET_CMD)+module+":"+key+":"+"-i:"+to_string(value);
        if(shm_ptr->FindKeyInThisDataPool(module,key))
        {
            shm_ptr->SetIntModuleParam(key,value,module);
        }
        return pushCmd(cmd);
    }
    bool QueryIntModuleParam( const std::string &key, int &value,const std::string module="global")
    {
        string cmd = string(PARAM_CMD)+string(PARAM_GET_CMD)+module+":"+key+":"+"-i";
        if(shm_ptr->FindKeyInThisDataPool(module,key))
        {
           return shm_ptr->QueryIntModuleParam(key,value,module);
        }
        pushCmd(cmd);
        return false;
    }

    bool SetStringModuleParam( const std::string &key, const std::string& value,const std::string module="global")
    {
        string cmd = string(PARAM_CMD)+string(PARAM_SET_CMD)+module+":"+key+":"+"-s:"+value;
        if(shm_ptr->FindKeyInThisDataPool(module,key))
        {
            shm_ptr->SetStringModuleParam(key,value,module);
        }
        return pushCmd(cmd);
    }
    bool QueryStringModuleParam( const std::string &key, string &value,const std::string module="global")
    {
        string cmd = string(PARAM_CMD)+string(PARAM_GET_CMD)+module+":"+key+":"+"-s";
        if(shm_ptr->FindKeyInThisDataPool(module,key))
        {
            return shm_ptr->QueryStringModuleParam(key,value,module);
        }
        pushCmd(cmd);
        return false;
    }

    bool SetDoubleModuleParam( const std::string &key, const vector<double> &value,const std::string module="global")
    {
        string cmd = string(PARAM_CMD)+string(PARAM_SET_CMD)+module+":"+key+":"+"-d:"+vecPrintf(value);
        if(shm_ptr->FindKeyInThisDataPool(module,key))
        {
            shm_ptr->SetDoubleModuleParam(key,value,module);
        }
        return pushCmd(cmd);
    }
    bool QueryDoubleModuleParam( const std::string &key, vector<double> &value,const std::string module="global")
    {
        string cmd = string(PARAM_CMD)+string(PARAM_GET_CMD)+module+":"+key+":"+"-d";
        if(shm_ptr->FindKeyInThisDataPool(module,key))
        {
            return shm_ptr->QueryDoubleModuleParam(key,value,module);
        }
        pushCmd(cmd);
        return false;
    }

    bool SetIntModuleParam( const std::string &key, const vector<int> &value,const std::string module="global")
    {
        string cmd = string(PARAM_CMD)+string(PARAM_SET_CMD)+module+":"+key+":"+"-i:"+vecPrintf(value);
        if(shm_ptr->FindKeyInThisDataPool(module,key))
        {
            shm_ptr->SetIntModuleParam(key,value,module);
        }
        return pushCmd(cmd);
    }
    bool QueryIntModuleParam( const std::string &key, vector<int> &value,const std::string module="global")
    {
        string cmd = string(PARAM_CMD)+string(PARAM_GET_CMD)+module+":"+key+":"+"-i";
        if(shm_ptr->FindKeyInThisDataPool(module,key))
        {
            return shm_ptr->QueryIntModuleParam(key,value,module);
        }
        pushCmd(cmd);
        return false;
    }

    bool SetStringModuleParam( const std::string &key, const vector<string> &value,const std::string module="global")
    {
        string cmd = string(PARAM_CMD)+string(PARAM_SET_CMD)+module+":"+key+":"+"-s:"+vecPrintf(value);
        if(shm_ptr->FindKeyInThisDataPool(module,key))
        {
            shm_ptr->SetStringModuleParam(key,value,module);
        }
        return pushCmd(cmd);
    }
    bool QueryStringModuleParam( const std::string &key, vector<string> &value,const std::string module="global")
    {
        string cmd = string(PARAM_CMD)+string(PARAM_GET_CMD)+module+":"+key+":"+"-s";
        if(shm_ptr->FindKeyInThisDataPool(module,key))
        {
            return shm_ptr->QueryStringModuleParam(key,value,module);
        }
        pushCmd(cmd);
        return false;
    }

    SHMParamApi* shm_ptr;
    CParamGlobalQue* param_cmd_que;
    BASE_DECLARE_SINGLETON(avos_param_interface)
};

#endif
