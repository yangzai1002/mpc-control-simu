#ifndef CCODE_TAG_H
#define CCODE_TAG_H
#include <iostream>
#include <chrono>
#include <string>
#include <unordered_map>
#include <thread>
#include <pthread.h>
#include <string.h>
#include <iostream>
#include <chrono>
#include <string>
#include <unordered_map>
#include <thread>
#include <pthread.h>
#include <iostream>
#include <vector>
#include <cstring>
#include <stdarg.h>
#include <sys/syscall.h>

#include "macros.h"
#include "cshmqueue.h"

#define CODETAG_BUFLEN 1024*1024 //1024*1024 len

#define FUNC_NAME_MAX_LEN 32
#define TAG_NAME_MAX_LEN 64

struct SCodeTag {
    int32_t thread_id;
    char func_name[FUNC_NAME_MAX_LEN];
    char tag_name[TAG_NAME_MAX_LEN];
    u_int16_t line;
    u_int64_t stamp;
    u_int64_t user_stamp;
    u_int16_t user_id;
};

enum EAvosCodetagId {
    eAvosCodetagstartId,
    eAvosCodetagMidtagId,
    eAvosCodetagPubtagId,
    eAvosCodetagSubtagId,
    eAvosCodetagCallStartBtagId,
    eAvosCodetagCallStartEtagId,
    eAvosCodetagE2EFristtagId,
    eAvosCodetagE2EtagId,
    eAvosCodetagEndId,
};
class CCodeTag {
 public:
    CCodeTag()
    {
        constexpr const char* const SHM_NAME = "/code_tag_queue";
        //    shm_unlink(SHM_NAME);
        // Create the shared memory region
        int fd = shm_open(SHM_NAME, O_RDWR, 0);  // 打开共享内存，获得文件描述符
        if (fd == -1) {
            if (errno == ENOENT) {  // 表示共享内存不存在
                shm_unlink(SHM_NAME);

                printf("Shared memory does not exist, creating...\n");
                fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
                //            shm_fd = shm_open(SHM_NAME, O_CREAT | O_EXCL | O_RDWR, 0666);
                if (fd == -1) {
                    // 创建共享内存失败
                    printf("Failed to create shared memory\n");
                }

                // 创建共享内存成功后，进行其他操作，比如映射到进程地址空间
                // Size the shared memory region appropriately
                const std::size_t region_size =
                        sizeof(LockFreeQueue<SCodeTag, CODETAG_BUFLEN>) + sizeof(SCodeTag) * CODETAG_BUFLEN;
                if (ftruncate(fd, region_size) == -1) {
                    shm_unlink(SHM_NAME);
                    error_exit("ftruncate");
                }

                // Map the shared memory region into our address space
                void* const shm_ptr = mmap(NULL, region_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
                memset(shm_ptr, 0, region_size);
                if (shm_ptr == MAP_FAILED) {
                    shm_unlink(SHM_NAME);
                    error_exit("mmap");
                }

                // Initialize the shared lock-free queue
                //            mqueue = new (shm_ptr) LockFreeQueue<SCodeTag,
                //            CODETAG_BUFLEN>(reinterpret_cast<SCodeTag*>(static_cast<char*>(shm_ptr) +
                //            sizeof(LockFreeQueue<SCodeTag, CODETAG_BUFLEN>)));
                mqueue = (LockFreeQueue<SCodeTag, CODETAG_BUFLEN>*)shm_ptr;
                mqueue->init();

            } else {  // 打开共享内存失败
                printf("Failed to open shared memory\n");
            }
        } else {
            // Size the shared memory region appropriately
            const std::size_t region_size =
                    sizeof(LockFreeQueue<SCodeTag, CODETAG_BUFLEN>) + sizeof(SCodeTag) * CODETAG_BUFLEN;
            if (ftruncate(fd, region_size) == -1) {
                shm_unlink(SHM_NAME);
                error_exit("ftruncate");
            }

            // Map the shared memory region into our address space
            void* shm_ptr = mmap(NULL, region_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
            if (shm_ptr == MAP_FAILED) {
                shm_unlink(SHM_NAME);
                error_exit("mmap");
            }

            // Initialize the shared lock-free queue
            mqueue = (LockFreeQueue<SCodeTag, CODETAG_BUFLEN>*)shm_ptr;
            //        *(int*)((char*)shm_ptr+sizeof(SharedHeader)) = (int)(
            //        (intptr_t)shm_ptr+sizeof(SharedHeader)+sizeof(void*));
            //        mqueue->setbufaddr((intptr_t)shm_ptr+sizeof(SharedHeader)+sizeof(void*));
            //        mqueue->
        }
    }

    void codetag(const char* tag, const char* func, const u_int16_t line, u_int64_t user_stamp, u_int8_t user_id)
    {
        // Store the current thread ID and start time for this function call
        SCodeTag code_tag;
        code_tag.thread_id = syscall(SYS_gettid);
        int len = FUNC_NAME_MAX_LEN < strlen(func) ? FUNC_NAME_MAX_LEN : strlen(func);
        memcpy(code_tag.func_name, func, len);
        code_tag.func_name[len < (FUNC_NAME_MAX_LEN - 1) ? len : (FUNC_NAME_MAX_LEN - 1)] = '\0';
        len = TAG_NAME_MAX_LEN < strlen(tag) ? TAG_NAME_MAX_LEN : strlen(tag);
        memcpy(code_tag.tag_name, tag, len);
        code_tag.tag_name[len < (TAG_NAME_MAX_LEN - 1) ? len : (TAG_NAME_MAX_LEN - 1)] = '\0';
        code_tag.line = line;
        code_tag.stamp = std::chrono::time_point_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now())
                .time_since_epoch()
                .count();
        code_tag.user_stamp = user_stamp;
        code_tag.user_id = user_id;
        //    printf("code_tag.thread_id = %d,code_tag.func_name = %s,code_tag.tag_name = %s,"
        //           "code_tag.line = %d,code_tag.stamp =
        //           %ld\n",code_tag.thread_id,code_tag.func_name,code_tag.tag_name,code_tag.line,code_tag.stamp);
        mqueue->write(code_tag);
    }

    LockFreeQueue<SCodeTag, CODETAG_BUFLEN>* getqueue()
    {
        return mqueue;
    }


private:
    void error_exit(const char* message) {
        perror(message);
        exit(EXIT_FAILURE);
    }
private:

    LockFreeQueue<SCodeTag, CODETAG_BUFLEN>* mqueue;
    BASE_DECLARE_SINGLETON(CCodeTag)
};

class CCodeTagOpt
{
public:
    CCodeTagOpt()
     {
        mqueue = CCodeTag::Instance()->getqueue();
    }
    void readcodetag(std::vector<SCodeTag> & code_tag_list)
    {
        int cur_count = mqueue->getheader()->write_count.load();
        int buf_len = mqueue->getheader()->buf_len.load();
        // int cur_write_index = mqueue->getheader()->write_index.load();
        int cur_write_index = mqueue->getheader()->write_count.load() & (CODETAG_BUFLEN-1);
        int read_len = cur_count - read_count;

        int catch_len = 1024;

        if (read_len > buf_len) {
            code_tag_list.clear();
            // code_tag_list.resize(cur_write_index);
            // std::memcpy( (char*)code_tag_list.data(),
            //              (char*)mqueue+sizeof(LockFreeQueue<SCodeTag, CODETAG_BUFLEN>),
            //              cur_write_index*sizeof(SCodeTag));
            if (buf_len > (cur_write_index + catch_len)) {
                code_tag_list.resize(buf_len - catch_len);
                std::memcpy(
                            (char*)code_tag_list.data(),
                            (char*)mqueue + sizeof(LockFreeQueue<SCodeTag, CODETAG_BUFLEN>) +
                            (cur_write_index + catch_len) * sizeof(SCodeTag),
                            (buf_len - (cur_write_index + catch_len)) * sizeof(SCodeTag));
                std::memcpy(
                            (char*)code_tag_list.data() + (buf_len - (cur_write_index + catch_len)) * sizeof(SCodeTag),
                            (char*)mqueue + sizeof(LockFreeQueue<SCodeTag, CODETAG_BUFLEN>),
                            cur_write_index * sizeof(SCodeTag));
            }
            else
            {
                code_tag_list.resize(cur_write_index);
                std::memcpy( (char*)code_tag_list.data(),
                            (char*)mqueue+sizeof(LockFreeQueue<SCodeTag, CODETAG_BUFLEN>),
                            cur_write_index*sizeof(SCodeTag));
            }
        } else {
            if (read_index > cur_write_index) {
                code_tag_list.clear();
                code_tag_list.resize(buf_len - read_index + cur_write_index);
                std::memcpy(
                            (char*)code_tag_list.data(),
                            (char*)mqueue + sizeof(LockFreeQueue<SCodeTag, CODETAG_BUFLEN>) + read_index * sizeof(SCodeTag),
                            (buf_len - read_index) * sizeof(SCodeTag));
                std::memcpy(
                            (char*)code_tag_list.data() + (buf_len - read_index) * sizeof(SCodeTag),
                            (char*)mqueue + sizeof(LockFreeQueue<SCodeTag, CODETAG_BUFLEN>),
                            cur_write_index * sizeof(SCodeTag));
            } else {
                code_tag_list.clear();
                code_tag_list.resize(cur_write_index - read_index);
                std::memcpy(
                            (char*)code_tag_list.data(),
                            (char*)mqueue + sizeof(LockFreeQueue<SCodeTag, CODETAG_BUFLEN>) + read_index * sizeof(SCodeTag),
                            (cur_write_index - read_index) * sizeof(SCodeTag));
            }
        }

        read_count = cur_count;
        read_index = cur_write_index;
    }


