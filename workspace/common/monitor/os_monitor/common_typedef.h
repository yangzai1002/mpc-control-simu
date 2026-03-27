#ifndef __COMPUTER_MONITOR_COMMON_TYPEDEF_H__
#define __COMPUTER_MONITOR_COMMON_TYPEDEF_H__

#include <map>
#include <string>
#include <sstream>
#include <mutex>
#include <atomic>
#include <unistd.h>
#include <iomanip>

#include "common_header/util.h"
#include "time/node_time.h"

namespace avos {
namespace common {


struct OSMemInfo
{
	std::atomic<int> is_writing;
	int total_mem;  // kb
	int free_mem;	// kb
	int used_mem;	// kb

	OSMemInfo(const OSMemInfo & c)
	{
		total_mem = c.total_mem;
		free_mem = c.free_mem;
		used_mem = c.used_mem;
	}

	OSMemInfo& operator= (const OSMemInfo& c) {
		while (is_writing.load() == 1) {
			usleep(1000 * 10);
		}
		total_mem = c.total_mem;
		free_mem = c.free_mem;
		used_mem = c.used_mem;
		return *this;
	}

	void Print(const std::string &prefix =  "") {
		while (is_writing.load() == 1) {
			usleep(1000 * 10);
		}
		std::cout << prefix << "total_mem(kb):" << total_mem << std::endl;
		std::cout << prefix << "free_mem(kb):" << free_mem << std::endl;
		std::cout << prefix << "used_mem(kb):" << used_mem << std::endl;
	}

	OSMemInfo() {
		total_mem = 0;
		free_mem = 0;
		used_mem = 0;
		is_writing.store(0);
	}

	void FillData() {
		std::string cmd = "cat /proc/meminfo | grep -E 'MemTotal|MemAvailable' | awk '{print $2}'";
		std::vector<std::string> resvec;
		avos::common::Util::ExeLinuxCmd(cmd, resvec);
		if (resvec.size() != 2) {
			std::cout << "error exe " << cmd << std::endl;
		} else {
			is_writing.store(1);

			std::stringstream ss_total;
			ss_total << resvec[0];
			ss_total >> total_mem;
			std::stringstream ss_free;
			ss_free << resvec[1];
			ss_free >> free_mem;
			used_mem = total_mem - free_mem;

			is_writing.store(0);
		}
	}
};



typedef struct SingleCpuInfo
{
	float cpu_freq;

	float usr_percent;
	float sys_percent;
	float iowait_percent;
	float irq_percent;
	float soft_irq_percent;
	float idle_percent;

	SingleCpuInfo() {
		cpu_freq = 0.0;
		usr_percent = 0.0;
		sys_percent = 0.0;
		iowait_percent = 0.0;
		irq_percent = 0.0;
		soft_irq_percent = 0.0;
		idle_percent = 0.0;
	}

	void Print(const std::string prefix = "") {
		std::cout << prefix << " cpu_freq:" << cpu_freq << std::endl;
		std::cout << prefix << " usr_percent:" << usr_percent << std::endl;
		std::cout << prefix << " sys_percent:" << sys_percent << std::endl;
		std::cout << prefix << " iowait_percent:" << iowait_percent << std::endl;
		std::cout << prefix << " irq_percent:" << irq_percent << std::endl;
		std::cout << prefix << " soft_irq_percent:" << soft_irq_percent << std::endl;
		std::cout << prefix << " idle_percent:" << idle_percent << std::endl;
	}

} SingleCpuInfo;

struct OSCpuInfo
{
#define MAX_CPU_NUM 	64
	std::atomic<int> is_writing;
	int cpu_size;
	SingleCpuInfo all_cpu_info;
	SingleCpuInfo cpu_infos[MAX_CPU_NUM];

	OSCpuInfo() {
		cpu_size = 0;
		is_writing.store(0);
	}

