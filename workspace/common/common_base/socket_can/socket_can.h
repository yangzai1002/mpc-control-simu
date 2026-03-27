#ifndef __SOCKET_CAN_TANSMITTER_H__
#define __SOCKET_CAN_TANSMITTER_H__
// linux base header
#include <sys/types.h>
#include <sys/socket.h>
#include <linux/can.h>
#include <linux/can/raw.h>
#include <linux/can/error.h>
#include <errno.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <iostream>
#include <vector>
#include <memory.h>
#include <math.h>
#include <string>

namespace avos {
namespace driver {

struct CANFrame {
	int id;
	double time_stamp;
	int remote_flag;
	int extend_flag;
	int data_len;
	uint8_t data[8];
	CANFrame() {
		id = -1;
		time_stamp = 0.0;
		remote_flag = 0;
		extend_flag = 0;
		data_len = 0;
		memset(data, 0, 8);
	}
	void print(){
		std::cout << "ID:" << std::hex << id << "\t" 
							<< data[0] << " " << data[1] << " " << data[2] << " " << data[3] << " " 
							<< data[4] << " " << data[5] << " " << data[6] << " " << data[7] << std::dec << std::endl;
	}
};


class SocketCANTool {
public:
	SocketCANTool();
	~SocketCANTool() {}

	bool InitDevice(std::string can_channel, std::vector<canid_t> &can_ids);
	int ReadCanFrame(CANFrame &can_frame);
	int WriteCanFrame(const CANFrame &can_frame);

private:
	int socketcan_fd_;
	struct timeval CanTimeout;
	fd_set rfd;
};


}
}

#endif
