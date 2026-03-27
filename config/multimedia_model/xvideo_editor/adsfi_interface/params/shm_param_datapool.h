#ifndef __AVOS_SHM_PARAM_DATAPOOLH__
#define __AVOS_SHM_PARAM_DATAPOOLH__

#include <string>
#include <vector>
#include <memory>
#include <iostream>
#include <string.h>
#include <sstream>
#include <stdio.h>
#include <sys/sem.h>
#include <string.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <errno.h>

#include "shared_memory.h"
#include "macros.h"

using namespace std;

//#define DATA_POOL_INT_VEC_BUFSIZE      10*1024
//#define DATA_POOL_DOUBLE_VEC_BUFSIZE   10*1024
#define DATA_POOL_STRING_VEC_BUFSIZE   30*1024

#define DATA_POOL_STRING_BUFSIZE        256

#define DATA_POOL_INT_VEC_SIZE      256
#define DATA_POOL_DOUBLE_VEC_SIZE   256
#define DATA_POOL_STRING_VEC_SIZE   256

#define DATA_POOL_BUFSIZE (30*1024*1024)

class CSEM
{
private:
    union semun // 用于信号灯操作的共同体
    {
        int val; // 信号量的值
        struct semid_ds *buf;
        unsigned short *arry;
    };

    int sem_id;

public:
    // 创建获得、初始化信号量
    bool init(key_t key)
    {
        // 第一步：先尝试获取信号量，如果信号量不存在就创建信号量
        // int sem_id;   别在这里定义，不然这个信号量的标识是局部的。会出错
        if ((sem_id = semget(key, 1, 0640)) == -1) // 信号量不存在吗
        {
            if (errno == 2) // 信号量不存在
            {
                // 创建信号量
                if ((sem_id = semget(key, 1, 0640 | IPC_CREAT)) == -1)
                {
                    perror("init 1 semget()");
                    return false;
                }

                // 创建了信号量，对信号量进行初始化
                union semun sem_union;
                sem_union.val = 1;

                if (semctl(sem_id, 0, SETVAL, sem_union) < 0)
                {
                    perror("init semctl()");
                    return false;
                }
            }
            else // 信号灯存在了但是获取失败，记录下来
            {
                perror("init 2 semget()");
                return false;
            }
        }

        return true; // 成功创建或获取信号量，并初始化成功
    }

    // 等待操作信号量
    bool wait()
    {
        struct sembuf sem_b;
        sem_b.sem_num = 0;
        sem_b.sem_op = -1;
        sem_b.sem_flg = SEM_UNDO;

        if (semop(sem_id, &sem_b, 1) == -1)
        {
            perror("wait semop()");
            return false;
        }

        return true;
    }

    // 挂出信号量
    bool post()
    {
        struct sembuf sem_b;
        sem_b.sem_num = 0;
        sem_b.sem_op = 1;
        sem_b.sem_flg = SEM_UNDO;

        if (semop(sem_id, &sem_b, 1) == -1)
        {
            perror("post semop()");
            return false;
        }

        return true;
    }

    // 摧毁信号量
    bool destroy()
    {
        if (semctl(sem_id, 0, IPC_RMID) == -1)
        {
            perror("destroy semctl()");
            return false;
        }

        return true;
    }
};

union UDataShmAttribute
{
    struct SDataShmAttribute
    {
        unsigned int isInt:1;
        unsigned int isDouble:1;
        unsigned int isString:1;
        unsigned int isVecInt:1;
        unsigned int isVecDouble:1;
        unsigned int isVecString:1;
        unsigned int isVecInt_size:8;
        unsigned int isVecDouble_size:8;
        unsigned int isVecString_size:8;
        unsigned int isString_size:8;
        unsigned int isVecString_bufsize:16;
        unsigned int recoverd:10;
    } SData;
    int IData[2];
};

class CShmBaseFunc
{
public:
    CShmBaseFunc() {}
    vector<string> splitWithStl(const string &str,vector<string>& resVec,const string &pattern)
    {
        if ("" == str)
        {
            return resVec;
        }
        //方便截取最后一段数据
        string strs = str + pattern;

        size_t pos = strs.find(pattern);
        size_t size = strs.size();

        while (pos != std::string::npos)
        {
            std::string x = strs.substr(0,pos);
            resVec.push_back(x);
    //        printf("splitWithStl:%s\n",x.c_str());
            strs = strs.substr(pos+pattern.size(),size);
            if(strs==pattern)
                break;
            pos = strs.find(pattern);
        }
        return resVec;
    }