	OSCpuInfo(const OSCpuInfo & c)
	{
		cpu_size = c.cpu_size;
		all_cpu_info = c.all_cpu_info;
		memcpy((char*)&cpu_infos, (char*)&c.cpu_infos, MAX_CPU_NUM * sizeof(SingleCpuInfo));
	}

	OSCpuInfo& operator= (const OSCpuInfo& c) {
		while (is_writing.load() == 1) {
			usleep(1000 * 10);
		}
		cpu_size = c.cpu_size;
		all_cpu_info = c.all_cpu_info;
		memcpy((char*)&cpu_infos, (char*)&c.cpu_infos, MAX_CPU_NUM * sizeof(SingleCpuInfo));
		return *this;
	}

	void Print(const std::string &prefix = "") {
		while (is_writing.load() == 1) {
			usleep(1000 * 10);
		}

		std::cout << prefix << "-ALL CPU average info:" << std::endl;
		all_cpu_info.Print(prefix + "\t");
		for (int i = 0; i < cpu_size; i++) {
			std::cout << prefix << "-the " << i << "th cpu info:" << std::endl;
			cpu_infos[i].Print(prefix + "\t");
		}
	}

	void XavierFillData() {
		while (1) {
			std::string cmd = "jetson_clocks --show | grep ^cpu | awk '{print $6}'";
			std::vector<std::string> resvec;
			avos::common::Util::ExeLinuxCmd(cmd, resvec);
			if (resvec.size() == 0) {
				std::cout << "error exe " << cmd << std::endl;
			} else if (resvec.size() > MAX_CPU_NUM) {
				std::cout << "error: cpu is overflow " << resvec.size() << "|" << MAX_CPU_NUM << std::endl;
			} else {
				cpu_size = resvec.size();
				is_writing.store(1);
				double sum = 0;
				for (int i = 0; i < resvec.size(); i++) {
					std::stringstream ss;

					ss << resvec[i].substr(resvec[i].find('=') + 1, resvec[i].size());
					std::cout << "freq:" << ss.str() << std::endl;
					ss >> cpu_infos[i].cpu_freq;
					sum += cpu_infos[i].cpu_freq;
				}
				all_cpu_info.cpu_freq = sum / resvec.size();
				is_writing.store(0);
			}

			// cpus
			cmd = "mpstat -P ALL 2 1| grep Average | awk '{if (NR>1) {print $3,$5,$6,$7,$8,$12}}'";
			avos::common::Util::ExeLinuxCmd(cmd, resvec);
			if (resvec.size() == 0) {
				std::cout << "error exe " << cmd << std::endl;
				return;
			}
			if (cpu_size != (resvec.size() - 1)) {
				std::cout << "warning:cpu size is not equal: " << cpu_size << "|" << resvec.size() << std::endl;
			}
			is_writing.store(1);
			for (int i = 0; i < resvec.size(); i++) {
				std::stringstream ss;
				ss << resvec[i];
				if (i == 0) {
					ss >> all_cpu_info.usr_percent;
					ss >> all_cpu_info.sys_percent;
					ss >> all_cpu_info.iowait_percent;
					ss >> all_cpu_info.irq_percent;
					ss >> all_cpu_info.soft_irq_percent;
					ss >> all_cpu_info.idle_percent;
				} else {
					ss >> cpu_infos[i - 1].usr_percent;
					ss >> cpu_infos[i - 1].sys_percent;
					ss >> cpu_infos[i - 1].iowait_percent;
					ss >> cpu_infos[i - 1].irq_percent;
					ss >> cpu_infos[i - 1].soft_irq_percent;
					ss >> cpu_infos[i - 1].idle_percent;
				}
			}
			is_writing.store(0);
		}

	}


