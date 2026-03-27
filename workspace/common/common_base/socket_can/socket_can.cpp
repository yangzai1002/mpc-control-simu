#include "socket_can/socket_can.h"

namespace avos {
namespace driver {

SocketCANTool::SocketCANTool() {}

bool SocketCANTool::InitDevice(std::string can_channel,
                               std::vector<canid_t> &can_ids) {
	std::cout << "CANBUS initdevice" << std::endl;
	sockaddr_can addr;
	ifreq ifr;
	socketcan_fd_ = socket(PF_CAN, SOCK_RAW, CAN_RAW);
	if (socketcan_fd_ < 0) {
		perror("scoket_can--> socket:");
		exit(-1);
	}
	strcpy(ifr.ifr_name, can_channel.c_str());
	ioctl(socketcan_fd_, SIOCGIFINDEX, &ifr);
	addr.can_family = AF_CAN;
	addr.can_ifindex = ifr.ifr_ifindex;
	if (bind(socketcan_fd_, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
		perror("scoket_can--> bind:");
		exit(-1);
	}

	size_t len = can_ids.size();
	struct can_filter rfilter[len];
	for (int i = 0; i < len; ++i) {
		rfilter[i].can_id = can_ids[i];
		rfilter[i].can_mask = CAN_SFF_MASK;
	}
	setsockopt(socketcan_fd_, SOL_CAN_RAW, CAN_RAW_FILTER, &rfilter, sizeof(rfilter));
	can_err_mask_t err_mask =  CAN_ERR_MASK;
	setsockopt(socketcan_fd_, SOL_CAN_RAW, CAN_RAW_ERR_FILTER, &err_mask, sizeof(err_mask));
	std::cout << "CANBUS init return true" << std::endl;
	return true;
}

int SocketCANTool::ReadCanFrame(CANFrame &can_frame_rev) {
	struct can_frame frame;

	FD_ZERO(&rfd);
	FD_SET(socketcan_fd_, &rfd);
	CanTimeout.tv_sec = 0;
	CanTimeout.tv_usec = 50000;
	if (select(socketcan_fd_ + 1, &rfd, 0, 0, &CanTimeout) == 0) {
		return 77;
	}

	memset(&frame, 0, sizeof(frame));
	int nbytes = read(socketcan_fd_, &frame, sizeof(frame));
	if (nbytes < 0) {
		fprintf(stderr, "can't read socketcan data\n");
		return -1;
	}
	if (nbytes < sizeof(struct can_frame)) {
		fprintf(stderr, "socketcan read: incomplete CAN frame\n");
		return -1;
	}

	if ((frame.can_id & CAN_ERR_FLAG) != 0) {
		std::cout << "CAN_ERR_FLAG" << frame.can_id << std::endl;
		if (frame.can_id & CAN_ERR_BUSOFF) {
			std::cout << "CANBUS CAN_ID" << frame.can_id << std::endl;
			std::cout << "Coming in here" << std::endl;
			return -2;
		}
	}

	can_frame_rev.id = frame.can_id;
	can_frame_rev.data_len = frame.can_dlc;
	memcpy(can_frame_rev.data, frame.data, 8);
	return 0;
}


int SocketCANTool::WriteCanFrame(const CANFrame &can_frame_send) {
	struct can_frame frame;
	memset(&frame, 0, sizeof(frame));
	frame.can_id = can_frame_send.id;
	frame.can_dlc = can_frame_send.data_len;
	memcpy(frame.data, can_frame_send.data , 8);
	int nbytes = write(socketcan_fd_, &frame, sizeof(frame));
	if (nbytes < 0) {
		return -1;
	}
	return 0;
}


}
}