    vector<int> splitWithStl(const string &str,vector<int> &intVec,const string &pattern)
    {
        if ("" == str)
        {
            return intVec;
        }
        //方便截取最后一段数据
        string strs = str + pattern;

        size_t pos = strs.find(pattern);
        size_t size = strs.size();

        while (pos != std::string::npos)
        {
            std::string x = strs.substr(0,pos);
            intVec.push_back(atoi(x.c_str()));
    //        printf("splitWithStl:%s\n",x.c_str());
            strs = strs.substr(pos+pattern.size(),size);
            if(strs==pattern)
                break;
            pos = strs.find(pattern);
        }
        return intVec;
    }

    vector<double> splitWithStl(const string &str,vector<double> &doubleVec,const string &pattern)
    {
        if ("" == str)
        {
            return doubleVec;
        }
        //方便截取最后一段数据
        string strs = str + pattern;

        size_t pos = strs.find(pattern);
        size_t size = strs.size();

        while (pos != std::string::npos)
        {
            std::string x = strs.substr(0,pos);
            doubleVec.push_back(atof(x.c_str()));
    //        printf("splitWithStl:%s\n",x.c_str());
            strs = strs.substr(pos+pattern.size(),size);
            if(strs==pattern)
                break;
            pos = strs.find(pattern);
        }
        return doubleVec;
    }

