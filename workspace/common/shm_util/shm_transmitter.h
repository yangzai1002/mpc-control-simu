#ifndef __SHM_TRANSMITTER_H__
#define __SHM_TRANSMITTER_H__
#include <stdio.h>
#include <cstdlib>
#include <unistd.h>

#include <atomic>
#include <iostream>
#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include "shared_memory.h"
#include "time/node_time.h"

namespace avos {
namespace common {
template <class T>
class ShmTransmiiter {
public:
	ShmTransmiiter() {
		header_buffer_pointer = nullptr;
		topic = "";
		shmid_header_ = -1;
		start_fre = -1;
		start_time = -1.0;
		is_data_empty = true;
		std::vector<std::string> res_vec;
		ExeLinuxCmd(
		    "ipcs -m | awk '{if($6==0) print $2,$4}' | grep 666 | awk '{print $1}' "
		    "| xargs -I {} ipcrm -m {}",
		    res_vec);
	}
	~ShmTransmiiter() {
		std::cout << "Uninit ShmTransmiiter" << std::endl;
		ShmInfo info;
		if (shmid_header_ != -1) {
			avos::common::SharedMemory::sharedMemoryDetatch(
			    (void *)header_buffer_pointer);
			avos::common::SharedMemory::sharedMemoryInfo(shmid_header_, info);
			if (info.shm_nattach == 0) {
				std::cout << "delete" << std::endl;
				avos::common::SharedMemory::sharedMemoryDelete(shmid_header_);
			}
		}
		header_buffer_pointer = nullptr;

		if (shmid_data_ != -1) {
			avos::common::SharedMemory::sharedMemoryDetatch(
			    (void *)data_buffer_pointer);
			avos::common::SharedMemory::sharedMemoryInfo(shmid_data_, info);
			if (info.shm_nattach == 0) {
				std::cout << "delete" << std::endl;
				avos::common::SharedMemory::sharedMemoryDelete(shmid_data_);
			}
		}
		data_buffer_pointer = nullptr;
	}

public:
	void GetInfo() {
		ShmInfo info;
		avos::common::SharedMemory::sharedMemoryInfo(shmid_header_, info);
		info.print();
	}

	int InitWriter(const std::string &_topic, int _buffer_len = 10) {
		std::cout << "Init ShmTransmiiter writer" << std::endl;
		topic = _topic;
		data_topic = topic + "data";
		buffer_len = _buffer_len;
		if (_buffer_len <= 0) _buffer_len = 10;
		shmid_header_ = avos::common::SharedMemory::sharedMemoryCreateOrGet(
		                    avos::common::SharedMemory::BKDRHash(topic.c_str()),
		                    (sizeof(ShmHeader) / 4096 + 1) * 4096);

		if (shmid_header_ == -1) return -1;

		std::cout << "object_shmid_header_:" << shmid_header_ << std::endl;
		header_buffer_pointer =
		    (ShmHeader *)avos::common::SharedMemory::sharedMemoryAttach(
		        shmid_header_);
		header_buffer_pointer->buffer_len.store(_buffer_len);

		if (header_buffer_pointer->buffer_len.load() == -1) {
			header_buffer_pointer->buffer_len.store(_buffer_len);
		}
		if (buffer_len > header_buffer_pointer->buffer_len.load()) {
			buffer_len = header_buffer_pointer->buffer_len.load();
		}
		read_loc = header_buffer_pointer->highest_loc;
		header_buffer_pointer->shm_attach_times++;

		// data topic

		shmid_data_ = avos::common::SharedMemory::sharedMemoryCreateOrGet(
		                  avos::common::SharedMemory::BKDRHash(data_topic.c_str()),
		                  ((sizeof(T) + sizeof(ShmAreaHeader)) * buffer_len / 4096 + 1) * 4096);
		if (shmid_data_ == -1) return -1;
		std::cout << "shmid_data_:" << shmid_data_ << std::endl;
		data_buffer_pointer =
		    (char *)avos::common::SharedMemory::sharedMemoryAttach(shmid_data_);

		return 0;
	}