	void X86FillData() {
		while (1) {
			// 1 cpu_freq
			std::string cmd = "cat /proc/cpuinfo | grep 'cpu MHz' | awk '{print $4}'";
			std::vector<std::string> resvec;
			avos::common::Util::ExeLinuxCmd(cmd, resvec);
			if (resvec.size() == 0) {
				std::cout << "error exe " << cmd << std::endl;
			} else if (resvec.size() > MAX_CPU_NUM) {
				std::cout << "error: cpu is overflow " << resvec.size() << "|" << MAX_CPU_NUM << std::endl;
			} else {
				cpu_size = resvec.size();
				is_writing.store(1);
				double sum = 0;
				for (int i = 0; i < resvec.size(); i++) {
					std::stringstream ss;
					ss << resvec[i];
					ss >> cpu_infos[i].cpu_freq;
					sum += cpu_infos[i].cpu_freq;
				}
				all_cpu_info.cpu_freq = sum / resvec.size();
				is_writing.store(0);
			}

			// cpus
			cmd = "mpstat -P ALL 2 1| grep Average | awk '{if (NR>1) {print $3,$5,$6,$7,$8,$12}}'";
			avos::common::Util::ExeLinuxCmd(cmd, resvec);
			if (resvec.size() == 0) {
				std::cout << "error exe " << cmd << std::endl;
				return;
			}
			if (cpu_size != (resvec.size() - 1)) {
				std::cout << "warning:cpu size is not equal: " << cpu_size << "|" << resvec.size() << std::endl;
			}
			is_writing.store(1);
			for (int i = 0; i < resvec.size(); i++) {
				std::stringstream ss;
				ss << resvec[i];
				if (i == 0) {
					ss >> all_cpu_info.usr_percent;
					ss >> all_cpu_info.sys_percent;
					ss >> all_cpu_info.iowait_percent;
					ss >> all_cpu_info.irq_percent;
					ss >> all_cpu_info.soft_irq_percent;
					ss >> all_cpu_info.idle_percent;
				} else {
					ss >> cpu_infos[i - 1].usr_percent;
					ss >> cpu_infos[i - 1].sys_percent;
					ss >> cpu_infos[i - 1].iowait_percent;
					ss >> cpu_infos[i - 1].irq_percent;
					ss >> cpu_infos[i - 1].soft_irq_percent;
					ss >> cpu_infos[i - 1].idle_percent;
				}
			}
			is_writing.store(0);
		}
	}

};


struct OSGpuInfo
{
	std::atomic<int> is_writing;
	float gpu_temperature;
	float usage_rate;
	float gpu_freq;
	OSGpuInfo() {
		is_writing.store(0);
		gpu_temperature = 0.0;
		usage_rate = 0.0;
		gpu_freq = 0.0;
	}

	OSGpuInfo(const OSGpuInfo & c) {
		gpu_temperature = c.gpu_temperature;
		usage_rate = c.usage_rate;
		gpu_freq = c.gpu_freq;
	}

	OSGpuInfo& operator= (const OSGpuInfo& c) {
		while (is_writing.load() == 1) {
			usleep(1000 * 10);
		}
		gpu_temperature = c.gpu_temperature;
		usage_rate = c.usage_rate;
		gpu_freq = c.gpu_freq;
		return *this;
	}

	void Print(const std::string prefix = "") {
		while (is_writing.load() == 1) {
			usleep(1000 * 10);
		}
		std::cout << prefix << " gpu_temperature:" << gpu_temperature << std::endl;
		std::cout << prefix << " usage_rate:" << usage_rate << std::endl;
		std::cout << prefix << " gpu_freq:" << gpu_freq << std::endl;
	}