    bool isContain(string str1, string str2)
    {
        if (str1.find(str2) != string::npos)
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    char* data_pool_base_addr;

    BASE_DECLARE_SINGLETON(CShmBaseFunc)
};

typedef struct SDataPoolHeader
{
#define KEY_MSG_SIZE 52
    char _key[KEY_MSG_SIZE];

    UDataShmAttribute head_attr;
//    char* data_pool_ptr=NULL;
    int data_pool_offset=0;
    int data_pool_size=0;

    void SetKey(const std::string &str) {
        memset(_key, 0, KEY_MSG_SIZE);
        memcpy(_key, str.c_str(), str.size() > (KEY_MSG_SIZE - 1) ? (KEY_MSG_SIZE - 1) : str.size());
    }
    std::string GetKey() {
        return _key;
    }

    void SetValOffset(int offset,const int size)
    {
        data_pool_offset = offset;
        data_pool_size = size;
    }
    int GetDataPoolSize() {
        return data_pool_size;
    }

    void SetAttr(const UDataShmAttribute Attr) {
        memcpy((void*)&head_attr,(void*)&Attr,sizeof(UDataShmAttribute));
    }
    UDataShmAttribute GetAttr() {
        return head_attr;
    }
    int GetDataPoolOffset() {
        return data_pool_offset;
    }

    template <typename T>
    string vecPrintf(vector<T> & vec_data)
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
    std::string getString( char* data_pool_base, const std::string &prefix = "")
    {
        char *data_pool_ptr=data_pool_base+data_pool_offset;
        string str_out;
//        std::cout << GetKey() << std::endl;
        if (head_attr.SData.isDouble)
        {
            double data;
            memcpy((char*)&data,data_pool_ptr,sizeof(double));
            str_out = prefix+GetKey()+"|-d|"+to_string(data);

        }
        else if (head_attr.SData.isInt)
        {
            int data;
            memcpy((char*)&data,data_pool_ptr,sizeof(int));
            str_out = prefix+GetKey()+"|-i|"+to_string(data);
        }
        else if (head_attr.SData.isString)
        {
            string data(data_pool_ptr,head_attr.SData.isString_size);
            str_out = prefix+GetKey()+"|-s|"+data;
        }
        else if (head_attr.SData.isVecDouble)
        {
            vector<double> data(head_attr.SData.isVecDouble_size);
            memcpy((char*)&data[0],data_pool_ptr,head_attr.SData.isVecDouble_size*sizeof(double));
            str_out = prefix + GetKey() + "|-d|" + vecPrintf(data);
        }
        else if (head_attr.SData.isVecInt)
        {
            vector<int> data(head_attr.SData.isVecInt_size);
            memcpy((char*)&data[0],data_pool_ptr,head_attr.SData.isVecInt_size*sizeof(int));
            str_out = prefix + GetKey() + "|-i|" + vecPrintf(data);
        }
        else if (head_attr.SData.isVecString)
        {
            string frame_End="DLEEDLES";
            vector<string> data_vec;
            string all_data(data_pool_ptr,head_attr.SData.isVecString_bufsize);
            string data(data_pool_ptr,head_attr.SData.isString_size);
            CShmBaseFunc::Instance()->splitWithStl(all_data,data_vec,frame_End);
            str_out = prefix+GetKey()+"|-s|"+vecPrintf(data_vec);
        }


        return str_out;
    }
    void Print( char* data_pool_base, const std::string &prefix = "")
    {
        std::cout << getString(data_pool_base,prefix) << std::endl;
    }

} ArrayParam;


typedef struct ParamPool {
#define MODULE_MSG_SIZE 52
#define DATA_POOL_AREA_SIZE 128


    char module_name[MODULE_MSG_SIZE];
    int data_pool_area_size=0;
    ArrayParam data_pool_area[DATA_POOL_AREA_SIZE];
//    char * nouse_data_pool_ptr=NULL;

	ParamPool() {
        memset(module_name, 0, MODULE_MSG_SIZE);
	}

    void UpdateAttr(const std::string &key, const UDataShmAttribute &attr)
    {
        for (int i = 0; i < data_pool_area_size; i++) {
            if (data_pool_area[i].GetKey() == key) {
                data_pool_area[i].SetAttr(attr);
            }
        }
        return;
    }

    bool AddParam(const std::string &key, const UDataShmAttribute &attr, int offset, const int buf_size ) {
        if (data_pool_area_size >= DATA_POOL_AREA_SIZE ) {
            return false;
        }

        data_pool_area[data_pool_area_size].SetKey(key);
        data_pool_area[data_pool_area_size].SetValOffset(offset,buf_size);
        data_pool_area[data_pool_area_size].SetAttr(attr);
        data_pool_area_size++;
        return true;
    }

    int GetDataPoolOffset(const std::string &key)
    {
        for (int i = 0; i < data_pool_area_size; i++) {
            if (data_pool_area[i].GetKey() == key) {
                return data_pool_area[i].GetDataPoolOffset();
            }
        }
        return -1;
    }

    UDataShmAttribute GetDataPoolAttr(const std::string &key)
    {
        UDataShmAttribute attr;
        attr.IData[0]=0;attr.IData[1]=0;
        for (int i = 0; i < data_pool_area_size; i++) {
            if (data_pool_area[i].GetKey() == key) {
                return data_pool_area[i].GetAttr();
            }
        }
        return attr;
    }

    void Print( char* data_pool_base,const std::string &prefix = "") {
        std::cout << prefix << "module_name:" << GetModuleName()  << std::endl;
        std::cout << prefix << "data_pool_area_size:" << data_pool_area_size << std::endl;
        for (int i = 0; i < data_pool_area_size; i++) {
            data_pool_area[i].Print(data_pool_base,prefix + "\t");
        }

    }
	std::string GetModuleName() {
		return module_name;
	}


	void SetModuleName(const std::string &str) {
		memset(module_name, 0, MODULE_MSG_SIZE);
		memcpy(module_name, str.c_str(), str.size() > (MODULE_MSG_SIZE - 1) ? (MODULE_MSG_SIZE - 1) : str.size());
	}

} ParamPool;

typedef struct MultiModuleShmPool {
#define MODULE_AREA_SIZE 512
    int module_pool_size;

    ParamPool param[MODULE_AREA_SIZE];

    MultiModuleShmPool() {
        module_pool_size = 0;
    }

    void Print(const std::string &prefix = "") {
        std::cout << prefix << "module_pool_size:" << module_pool_size << std::endl;
        for (int i = 0; i < module_pool_size; i++) {
            param[i].Print(CShmBaseFunc::Instance()->data_pool_base_addr,prefix + "\t");
        }
    }

    bool AddModuleParam(const std::string &module_name) {
        for (int i = 0; i < module_pool_size; i++) {
            if (param[i].GetModuleName() == module_name) {
                return true;
            }
        }
        if (module_pool_size > MODULE_AREA_SIZE - 3) {
            return false;
        }
        param[module_pool_size].SetModuleName(module_name);
        module_pool_size++;
        return true;
    }

    bool UpdateAttr(const std::string &module_name,const std::string &key, const UDataShmAttribute &attr)
    {
        for (int i = 0; i < module_pool_size; i++)
        {
            if (param[i].GetModuleName() == module_name)
            {
                param[i].UpdateAttr(key, attr);
            }
        }
        return false;
    }

    bool AddParam(const std::string &module_name,const std::string &key, const UDataShmAttribute &attr,
                  char* data_pool_ptr, const int buf_size )
    {
        for (int i = 0; i < module_pool_size; i++) {
            if (param[i].GetModuleName() == module_name)
            {
                int offset = data_pool_ptr - CShmBaseFunc::Instance()->data_pool_base_addr;
                return param[i].AddParam(key, attr,offset,buf_size);
            }
        }
        return false;
    }

    char* GetDataPoolPtr(const std::string &module_name,const std::string &key)
    {
        for (int i = 0; i < module_pool_size; i++) {
            if (param[i].GetModuleName() == module_name)
            {

                int offset = param[i].GetDataPoolOffset(key);
                // std::cout << "offset " <<offset<<std::endl;
                if(offset==-1)
                {
                    std::cout << "NULL param[i].GetModuleName() " <<param[i].GetModuleName() <<" module_name "<<module_name<<std::endl;
                    return NULL;
                }
                // std::cout << "param[i].GetModuleName() " <<param[i].GetModuleName() <<" module_name "<<module_name<<std::endl;
                return (char*)(CShmBaseFunc::Instance()->data_pool_base_addr+offset);
            }
        }
        return NULL;
    }

    UDataShmAttribute GetDataPoolAttr(const std::string &module_name,const std::string &key)
    {
        UDataShmAttribute attr;
        for (int i = 0; i < module_pool_size; i++) {
            if (param[i].GetModuleName() == module_name) {
                return param[i].GetDataPoolAttr(key);
            }
        }
        return attr;
    }


} MultiModuleShmPool;


class CDataPoolbuf
{
public:
    CDataPoolbuf() {}
    int cur_data_pool_size=0;
    char _data_pool_ptr_[DATA_POOL_BUFSIZE];

};

class SHMParamApi
{

public:


