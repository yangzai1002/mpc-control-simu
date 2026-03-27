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

            // Method to remove a message pack pointer
            void remove(const std::string& key) {
                std::lock_guard<std::mutex> lock(mtx);
                msgPackMap.erase(key);
            }
        };

    } // namespace idpware
} // namespace avos