	int InitReader(const std::string &_topic) {
		std::cout << "Init ShmTransmiiter reader" << std::endl;
		topic = _topic;
		data_topic = topic + "data";
		shmid_header_ = avos::common::SharedMemory::sharedMemoryCreateOrGet(
		                    avos::common::SharedMemory::BKDRHash(topic.c_str()),
		                    (sizeof(ShmHeader) / 4096 + 1) * 4096);

		if (shmid_header_ == -1) return -1;
		std::cout << "object_shmid_header_:" << shmid_header_ << std::endl;
		header_buffer_pointer =
		    (ShmHeader *)avos::common::SharedMemory::sharedMemoryAttach(
		        shmid_header_);

		if (header_buffer_pointer->buffer_len.load() <= 0) {
			return -1;
		}

		read_loc = header_buffer_pointer->highest_loc;
		header_buffer_pointer->shm_attach_times++;

		buffer_len = header_buffer_pointer->buffer_len.load();
		// data topic
		shmid_data_ = avos::common::SharedMemory::sharedMemoryCreateOrGet(
		                  avos::common::SharedMemory::BKDRHash(data_topic.c_str()),
		                  ((sizeof(T) + sizeof(ShmAreaHeader)) * buffer_len / 4096 + 1) * 4096);
		if (shmid_data_ == -1) return -1;
		std::cout << "shmid_data_:" << shmid_data_ << std::endl;
		is_data_empty = false;
		data_buffer_pointer =
		    (char *)avos::common::SharedMemory::sharedMemoryAttach(shmid_data_);
		return 0;
	}

	bool GetData(T &data) {
		if (is_data_empty) {
			InitReader(topic);
			usleep(1000);
			return false;
		}
		if (header_buffer_pointer == nullptr || data_buffer_pointer == nullptr) {
			std::cout << "GetData pointer is null" << std::endl;
			return false;
		}
		pShmAreaHeader pointer_header;
		// std::cout << "read_loc " << read_loc.array_index << "|" <<
		// read_loc.seq_num << std::endl;
		int index = GetNextReadIndex(read_loc, pointer_header);

		if (index == -1) return false;
		pointer_header->shm_status.store(eShmAreaStatus::READING);
		memcpy((char *)&data, (char *)pointer_header + sizeof(ShmAreaHeader),
		       sizeof(T));
		pointer_header->shm_status.store(eShmAreaStatus::IDLE);
		// std::cout << "gap_times:"
		//           << avos::common::NodeTime::Now().ToSecond() -
		//                  pointer_header->send_time_stamp
		//           << std::endl;
		return true;
	}

	bool GetUniqueDataPtr(T * (&data)) {
		if (is_data_empty) {
			InitReader(topic);
			return false;
		}
		if (header_buffer_pointer == nullptr || data_buffer_pointer == nullptr) {
			std::cout << "GetData pointer is null" << std::endl;
			return false;
		}
		ShmAreaHeader *p = GetIndexHeader((read_loc.array_index) % buffer_len);
		if (p == nullptr) {
			return false;
		}
		data = (T *)((char *)p + sizeof(ShmAreaHeader));
		return true;
	}

	bool GetDataPtr(T * (&data)) {
		if (is_data_empty) {
			InitReader(topic);
			return false;
		}
		if (header_buffer_pointer == nullptr || data_buffer_pointer == nullptr) {
			std::cout << "GetData pointer is null" << std::endl;
			return false;
		}

		// std::cout << "read_loc.array_index:" << read_loc.array_index <<
		// std::endl;
		ShmAreaHeader *p = GetIndexHeader((read_loc.array_index) % buffer_len);
		if (p != nullptr && p->shm_status.load() == eShmAreaStatus::READING) {
			p->shm_status.store(eShmAreaStatus::IDLE);
		}
		pShmAreaHeader pointer_header;
		// std::cout << "read_loc " << read_loc.array_index << "|" <<
		// read_loc.seq_num << std::endl;
		int index = GetNextReadIndex(read_loc, pointer_header);

		if (index == -1) return false;
		pointer_header->shm_status.store(eShmAreaStatus::READING);
		data = (T *)((char *)pointer_header + sizeof(ShmAreaHeader));
		// std::cout << "gap_times:"
		//           << avos::common::NodeTime::Now().ToSecond() -
		//                  pointer_header->send_time_stamp
		//           << std::endl;
		return true;
	}