	~SHMParamApi() {
        data_pool_sem.destroy();
        std::cout << "Uninit SHMParamApi" << std::endl;
        avos::common::SharedMemory::sharedMemoryDetatch(shm_param_api_);
		shm_param_api_ = nullptr;
		avos::common::ShmInfo info;
		if (avos::common::SharedMemory::sharedMemoryInfo(current_shmid, info) < 0) {
			std::cout << "cannot get shmid info " << current_shmid << std::endl;
			exit(0);
		}

		if (info.shm_nattach == 0) {
			avos::common::SharedMemory::sharedMemoryDelete(current_shmid);
			std::cout << "delete shm" << std::endl;
		}
	}

public:

    //double
    bool AddModuleParam(const std::string &module_name) {
        return shm_param_api_->AddModuleParam(module_name);
    }

    char *WriteNewDataPool(const double value)
    {
        if((data_pool_ptr_->cur_data_pool_size+sizeof(double)) > DATA_POOL_BUFSIZE)
        {
            return NULL;
        }
        memcpy(data_pool_ptr_->_data_pool_ptr_+data_pool_ptr_->cur_data_pool_size,(void*)&value,sizeof(double));
        data_pool_ptr_->cur_data_pool_size+=sizeof(double);
        return data_pool_ptr_->_data_pool_ptr_+data_pool_ptr_->cur_data_pool_size-sizeof(double);
    }
    void WriteExistDataPool(const double value, char* ptr)
    {
        memcpy(ptr,(void*)&value,sizeof(double));
    }

    bool SetDoubleModuleParam( const std::string &key, const double value,const std::string module="global")
    {
        lock_data_pool();
        char * ptr = shm_param_api_->GetDataPoolPtr(module,key);
        if(NULL==ptr)
        {
            UDataShmAttribute attr;
            attr.IData[0]=0;attr.IData[1]=0;
            attr.SData.isDouble=1;
            ptr = WriteNewDataPool(value);
            if(NULL==ptr)
            {
                unlock_data_pool();
                return false;
            }
            shm_param_api_->AddParam(module,key,attr,ptr,sizeof(double));
        }
        else
        {
            WriteExistDataPool(value,ptr);
        }
        unlock_data_pool();
        return true;
    }
    bool QueryDoubleModuleParam( const std::string &key, double &value,const std::string module="global")
    {
        char * ptr = shm_param_api_->GetDataPoolPtr(module,key);
        if(NULL==ptr)
        {
           std::cout<<"not found key: "<<key<<std::endl;
           return false;
        }
        else
        {
            UDataShmAttribute attr=shm_param_api_->GetDataPoolAttr(module,key);
            if(attr.IData[0]==0)
            {
                std::cout<<"not found attr: "<<key<<std::endl;
                return false;
            }
            if(!attr.SData.isDouble)
            {
                return false;
            }
            memcpy((char*)&value,ptr,sizeof(double));
        }
        return true;
    }

    //int
    char *WriteNewDataPool(const int value)
    {
        if((sizeof(int)+data_pool_ptr_->cur_data_pool_size) > DATA_POOL_BUFSIZE)
        {
            return NULL;
        }
        memcpy(data_pool_ptr_->_data_pool_ptr_+data_pool_ptr_->cur_data_pool_size,(void*)&value,sizeof(int));
        data_pool_ptr_->cur_data_pool_size+=sizeof(int);

        return data_pool_ptr_->_data_pool_ptr_+data_pool_ptr_->cur_data_pool_size-sizeof(int);
    }
    void WriteExistDataPool(const int value, char* ptr)
    {
        memcpy(ptr,(void*)&value,sizeof(int));
    }