    void readcodetag(int &read_index,std::vector<SCodeTag> & code_tag_list)
    {
        int cur_count = mqueue->getheader()->write_count.load();
        int buf_len = mqueue->getheader()->buf_len.load();
        // int cur_write_index = mqueue->getheader()->write_index.load();
        int cur_write_index = mqueue->getheader()->write_count.load() & (CODETAG_BUFLEN - 1);
        int read_len = cur_count - read_count;
        code_tag_list.clear();
        if(read_index == -1)
        {
            read_index = cur_write_index;
        }
        else if (read_index == cur_write_index)
        {

        }
        else if(read_index < cur_write_index)
        {
            code_tag_list.resize(cur_write_index - read_index);
            std::memcpy(
                        (char*)code_tag_list.data(),
                        (char*)mqueue + sizeof(LockFreeQueue<SCodeTag, CODETAG_BUFLEN>) +
                        (read_index) * sizeof(SCodeTag),
                        (cur_write_index - read_index) * sizeof(SCodeTag));

            read_index = cur_write_index;
        }
        else if(read_index > cur_write_index)
        {
            code_tag_list.resize(buf_len-(read_index-cur_write_index));
            std::memcpy(
                        (char*)code_tag_list.data(),
                        (char*)mqueue + sizeof(LockFreeQueue<SCodeTag, CODETAG_BUFLEN>) +
                        (read_index) * sizeof(SCodeTag),
                        (buf_len - read_index) * sizeof(SCodeTag));
            std::memcpy(
                        (char*)code_tag_list.data()+(buf_len - read_index) * sizeof(SCodeTag),
                        (char*)mqueue + sizeof(LockFreeQueue<SCodeTag, CODETAG_BUFLEN>),
                        (cur_write_index) * sizeof(SCodeTag));

            read_index = cur_write_index;
        }
    }