	bool GetNewestData(T &data) {
		if (is_data_empty) {
			InitReader(topic);
			usleep(1000 );
			std::cout << "is_data_empty" << std::endl;
			return false;
		}
		if (header_buffer_pointer == nullptr || data_buffer_pointer == nullptr) {
			std::cout << "GetData pointer is null" << std::endl;
			return false;
		}

		ShmAreaHeader *pointer_header = GetIndexHeader(
		                                    (header_buffer_pointer->highest_loc.array_index) % buffer_len);
		if (pointer_header == nullptr) {
			std::cout << "pointer_header is null" << std::endl;
			return false;
		}



		while (pointer_header->shm_status.load() == eShmAreaStatus::WRITING) {
			usleep(10);
		}

		if (header_buffer_pointer->highest_loc.seq_num <= read_loc.seq_num) {
			// std::cout << "sequence is error " << read_loc.seq_num << "|"
			// 		  << header_buffer_pointer->highest_loc.seq_num << std::endl;
			pointer_header->shm_status.store(eShmAreaStatus::IDLE);
			// std::cout << "header_buffer_pointer->highest_loc.seq_num <= read_loc.seq_num" << std::endl;
			return false;
		}




		pointer_header->shm_status.store(eShmAreaStatus::READING);
		read_loc = header_buffer_pointer->highest_loc;

		// std::cout << "read_loc " << read_loc.array_index << "|" <<
		// read_loc.seq_num << std::endl; int index = GetNextReadIndex(read_loc,
		// pointer_header);

		// if (index == -1) return false;

		memcpy((char *)&data, (char *)pointer_header + sizeof(ShmAreaHeader),
		       sizeof(T));
		pointer_header->shm_status.store(eShmAreaStatus::IDLE);
		// std::cout << "gap_times:"
		//           << avos::common::NodeTime::Now().ToSecond() -
		//                  pointer_header->send_time_stamp
		//           << std::endl;
		return true;
	}

	bool SetData(const T &data) {
		if (header_buffer_pointer == nullptr || data_buffer_pointer == nullptr) {
			return false;
		}
		pShmAreaHeader pointer_header;
		AreaLocation temp_highest_loc = header_buffer_pointer->highest_loc;
		int res = GetNextWriteIndex(temp_highest_loc, pointer_header);
		if (res == -1) {
			// pointer_header->shm_status.store(eShmAreaStatus::IDLE);
			return false;
		}
		pointer_header->shm_status.store(eShmAreaStatus::WRITING);
		pointer_header->send_time_stamp.store(
		    avos::common::NodeTime::Now().ToSecond());
		header_buffer_pointer->highest_loc = temp_highest_loc;
		pointer_header->sequence = header_buffer_pointer->highest_loc.seq_num;
		memcpy((char *)pointer_header + sizeof(ShmAreaHeader), (char *)&data,
		       sizeof(T));
		FillInnerLoc(header_buffer_pointer->lowest_loc,
		             header_buffer_pointer->highest_loc);
		pointer_header->shm_status.store(eShmAreaStatus::IDLE);
		return true;
	}

	void PrintShmInfo() {
		if (header_buffer_pointer != nullptr) {
			header_buffer_pointer->PrintInfo();
		}
	}