    bool SetIntModuleParam( const std::string &key, const int value,const std::string module="global")
    {
        lock_data_pool();
        char * ptr = shm_param_api_->GetDataPoolPtr(module,key);
        if(NULL==ptr)
        {
            UDataShmAttribute attr;
            attr.IData[0]=0;attr.IData[1]=0;
            attr.SData.isInt=1;
            ptr = WriteNewDataPool(value);
            if(NULL==ptr)
            {
                unlock_data_pool();
                return false;
            }
            shm_param_api_->AddParam(module,key,attr,ptr,sizeof(int));
        }
        else
        {
            WriteExistDataPool(value,ptr);
        }
        unlock_data_pool();
        return true;
    }
    bool QueryIntModuleParam( const std::string &key, int &value,const std::string module="global")
    {
        char * ptr = shm_param_api_->GetDataPoolPtr(module,key);
        if(NULL==ptr)
        {
           std::cout<<"not found key: "<<key<<std::endl;
           return false;
        }
        else
        {
            UDataShmAttribute attr=shm_param_api_->GetDataPoolAttr(module,key);
            if(attr.IData[0]==0)
            {
                std::cout<<"not found attr: "<<key<<std::endl;
                return false;
            }
            if(!attr.SData.isInt)
            {
                return false;
            }
            memcpy((char*)&value,ptr,sizeof(int));
        }
        return true;
    }

    //string
    char *WriteNewDataPool(const string& value)
    {
        // std::cout << "para tools set value.size() = " << value.size() << std::endl;
        if((value.size()+data_pool_ptr_->cur_data_pool_size) > DATA_POOL_BUFSIZE)
        {
            return NULL;
        }
        memcpy(data_pool_ptr_->_data_pool_ptr_+data_pool_ptr_->cur_data_pool_size,value.c_str(),value.size());
        data_pool_ptr_->cur_data_pool_size += DATA_POOL_STRING_BUFSIZE;
        return data_pool_ptr_->_data_pool_ptr_ + data_pool_ptr_->cur_data_pool_size - DATA_POOL_STRING_BUFSIZE;
    }
    void WriteExistDataPool(const string& value,char* ptr)
    {
        // std::cout << "para tools set "<< " value = " << value << std::endl;

        memcpy(ptr,value.c_str(),(value.size() <DATA_POOL_STRING_BUFSIZE)?value.size():DATA_POOL_STRING_BUFSIZE);
    }

    bool SetStringModuleParam( const std::string &key, const std::string& value,const std::string module="global")
    {
        lock_data_pool();
        char * ptr = shm_param_api_->GetDataPoolPtr(module,key);

        UDataShmAttribute attr;
        attr.IData[0]=0;attr.IData[1]=0;
        attr.SData.isString=1;
        attr.SData.isString_size=value.size()<DATA_POOL_STRING_BUFSIZE?value.size():DATA_POOL_STRING_BUFSIZE;

        // std::cout << "para tools set key = " << key << " value = " << value<< std::endl;

        if (NULL == ptr)
        {
            ptr = WriteNewDataPool(value);
            if(NULL==ptr)
            {
                unlock_data_pool();
                return false;
            }
            shm_param_api_->AddParam(module,key,attr,ptr,DATA_POOL_STRING_BUFSIZE);
        }
        else
        {
            shm_param_api_->UpdateAttr(module,key,attr);
            WriteExistDataPool(value,ptr);
        }
        unlock_data_pool();
        return true;
    }

    bool QueryStringModuleParam( const std::string &key, string &value,const std::string module="global")
    {
        char * ptr = shm_param_api_->GetDataPoolPtr(module,key);
        if(NULL==ptr)
        {
           std::cout<<"not found key: "<<key<<std::endl;
           return false;
        }
        else
        {
            UDataShmAttribute attr=shm_param_api_->GetDataPoolAttr(module,key);
            if(attr.IData[0]==0)
            {
                std::cout<<"not found attr: "<<key<<std::endl;
                return false;
            }
            if(!attr.SData.isString)
            {
                return false;
            }
            value = string(ptr,attr.SData.isString_size);
        }
        return true;
    }