	void GetGpuInfo(const std::string &str, float &percent, float &freq, float &temprature) {
		std::string sub_str = str.substr(str.find("GR3D_FREQ") + sizeof("GR3D_FREQ"), str.size());

		std::stringstream ss ;
		std::string str1, str2;
		ss.str("");

		ss << sub_str;
		ss >> str1;
		ss.str("");
		ss << str1.substr(0, str1.find("%@"));
		ss >> percent;


		std::stringstream ss_freq ;
		ss_freq << str1.substr(str1.find("%@") + sizeof("%@") - 1, str1.size());
		std::cout << "ss_freq.str():" << ss_freq.str() << std::endl;
		ss_freq >> freq;


		std::stringstream ss_temprature ;
		str1 = sub_str.substr(sub_str.find("GPU@") + sizeof("GPU@") - 1, sub_str.size());
		ss_temprature.str("");
		ss_temprature << str1;
		ss_temprature >> str1;
		std::cout << "str1:" << str1 << std::endl;

		ss_temprature.str("");
		ss_temprature << str1;
		ss_temprature >> temprature;
	}

	void XavierFillData() {
		while (1) {
			std::vector<std::string> resvec;
			std::string cmd = "timeout 2 tegrastats --interval 1500";
			avos::common::Util::ExeLinuxCmd(cmd, resvec);
			if (resvec.size() != 1) {
				std::cout << "error exe " << cmd << std::endl;
				continue;
			}
			GetGpuInfo(resvec[0], usage_rate, gpu_freq, gpu_temperature);

			sleep(1);
		}
	}


