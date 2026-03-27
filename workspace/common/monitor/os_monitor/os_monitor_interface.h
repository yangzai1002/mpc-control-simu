#ifndef __AVOS_OS_MONITOR_INTERFACE_H__
#define __AVOS_OS_MONITOR_INTERFACE_H__

#include "shm_util/shared_memory.h"
#include "macros.h"
#include "common_typedef.h"

#include "thread_pool/thread_pool.h"
namespace avos {
namespace common {
class OsMonitorInterface
{
private:
	OsMonitorInterface() {
		current_shmid = avos::common::SharedMemory::sharedMemoryCreateOrGet(avos::common::SharedMemory::BKDRHash("avos_os_monitor"), sizeof(BoardResourceInfo) / 4096 * 4096 + 4096);
		std::cout << "shmid:" << current_shmid << "|" << (sizeof(BoardResourceInfo) / 4096 * 4096 + 4096) << std::endl;
		if (current_shmid == -1) {
			std::cout << "5 error to alloc current_shmid " << current_shmid << std::endl;
			exit(0);
		}

		board_resource_info_ptr = (BoardResourceInfo *)avos::common::SharedMemory::sharedMemoryAttach(current_shmid);
		board_resource_info_ptr->size = 1;
	}


public:
	void PrintAll() {
		while (1) {
			for (int i = 0; i < board_resource_info_ptr->size; i++) {
				std::cout << "-----" << std::endl;
				board_resource_info_ptr->Print();
			}
			sleep(2);
		}
	}

	void PrintDetail(int index = 0) {

		if (index < 0 || index > board_resource_info_ptr->size) {
			std::cout << "cannot get target index:" << index << std::endl;
			return;
		}
		while (1) {
			board_resource_info_ptr->os_resource_info[index].global_info.Print();
			board_resource_info_ptr->os_resource_info[index].net_info.Print();
			board_resource_info_ptr->os_resource_info[index].dis_io_info.Print();
			board_resource_info_ptr->os_resource_info[index].mem_info.Print();
			board_resource_info_ptr->os_resource_info[index].gpu_info.Print();
			board_resource_info_ptr->os_resource_info[index].cpu_info.Print();
			sleep(2);
		}
	}



	void StartXavierOSMonitor() {
		auto TP = ThreadPool::Instance();
		TP->Init(6);
		int current_index = 0;


		board_resource_info_ptr->os_resource_info[current_index].Init();
		auto global_info_monitor_work = [&]() {
			board_resource_info_ptr->os_resource_info[current_index].global_info.FillData();
			return 0;
		};
		int global_info_monitor_handle = TP->getIdleThreadHandle();
		TP->AddThreadWork(global_info_monitor_handle, global_info_monitor_work);


		auto cpu_monitor_work = [&]() {
			board_resource_info_ptr->os_resource_info[current_index].cpu_info.XavierFillData();
			return 0;
		};
		int cpu_monitor_handle = TP->getIdleThreadHandle();
		TP->AddThreadWork(cpu_monitor_handle, cpu_monitor_work);

		auto gpu_monitor_work = [&]() {
			board_resource_info_ptr->os_resource_info[current_index].gpu_info.XavierFillData();
			return 0;
		};
		int gpu_monitor_handle = TP->getIdleThreadHandle();
		TP->AddThreadWork(gpu_monitor_handle, gpu_monitor_work);


		auto net_monitor_work = [&]() {
			board_resource_info_ptr->os_resource_info[current_index].net_info.FillData();
			return 0;
		};
		int net_monitor_handle = TP->getIdleThreadHandle();
		TP->AddThreadWork(net_monitor_handle, net_monitor_work);


		auto disk_monitor_work = [&]() {
			std::vector<std::string> vec;
			vec.push_back("mmcblk0");
			vec.push_back("nvme0n1");
			vec.push_back("loop0");

			board_resource_info_ptr->os_resource_info[current_index].dis_io_info.FillData(vec);
			return 0;
		};
		int disk_monitor_handle = TP->getIdleThreadHandle();
		TP->AddThreadWork(disk_monitor_handle, disk_monitor_work);
		while (1) {
			board_resource_info_ptr->os_resource_info[current_index].mem_info.FillData();
			sleep(2);
		}

	}
	void StartX86OSMonitor() {
		auto TP = ThreadPool::Instance();
		TP->Init(6);
		int current_index = 0;


		board_resource_info_ptr->os_resource_info[current_index].Init();
		auto global_info_monitor_work = [&]() {
			board_resource_info_ptr->os_resource_info[current_index].global_info.FillData();
			return 0;
		};
		int global_info_monitor_handle = TP->getIdleThreadHandle();
		TP->AddThreadWork(global_info_monitor_handle, global_info_monitor_work);


		auto cpu_monitor_work = [&]() {
			board_resource_info_ptr->os_resource_info[current_index].cpu_info.X86FillData();
			return 0;
		};
		int cpu_monitor_handle = TP->getIdleThreadHandle();
		TP->AddThreadWork(cpu_monitor_handle, cpu_monitor_work);


		auto net_monitor_work = [&]() {
			board_resource_info_ptr->os_resource_info[current_index].net_info.FillData();
			return 0;
		};
		int net_monitor_handle = TP->getIdleThreadHandle();
		TP->AddThreadWork(net_monitor_handle, net_monitor_work);


		auto disk_monitor_work = [&]() {
			std::vector<std::string> vec;
			vec.push_back("sda");
			vec.push_back("loop0");
			board_resource_info_ptr->os_resource_info[current_index].dis_io_info.FillData(vec);
			return 0;
		};
		int disk_monitor_handle = TP->getIdleThreadHandle();
		TP->AddThreadWork(disk_monitor_handle, disk_monitor_work);
		while (1) {
			board_resource_info_ptr->os_resource_info[current_index].mem_info.FillData();
			sleep(2);
		}

	}
	~OsMonitorInterface() {
		avos::common::SharedMemory::sharedMemoryDetatch(board_resource_info_ptr);
		board_resource_info_ptr = nullptr;
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

	BASE_DECLARE_SINGLETON(OsMonitorInterface)
private:
	int current_shmid;
public:
	BoardResourceInfo *board_resource_info_ptr;
};
}
}

#endif