    //vector<double>
    char *WriteNewDataPool(const vector<double> &value)
    {
        if((data_pool_ptr_->cur_data_pool_size+value.size()*sizeof(double)) > DATA_POOL_BUFSIZE)
        {
            return NULL;
        }
        memcpy(data_pool_ptr_->_data_pool_ptr_+data_pool_ptr_->cur_data_pool_size,(char*)&value[0],value.size()*sizeof(double));
        data_pool_ptr_->cur_data_pool_size+=DATA_POOL_DOUBLE_VEC_SIZE*sizeof(double);
        return data_pool_ptr_->_data_pool_ptr_+data_pool_ptr_->cur_data_pool_size-DATA_POOL_DOUBLE_VEC_SIZE*sizeof(double);
    }
    void WriteExistDataPool(const vector<double> &value, char* ptr)
    {
        memcpy(ptr,(char*)&value[0],value.size()*sizeof(double));
    }

    bool SetDoubleModuleParam( const std::string &key, const vector<double> &value,const std::string module="global")
    {
        lock_data_pool();
        char * ptr = shm_param_api_->GetDataPoolPtr(module,key);

        UDataShmAttribute attr;
        attr.IData[0]=0;attr.IData[1]=0;
        attr.SData.isVecDouble=1;
        attr.SData.isVecDouble_size =value.size()<DATA_POOL_DOUBLE_VEC_SIZE?value.size():DATA_POOL_DOUBLE_VEC_SIZE;

        if(NULL==ptr)
        {
            ptr = WriteNewDataPool(value);
            if(NULL==ptr)
            {
                unlock_data_pool();
                return false;
            }
            shm_param_api_->AddParam(module,key,attr,ptr,DATA_POOL_DOUBLE_VEC_SIZE*sizeof(double));
        }
        else
        {
            shm_param_api_->UpdateAttr(module,key,attr);
            WriteExistDataPool(value,ptr);
        }
        unlock_data_pool();
        return true;
    }
    bool QueryDoubleModuleParam( const std::string &key, vector<double> &value,const std::string module="global")
    {
        char * ptr = shm_param_api_->GetDataPoolPtr(module,key);
        if(NULL==ptr)
        {
           std::cout<<"not found key: "<<key<<std::endl;
           return false;
        }
        else
        {
            UDataShmAttribute attr=shm_param_api_->GetDataPoolAttr(module,key);
            if(attr.IData[0]==0)
            {
                std::cout<<"not found attr: "<<key<<std::endl;
                return false;
            }
            if(!attr.SData.isVecDouble)
            {
                return false;
            }
            value.resize(attr.SData.isVecDouble_size);
            memcpy((char*)&value[0],ptr,attr.SData.isVecDouble_size*sizeof(double));
        }
        return true;
    }


    //vector<int>
    char *WriteNewDataPool(const vector<int> &value)
    {
        if((data_pool_ptr_->cur_data_pool_size+value.size()*sizeof(int)) > DATA_POOL_BUFSIZE)
        {
            return NULL;
        }
        memcpy(data_pool_ptr_->_data_pool_ptr_+data_pool_ptr_->cur_data_pool_size,(char*)&value[0],value.size()*sizeof(int));
        data_pool_ptr_->cur_data_pool_size+=DATA_POOL_INT_VEC_SIZE*sizeof(int);
        return data_pool_ptr_->_data_pool_ptr_+data_pool_ptr_->cur_data_pool_size-DATA_POOL_INT_VEC_SIZE*sizeof(int);
    }
    void WriteExistDataPool(const vector<int> &value, char* ptr)
    {
        memcpy(ptr,(char*)&value[0],value.size()*sizeof(int));
    }

    bool SetIntModuleParam( const std::string &key, const vector<int> &value,const std::string module="global")
    {
        lock_data_pool();
        char * ptr = shm_param_api_->GetDataPoolPtr(module,key);

        UDataShmAttribute attr;
        attr.IData[0]=0;attr.IData[1]=0;
        attr.SData.isVecInt=1;
        attr.SData.isVecInt_size =value.size()<DATA_POOL_INT_VEC_SIZE?value.size():DATA_POOL_INT_VEC_SIZE;

        if(NULL==ptr)
        {
            ptr = WriteNewDataPool(value);
            if(NULL==ptr)
            {
                unlock_data_pool();
                return false;
            }
            shm_param_api_->AddParam(module,key,attr,ptr,DATA_POOL_INT_VEC_SIZE*sizeof(int));
        }
        else
        {
            shm_param_api_->UpdateAttr(module,key,attr);
            WriteExistDataPool(value,ptr);
        }
        unlock_data_pool();
        return true;
    }
    bool QueryIntModuleParam( const std::string &key, vector<int> &value,const std::string module="global")
    {
        char * ptr = shm_param_api_->GetDataPoolPtr(module,key);
        if(NULL==ptr)
        {
           std::cout<<"not found key: "<<key<<std::endl;
           return false;
        }
        else
        {
            UDataShmAttribute attr=shm_param_api_->GetDataPoolAttr(module,key);
            if(attr.IData[0]==0)
            {
                std::cout<<"not found attr: "<<key<<std::endl;
                return false;
            }
            if(!attr.SData.isVecInt)
            {
                return false;
            }
            value.resize(attr.SData.isVecInt_size);
            memcpy((char*)&value[0],ptr,attr.SData.isVecInt_size*sizeof(int));
        }
        return true;
    }