	void X86FillData() {

	}
};



typedef struct DiskIOInfo
{
#define IO_NAME_MAX_LENGTH 20
	char io_name[IO_NAME_MAX_LENGTH];
	float rkbps;
	float wkbps;
	float average_io_queue_size;
	float average_waiting_io_time; // ms
	float average_service_io_time; // ms
	float util_percent;
	DiskIOInfo() {
		rkbps = 0.0;
		wkbps = 0.0;
		average_io_queue_size = 0.0;
		average_waiting_io_time = 0.0; // ms
		average_service_io_time = 0.0; // ms
		util_percent = 0.0;
	}

	DiskIOInfo(const DiskIOInfo & c)
	{
		rkbps = c.rkbps;
		wkbps = c.wkbps;
		average_io_queue_size = c.average_io_queue_size;
		average_waiting_io_time = c.average_waiting_io_time;
		average_service_io_time = c.average_service_io_time;
		util_percent = c.util_percent;
		memcpy(io_name, c.io_name, IO_NAME_MAX_LENGTH);
	}

	DiskIOInfo& operator= (const DiskIOInfo& c) {
		rkbps = c.rkbps;
		wkbps = c.wkbps;
		average_io_queue_size = c.average_io_queue_size;
		average_waiting_io_time = c.average_waiting_io_time;
		average_service_io_time = c.average_service_io_time;
		util_percent = c.util_percent;
		memcpy(io_name, c.io_name, IO_NAME_MAX_LENGTH);
		return *this;
	}

	void SetIOName(const std::string &str) {
		memset(io_name, 0, IO_NAME_MAX_LENGTH);
		memcpy(io_name, str.c_str(), str.size() > (IO_NAME_MAX_LENGTH - 1) ? (IO_NAME_MAX_LENGTH - 1) : str.size());
	}

	std::string GetModuleName() {
		return io_name;
	}

	void Print(const std::string prefix = "") {
		std::cout << prefix << " io_name:" << GetModuleName() << std::endl;
		std::cout << prefix << " \trkbps:" << rkbps << std::endl;
		std::cout << prefix << " \twkbps:" << wkbps << std::endl;
		std::cout << prefix << " \taverage_io_queue_size:" << average_io_queue_size << std::endl;
		std::cout << prefix << " \taverage_waiting_io_time:" << average_waiting_io_time << std::endl;
		std::cout << prefix << " \taverage_service_io_time:" << average_service_io_time << std::endl;
		std::cout << prefix << " \tutil_percent:" << util_percent << std::endl;
	}

} DiskIOInfo;

typedef struct DiskIOInfoList
{
#define MAX_DISK_MONITOR_SIZE 32
	std::atomic<int> is_writing;
	int size;
	DiskIOInfo io_infos[MAX_DISK_MONITOR_SIZE];
	DiskIOInfoList() {
		size = 0;
		is_writing.store(0);
	}

	DiskIOInfoList(const DiskIOInfoList & c)
	{
		size = c.size;
		for (int i = 0; i < size; i++) {
			io_infos[i] = c.io_infos[i];
		}
	}

	DiskIOInfoList& operator= (const DiskIOInfoList& c) {
		while (is_writing.load() == 1) {
			usleep(1000 * 10);
		}
		size = c.size;
		for (int i = 0; i < size; i++) {
			io_infos[i] = c.io_infos[i];
		}
		return *this;
	}

	void Print(const std::string &prefix = "") {
		while (is_writing.load() == 1) {
			usleep(1000 * 10);
		}
		for (int i = 0; i < size; i++) {
			io_infos[i].Print(prefix);
		}
	}


	void FillData(const std::vector<std::string> &IO_list) {
		while (1) {
			std::string cmd = "iostat -dkx 2 2 | grep -E '";
			for (int i = 0; i < IO_list.size(); i++) {
				cmd = cmd + IO_list[i];
				if (i != IO_list.size() - 1) {
					cmd = cmd + " |";
				} else {
					cmd = cmd + " '";
				}
			}
			cmd = cmd + " |  awk '{if (NR>" + std::to_string(IO_list.size()) + ") {print $1,$6,$7,$9,$10,$13,$14}}'";
			// std::cout << "FillDiskIoInfo:" << cmd << std::endl;
			// std::string cmd = "iostat -dkx 2 2 | grep -E 'loop0|sda' | awk '{if (NR>2) {print $1,$6,$7,$9,$10,$13,$14}}'";
			std::vector<std::string> resvec;
			avos::common::Util::ExeLinuxCmd(cmd, resvec);
			if (resvec.size() != IO_list.size()) {
				std::cout << "error exe " << cmd << std::endl;
				continue;
			}
			is_writing.store(1);
			size = resvec.size();
			for (int i = 0; i < resvec.size(); i++) {
				std::stringstream ss;
				ss << resvec[i];
				std::string name;
				ss >> name;

				io_infos[i].SetIOName(name);
				ss >> io_infos[i].rkbps;
				ss >> io_infos[i].wkbps;
				ss >> io_infos[i].average_io_queue_size;
				ss >> io_infos[i].average_waiting_io_time;
				ss >> io_infos[i].average_service_io_time;
				ss >> io_infos[i].util_percent;
			}
			is_writing.store(0);

		}
	}
} DiskIOInfoList;

typedef struct NetInfo
{
#define NET_NAME_MAX_LENGTH 20
	char net_name[NET_NAME_MAX_LENGTH];
	float rkbps;
	float wkbps;
	float usage_rate;
	NetInfo() {
		rkbps = 0;
		wkbps = 0;
		usage_rate = 0;
	}
	NetInfo(const NetInfo & c)
	{
		rkbps = c.rkbps;
		wkbps = c.wkbps;
		usage_rate = c.usage_rate;
		memcpy(net_name, c.net_name, NET_NAME_MAX_LENGTH);
	}

	NetInfo& operator= (const NetInfo& c) {
		rkbps = c.rkbps;
		wkbps = c.wkbps;
		usage_rate = c.usage_rate;
		memcpy(net_name, c.net_name, NET_NAME_MAX_LENGTH);
		return *this;
	}

	void Print(const std::string &prefix = "") {
		std::cout << prefix << "name:" << net_name << std::endl;
		std::cout << prefix << "\trkbps:" << rkbps << std::endl;
		std::cout << prefix << "\twkbps:" << wkbps << std::endl;
		std::cout << prefix << "\tusage_rate:" << usage_rate << std::endl;
	}

	void SetNetName(const std::string &str) {
		memset(net_name, 0, NET_NAME_MAX_LENGTH);
		memcpy(net_name, str.c_str(), str.size() > (NET_NAME_MAX_LENGTH - 1) ? (NET_NAME_MAX_LENGTH - 1) : str.size());
	}

	std::string GetNetName() {
		return net_name;
	}
} NetInfo;

struct NetInfoList
{
#define MAX_NET_MONITOR_SIZE 32
	std::atomic<int> is_writing;
	int size;
	NetInfo net_infos[MAX_NET_MONITOR_SIZE];
	NetInfoList() {
		is_writing.store(0);
		size = 0;
	}