    void readcodetag(int &read_index, void* code_tag_buf, int& code_tag_buf_len)
    {
        int cur_count = mqueue->getheader()->write_count.load();
        int buf_len = mqueue->getheader()->buf_len.load();
        // int cur_write_index = mqueue->getheader()->write_index.load();
        int cur_write_index = mqueue->getheader()->write_count.load() & (CODETAG_BUFLEN-1);
        int read_len = cur_count - read_count;
        code_tag_buf_len = 0;
        
        if(read_index == -1)
        {
            read_index = cur_write_index;
        }
        else if (read_index == cur_write_index)
        {

        }
        else if(read_index < cur_write_index)
        {
            code_tag_buf_len = (cur_write_index - read_index)*sizeof(SCodeTag);
            memset(code_tag_buf, '\0', code_tag_buf_len);
            memcpy(
                        (char*)code_tag_buf,
                        (char*)mqueue + sizeof(LockFreeQueue<SCodeTag, CODETAG_BUFLEN>) +
                        (read_index) * sizeof(SCodeTag),
                        (cur_write_index - read_index) * sizeof(SCodeTag));

            read_index = cur_write_index;
        }
        else if(read_index > cur_write_index)
        {
            code_tag_buf_len = (buf_len-(read_index-cur_write_index))*sizeof(SCodeTag);
            memset(code_tag_buf, '\0', code_tag_buf_len);
            memcpy(
                        (char*)code_tag_buf,
                        (char*)mqueue + sizeof(LockFreeQueue<SCodeTag, CODETAG_BUFLEN>) +
                        (read_index) * sizeof(SCodeTag),
                        (buf_len - read_index) * sizeof(SCodeTag));
            memcpy(
                        (char*)code_tag_buf+(buf_len - read_index) * sizeof(SCodeTag),
                        (char*)mqueue + sizeof(LockFreeQueue<SCodeTag, CODETAG_BUFLEN>),
                        (cur_write_index) * sizeof(SCodeTag));

            read_index = cur_write_index;
        }
    }
private:
    LockFreeQueue<SCodeTag, CODETAG_BUFLEN>* mqueue;
    int read_index = 0;
    int read_count = 0;
    BASE_DECLARE_SINGLETON(CCodeTagOpt)
};

void error_exit(const char* message);

#define CODETAG(__tag__, __userstamp__, __userid__) \
    CCodeTag::Instance()->codetag(__tag__, __func__, __LINE__, __userstamp__, __userid__)


#define AVOS_CODETAG(__tag__) CODETAG(__tag__, 0, eAvosCodetagstartId)
#define AVOS_CODETAG_ADV1(__tag__, __userstamp__, __userid__) CODETAG(__tag__, __userstamp__, __userid__)


#define GETCODETAG(__list__) CCodeTagOpt::Instance()->readcodetag(__list__)
#define AVOS_GETCODETAG(__list__) GETCODETAG(__list__)

#define AVOS_GETCODETAG_VCT(__readindex__,__list__) CCodeTagOpt::Instance()->readcodetag(__readindex__,__list__)
#define AVOS_GETCODETAG_BUF(__readindex__,__buf__,__len__) CCodeTagOpt::Instance()->readcodetag(__readindex__,__buf__,__len__)


#endif //