    //vector<string>
//    string frame_Start="DLES";
    string frame_End="DLEEDLES";

    int SerializationSize(const vector<string> &input)
    {
        int cur_index=0;
        for (int var = 0; var < input.size(); ++var)
        {
//            cur_index+=frame_Start.size();
            cur_index+=input[var].size();
            cur_index+=frame_End.size();
        }
        return cur_index;
    }

    int Serialization(char **output,const vector<string> &input)
    {
        char *array_char = *output;
        int cur_index=0;

        for (int var = 0; var < input.size(); ++var)
        {

//            memcpy(array_char+cur_index,frame_Start.c_str(),frame_Start.size());
//            cur_index+=frame_Start.size();

            memcpy(array_char+cur_index,(char*)(&input[var][0]),input[var].size());
            cur_index+=input[var].size();

            memcpy(array_char+cur_index,frame_End.c_str(),frame_End.size());
            cur_index+=frame_End.size();
        }
        return cur_index;
    }

    char *WriteNewDataPool(const vector<string> &value)
    {
        int size = SerializationSize(value);
        if(data_pool_ptr_->cur_data_pool_size+size > DATA_POOL_BUFSIZE
                || size>DATA_POOL_STRING_VEC_BUFSIZE)
        {
            std::cout << "this vector<string> is bigger than 30*1024 is "<<size<<std::endl;
            return NULL;
        }
        char * ptr=data_pool_ptr_->_data_pool_ptr_+data_pool_ptr_->cur_data_pool_size;
        Serialization(&ptr,value);
        data_pool_ptr_->cur_data_pool_size+=DATA_POOL_STRING_VEC_BUFSIZE;
        return data_pool_ptr_->_data_pool_ptr_+data_pool_ptr_->cur_data_pool_size-DATA_POOL_STRING_VEC_BUFSIZE;
    }
    void WriteExistDataPool(const vector<string> &value, char* ptr)
    {
        int size = SerializationSize(value);
        if(size>DATA_POOL_STRING_VEC_BUFSIZE)
        {
            std::cout << "this vector<string> is bigger than 30*1024 is "<<size<<std::endl;
            return ;
        }
        Serialization(&ptr,value);
    }

    bool SetStringModuleParam( const std::string &key, const vector<string> &value,const std::string module="global")
    {
        lock_data_pool();
        char * ptr = shm_param_api_->GetDataPoolPtr(module,key);

        UDataShmAttribute attr;
        attr.IData[0]=0;attr.IData[1]=0;
        attr.SData.isVecString=1;
        attr.SData.isVecString_size  =value.size()<DATA_POOL_STRING_VEC_SIZE?value.size():DATA_POOL_STRING_VEC_SIZE;
        attr.SData.isVecString_bufsize = SerializationSize(value);
        if(NULL==ptr)
        {
            ptr = WriteNewDataPool(value);
            if(NULL==ptr)
            {
                unlock_data_pool();
                return false;
            }

            shm_param_api_->AddParam(module,key,attr,ptr,DATA_POOL_STRING_VEC_SIZE);
        }
        else
        {
            shm_param_api_->UpdateAttr(module,key,attr);
            WriteExistDataPool(value,ptr);
        }
        unlock_data_pool();
        return true;
    }
    bool QueryStringModuleParam( const std::string &key, vector<string> &value,const std::string module="global")
    {
        char * ptr = shm_param_api_->GetDataPoolPtr(module,key);
        if(NULL==ptr)
        {
           std::cout<<"not found key: "<<key<<std::endl;
           return false;
        }
        else
        {
            UDataShmAttribute attr=shm_param_api_->GetDataPoolAttr(module,key);
            if(attr.IData[0]==0)
            {
                std::cout<<"not found attr: "<<key<<std::endl;
                return false;
            }
            if(!attr.SData.isVecString)
            {
                return false;
            }
            string all_data(ptr,attr.SData.isVecString_bufsize);
            CShmBaseFunc::Instance()->splitWithStl(all_data,value,frame_End);
        }
        return true;
    }