	NetInfoList(const NetInfoList & c)
	{
		size = c.size;
		for (int i = 0; i < size; i++) {
			net_infos[i] = c.net_infos[i];
		}
	}

	NetInfoList& operator= (const NetInfoList& c) {
		while (is_writing.load() == 1) {
			usleep(1000 * 10);
		}
		size = c.size;
		for (int i = 0; i < size; i++) {
			net_infos[i] = c.net_infos[i];
		}
		return *this;
	}

	void Print(const std::string &prefix = "") {
		while (is_writing.load() == 1) {
			usleep(1000 * 10);
		}

		for (int i = 0; i < size; i++) {
			net_infos[i].Print(prefix);
		}
	}
	void FillData() {
		while (1) {
			std::string cmd = "sar -n DEV 2 1| grep Average | awk '{if (NR>1) {print $2,$5,$6,$10}}'";
			std::vector<std::string> resvec;
			avos::common::Util::ExeLinuxCmd(cmd, resvec);
			if (resvec.size() == 0) {
				std::cout << "error exe " << cmd << std::endl;
				continue;
			}
			is_writing.store(1);
			size = resvec.size();
			for (int i = 0; i < resvec.size(); i++) {
				std::stringstream ss;
				ss << resvec[i];
				std::string name;
				ss >> name;
				net_infos[i].SetNetName(name);
				ss >> net_infos[i].rkbps;
				ss >> net_infos[i].wkbps;
				ss >> net_infos[i].usage_rate;
			}
			is_writing.store(0);
		}
		sleep(1);
	}
} ;



struct OSGlobalData
{
#define DATE_TIME_LEN 32
	std::atomic<int> is_writing;
	char date_time[DATE_TIME_LEN];
	double current_time;
	float temperature;
	float load_average;
	int context_switch;
	OSGlobalData() {
		is_writing.store(0);
		current_time = 0.0;
		temperature = 0.0;
		load_average = 0.0;
		context_switch = 0;
	}

	OSGlobalData(const OSGlobalData & c)
	{
		current_time = c.current_time;
		temperature = c.temperature;
		load_average = c.load_average;
		context_switch = c.context_switch;
	}

	OSGlobalData& operator= (const OSGlobalData& c) {
		while (is_writing.load() == 1) {
			usleep(1000 * 10);
		}
		current_time = c.current_time;
		temperature = c.temperature;
		load_average = c.load_average;
		context_switch = c.context_switch;
		return *this;
	}


	void SetDateTime(const std::string &str) {
		memset(date_time, 0, DATE_TIME_LEN);
		memcpy(date_time, str.c_str(), str.size() > (DATE_TIME_LEN - 1) ? (DATE_TIME_LEN - 1) : str.size());
	}

	std::string GetDateTime() {
		return date_time;
	}
	void Print(const std::string &prefix = "") {
		while (is_writing.load() == 1) {
			usleep(1000 * 10);
		}
		std::cout << prefix << "date_time:" << GetDateTime() << std::endl;
		std::cout << prefix << "current_time:" << std::setprecision(10)  << current_time << std::setprecision(5) << std::endl;
		std::cout << prefix << "temperature:" << temperature << std::endl;
		std::cout << prefix << "load_average:" << load_average << std::endl;
		std::cout << prefix << "context_switch:" << context_switch << std::endl;
	}

