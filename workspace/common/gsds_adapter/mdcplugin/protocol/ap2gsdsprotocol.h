#pragma once

#include <mutex>
#include <condition_variable>
#include <atomic>
#include <map>
#include <vector>


namespace avos
{
    namespace idpware
    {
        enum EIdpIOType
        {
            IOCTL_MDC2GSDS_PARAM,
            IOCTL_GSDS2MDC_PARAM,
        };
        struct SMsgPackPtr
        {
            char msgtopicname[128];
            int msgtopiclen;
            char* msgptr;
            int msglen;

            SMsgPackPtr& operator=(const SMsgPackPtr& other) {
                if (this != &other) {
                    strcpy(msgtopicname, other.msgtopicname);
                    msgtopiclen = other.msgtopiclen;
                    msgptr = other.msgptr;
                    msglen = other.msglen;
                }
                return *this;
            }
        };
        
        class MsgPackManager {
        private:
            std::map<std::string, SMsgPackPtr> msgPackMap;
            std::map<std::string, std::stringstream> msgPackStrStreamMap;
            std::map<std::string, std::string> msgPackStringMap;
            std::mutex mtx;

            // Private constructor to prevent instantiation
            MsgPackManager() {}

            // Deleted copy constructor and assignment operator
            MsgPackManager(const MsgPackManager&) = delete;
            MsgPackManager& operator=(const MsgPackManager&) = delete;



        public:
            // Static method to get the single instance of the class
            static MsgPackManager& getInstance() {
                static MsgPackManager instance;
                return instance;
            }

            // Method to set a message pack pointer
            void set(const std::string& key, const SMsgPackPtr& msgPackPtr) {
                std::lock_guard<std::mutex> lock(mtx);
                msgPackMap[key] = msgPackPtr;
            }

            // Method to get a message pack pointer
            SMsgPackPtr* get(const std::string& key) {
                std::lock_guard<std::mutex> lock(mtx);
                auto it = msgPackMap.find(key);
                if (it != msgPackMap.end()) {
                    return &it->second;
                }
                return nullptr;
            }

            void updataStream(const std::string& key)
            {
                std::lock_guard<std::mutex> lock(mtx);
                memcpy(msgPackMap[key].msgtopicname,key.c_str(),key.size());
                msgPackMap[key].msgtopiclen = key.size();
                msgPackMap[key].msgptr = (char*)msgPackStringMap[key].data();
                msgPackMap[key].msglen = msgPackStringMap[key].size();
                printf("updata topic is %s,ptr is 0x%x,len is %d\n",key.c_str(),msgPackMap[key].msgptr,msgPackMap[key].msglen);
            }

            // Method to remove a message pack pointer
            void remove(const std::string& key) {
                std::lock_guard<std::mutex> lock(mtx);
                msgPackMap.erase(key);
            }
            /* 返回 key 对应的 stringstream 引用；如果不存在则默认构造一个并插入 */
            std::stringstream& getStream(const std::string& key) {
                std::lock_guard<std::mutex> lock(mtx);
                return msgPackStrStreamMap[key];        // map::operator[] 不存在时会默认构造
            }

            std::string& getString(const std::string& key) {
                std::lock_guard<std::mutex> lock(mtx);
                return msgPackStringMap[key];        // map::operator[] 不存在时会默认构造
            }

            /* 只读版本：返回指针，不存在返回 nullptr */
            std::stringstream* getStreamIfExists(const std::string& key) {
                std::lock_guard<std::mutex> lock(mtx);
                auto it = msgPackStrStreamMap.find(key);
                return (it == msgPackStrStreamMap.end()) ? nullptr : &it->second;
            }

            /* 如果以后需要删除 */
            void removeStream(const std::string& key) {
                std::lock_guard<std::mutex> lock(mtx);
                msgPackStrStreamMap.erase(key);
            }
        };

    } // namespace idpware
} // namespace avos