    bool FindKeyInThisDataPool(const std::string module,const std::string &key)
    {
        bool bRet;
        char * ptr = shm_param_api_->GetDataPoolPtr(module,key);
        if(NULL==ptr)
        {
           bRet = false;
        }
        else
        {
           bRet = true;
        }
        return bRet;
    }

    void Print(const std::string &prefix = "") {
        shm_param_api_->Print(prefix);
    }

    void lock_data_pool()
    {
        // 对共享内存上锁
        // printf("data_pool_sem.wait() start\n");
        if (data_pool_sem.wait() == false)
        {
           printf("data_pool_sem.wait failed\n");
        }
        // printf("data_pool_sem.wait() ok\n");
    }

    void unlock_data_pool()
    {
        // 写完之后，解锁
        // printf("data_pool_sem.post() start\n");
        if (data_pool_sem.post() == false)
        {
           printf("data_pool_sem.post failed\n");
        }
        // printf("data_pool_sem.post() ok\n");
    }

    MultiModuleShmPool* getMultiShmPool()
    {
        return shm_param_api_;
    }
private:


	bool ExeLinuxCmd(const std::string& cmd) {
		pid_t status = system(cmd.c_str());
		if (-1 == status) {
			return false;
		}

		if (WIFEXITED(status)) {
			if (0 == WEXITSTATUS(status)) {
				return true;
			}

			std::cout << "ExeLinuxCmdBlock " << cmd << "error returned " << WEXITSTATUS(status) << std::endl;
			return false;
		}
		return true;
	}

	SHMParamApi() {

        sem_key = avos::common::SharedMemory::BKDRHash("avos_data_pool_sem");
        // 信号量，第一步：创建信号量
        if (data_pool_sem.init(sem_key) == false)
        {
            printf("data_pool_sem.init failed\n");
            // exit(0);
        }
        // printf("data_pool_sem.init ok\n");

        lock_data_pool();
        current_shmid = avos::common::SharedMemory::sharedMemoryCreateOrGet(avos::common::SharedMemory::BKDRHash("avos_shm_param_pool_112"), sizeof(MultiModuleShmPool) / 4096 * 4096 + 4096);
        // std::cout << "current_shmid:" << current_shmid << std::endl;
        if (current_shmid == -1) {
            std::cout << "1 error to alloc current_shmid " << current_shmid << std::endl;
            exit(0);
        }

        shm_param_api_ = (MultiModuleShmPool *)avos::common::SharedMemory::sharedMemoryAttach(current_shmid);
//        AddModuleParam("global");
        // printf("shm_param_api_ = %p\n",&shm_param_api_->module_pool_size);

        bool bCreate = false;
        data_pool_shmid = avos::common::SharedMemory::sharedMemoryCreateOrGet(avos::common::SharedMemory::BKDRHash("avos_data_pool"), sizeof(CDataPoolbuf)/ 4096 * 4096 + 4096,bCreate);
        // std::cout << "data_pool_shmid:" << data_pool_shmid << std::endl;
        if (data_pool_shmid == -1) {
            std::cout << "error to alloc data_pool_shmid " << data_pool_shmid << std::endl;
			exit(0);
		}

        data_pool_ptr_ = (CDataPoolbuf *)avos::common::SharedMemory::sharedMemoryAttach(data_pool_shmid);
        if(bCreate)
            data_pool_ptr_->cur_data_pool_size=0;

        CShmBaseFunc::Instance()->data_pool_base_addr = data_pool_ptr_->_data_pool_ptr_;
//        shm_param_api_->data_pool_base_addr = data_pool_ptr_->_data_pool_ptr_;
        // printf("data_pool_ptr_->cur_data_pool_size=%d\n",data_pool_ptr_->cur_data_pool_size);

        // printf("data_pool_ptr_->_data_pool_ptr_[0] = %p\n",&data_pool_ptr_->_data_pool_ptr_[0]);
        // for (size_t i = 0; i < 1024; i++)
        // {
        //     printf("0x%x,", data_pool_ptr_->_data_pool_ptr_[i]);
        // }
        // printf("\n");

        // std::string data_pool_str(data_pool_ptr_->_data_pool_ptr_,1024);
        // std::cout << "data_pool_str:"<<data_pool_str<<std::endl;
        fflush(stdout);
        unlock_data_pool();
    }

     BASE_DECLARE_SINGLETON(SHMParamApi)


private:
    MultiModuleShmPool *shm_param_api_;
	int current_shmid;

    CDataPoolbuf* data_pool_ptr_;
    int data_pool_shmid;

    key_t sem_key;
    CSEM data_pool_sem;
};



#endif