	void FillData() {
		while (1) {
			is_writing.store(1);
			SetDateTime(avos::common::NodeTime::Now().ToString());
			current_time = avos::common::NodeTime::Now().ToSecond();
			is_writing.store(0);
			// 1 load_average
			std::string cmd = "cat /proc/loadavg | awk '{print $1}'";
			std::vector<std::string> resvec;
			avos::common::Util::ExeLinuxCmd(cmd, resvec);
			if (resvec.size() != 1) {
				std::cout << "error exe " << cmd << std::endl;
				continue;
			}

			is_writing.store(1);
			std::stringstream load_average_ss;
			load_average_ss << resvec[0];
			load_average_ss >> load_average;
			is_writing.store(0);

			cmd = "sar -m TEMP 2 1 | grep Average | awk ' {print $3}'";
			avos::common::Util::ExeLinuxCmd(cmd, resvec);
			if (resvec.size() == 0) {
				std::cout << "error exe " << cmd << std::endl;
				continue;
			}
			is_writing.store(1);
			std::stringstream temprature_ss;
			temprature_ss << resvec[0];
			temprature_ss >> temperature;
			if (temperature < 10 && resvec.size() > 1) {

				std::stringstream temprature_ss;
				temprature_ss << resvec[1];
				temprature_ss >> temperature;
			}
			is_writing.store(0);


			cmd = "sar -w 2 1 | grep Average | awk '{print $3}'";
			avos::common::Util::ExeLinuxCmd(cmd, resvec);
			if (resvec.size() != 1) {
				std::cout << "error exe " << cmd << std::endl;
				continue;
			}
			is_writing.store(1);
			std::stringstream context_switch_ss;
			context_switch_ss << resvec[0];
			context_switch_ss >> context_switch;
			is_writing.store(0);


		}
	}
};


typedef struct OSResourceInfo
{
#define BOARD_NAME_SIZE 32
	char board_name[BOARD_NAME_SIZE];
	OSGlobalData global_info;
	OSMemInfo mem_info;
	OSCpuInfo cpu_info;
	OSGpuInfo gpu_info;
	DiskIOInfoList dis_io_info;
	NetInfoList net_info;
	OSResourceInfo() {
		memset(board_name, 0, BOARD_NAME_SIZE);
	}
	void SetbName(const std::string &str) {
		memset(board_name, 0, BOARD_NAME_SIZE);
		memcpy(board_name, str.c_str(), str.size() > (BOARD_NAME_SIZE - 1) ? (BOARD_NAME_SIZE - 1) : str.size());
	}

	void Print(const std::string &prefix = "") {
		std::cout << prefix << "-OSGlobalData:" << std::endl;
		global_info.Print(prefix + "\t");
		std::cout << prefix << "-NetInfoList:" << std::endl;
		net_info.Print(prefix + "\t");
		std::cout << prefix << "-DiskIOInfoList:" << std::endl;
		dis_io_info.Print(prefix + "\t");
		std::cout << prefix << "-OSMemInfo:" << std::endl;
		mem_info.Print(prefix + "\t");
		std::cout << prefix << "-OSGpuInfo:" << std::endl;
		gpu_info.Print(prefix + "\t");
		std::cout << prefix << "-OSCpuInfo:" << std::endl;
		cpu_info.Print(prefix + "\t");
	}
	void Init() {
		char* ros_host_name = getenv("ROS_HOSTNAME");
		if (ros_host_name)
		{
			SetbName(std::string(ros_host_name));
		} else {
			SetbName(std::string("127.0.0.1"));
		}
	}

	std::string GetModuleName() {
		return board_name;
	}
} OSResourceInfo;

typedef struct BoardResourceInfo
{
#define MAX_BOARD_NUM 10
	int size;
	OSResourceInfo os_resource_info[MAX_BOARD_NUM];
	BoardResourceInfo() {
		size = 0;
	}


	void Print(const std::string &prefix = "") {
		std::cout << "-OSResourceInfo:" << std::endl;
		for (int i = 0; i < size; i++) {
			os_resource_info[i].Print(prefix + "\t");
		}
	}

} BoardResourceInfo;







}
}



#endif