	double GetFrequence() {
		if (header_buffer_pointer == nullptr || data_buffer_pointer == nullptr) {
			return 0.0;
		}
		if (start_fre <= 0 || start_time < 0) {
			start_fre = header_buffer_pointer->highest_loc.seq_num;
			start_time = avos::common::NodeTime::Now().ToSecond();
			return 0.0;
		}

		return (header_buffer_pointer->highest_loc.seq_num - start_fre) /
		       (avos::common::NodeTime::Now().ToSecond() - start_time);
	}

private:
	ShmAreaHeader *GetIndexHeader(const int &index) {
		if (index < 0 || index >= buffer_len) {
			return nullptr;
		}
		return (ShmAreaHeader *)((char *)data_buffer_pointer +
		                         index * (sizeof(T) + sizeof(ShmAreaHeader)));
	}

	int GetNextReadIndex(AreaLocation &loc, pShmAreaHeader &next_p) {
		for (int i = 1; i < buffer_len; i++) {
			ShmAreaHeader *p = GetIndexHeader((loc.array_index + i) % buffer_len);
			if (p == nullptr) {
				std::cout << "p is nullptr" << std::endl;
				return -1;
			}
			// std::cout << "AreaLocation loc:" << loc.seq_num << "|" << p->sequence
			// << std::endl;
			if (p->sequence > loc.seq_num &&
			        p->shm_status.load() != eShmAreaStatus::WRITING) {
				next_p = p;
				loc.array_index = (loc.array_index + i) % buffer_len;
				loc.seq_num = p->sequence;
				return 0;
			}
		}
		return -1;
	}

	int GetNextWriteIndex(AreaLocation &loc, pShmAreaHeader &area) {
		for (int i = 1; i < buffer_len; i++) {
			ShmAreaHeader *p = GetIndexHeader((loc.array_index + i) % buffer_len);
			if (p == nullptr) {
				std::cout << "p is nullptr" << std::endl;
				return -1;
			}
			if (p->shm_status.load() != eShmAreaStatus::READING) {
				loc.seq_num++;
				loc.array_index = (loc.array_index + i) % buffer_len;
				p->shm_status.store(eShmAreaStatus::WRITING);
				area = p;
				return 0;
			}
		}
		return -1;
	}

	void FillInnerLoc(AreaLocation &lowest, AreaLocation &highest) {
		int min_seq_num = 99999999;
		int max_seq_num = -1;

		AreaLocation temp_lowest, temp_highest;
		for (int i = 0; i < buffer_len; i++) {
			ShmAreaHeader *p = GetIndexHeader(i);
			if (p == nullptr) {
				std::cout << "p is nullptr" << std::endl;
			}
			if (p->sequence < min_seq_num) {
				min_seq_num = p->sequence;
				temp_lowest.array_index = i;
				temp_lowest.seq_num = min_seq_num;
			}
			if (p->sequence > max_seq_num) {
				max_seq_num = p->sequence;
				temp_highest.array_index = i;
				temp_highest.seq_num = max_seq_num;
			}
		}
		lowest = temp_lowest;
		highest = temp_highest;
	}

	int ExeLinuxCmd(const std::string &cmd, std::vector<std::string> &resvec) {
		resvec.clear();
		FILE *pp = popen(cmd.c_str(), "r");  //建立管道
		if (!pp) {
			return -1;
		}
		char tmp[1024];  //设置一个合适的长度，以存储每一行输出
		while (fgets(tmp, sizeof(tmp), pp) != NULL) {
			if (tmp[strlen(tmp) - 1] == '\n') {
				tmp[strlen(tmp) - 1] = '\0';  //去除换行符
			}
			resvec.push_back(tmp);
		}
		pclose(pp);  //关闭管道
		return resvec.size();
	}

private:
	ShmHeader *header_buffer_pointer;
	char *data_buffer_pointer;
	std::string topic, data_topic;
	int shmid_header_;
	int shmid_data_;

	AreaLocation read_loc;

	int buffer_len;
	bool is_data_empty;

	int start_fre;
	double start_time;
};

}  // namespace common
}  // namespace avos

#endif
