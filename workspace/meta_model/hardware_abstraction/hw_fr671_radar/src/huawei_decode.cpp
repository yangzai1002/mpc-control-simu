#include <math.h>
#include "huawei_decode.h"

namespace avos {
namespace driver {
HWDecoder::HWDecoder()  {
	for (int i = 0; i < CULCULATE_TABLE_SIZE; i++) {
		sin_table[i] = sin(i / 180.0 / (CULCULATE_TABLE_SIZE / 360.0) * 3.1415926);
	}

	for (int i = 0; i < CULCULATE_TABLE_SIZE; i++) {
		cos_table[i] = cos(i / 180.0 / (CULCULATE_TABLE_SIZE / 360.0) * 3.1415926);
	}
	one_frame_.store(false);
}

HWDecoder::~HWDecoder() {

}

std::string HWDecoder::getInterfaceNameByIP(const std::string& targetIP) {
    struct ifaddrs *ifaddr, *ifa;
    char host[NI_MAXHOST];
    std::string interfaceName;

    if (getifaddrs(&ifaddr) == -1) {
        perror("getifaddrs");
        return "";
    }

    for (ifa = ifaddr; ifa != nullptr; ifa = ifa->ifa_next) {
        if (ifa->ifa_addr == nullptr) {
            continue;
        }

        int family = ifa->ifa_addr->sa_family;
        if (family == AF_INET || family == AF_INET6) {
            int s = getnameinfo(ifa->ifa_addr,
                                (family == AF_INET) ? sizeof(struct sockaddr_in) : sizeof(struct sockaddr_in6),
                                host, NI_MAXHOST, nullptr, 0, NI_NUMERICHOST);
            if (s != 0) {
                continue;
            }

            if (targetIP == host) {
                interfaceName = ifa->ifa_name;
                break;
            }
        }
    }

    freeifaddrs(ifaddr);
    return interfaceName;
}

bool HWDecoder::isIpReachable(const std::string& ip, int debug) {
    // 获取ip对应的网卡
    std::string net_card = getInterfaceNameByIP(ip);
	if(debug)
    	printf("ip %s's net_card is: %s\n", ip.c_str(), net_card.c_str());
    // 构造 ping 命令：-c 1 发送1个包，-W 1 超时1秒
    std::string cmd = "";
    if (net_card != "")
    {
        cmd = "ping -I " + net_card + " -c 1 -W 1 192.168.201.101 " + " > /dev/null 2>&1";
    }
    else
    {
        cmd = "ping -c 1 -W 1 192.168.201.101  > /dev/null 2>&1";
    }
	if(debug)
    	printf("ping cmd is: %s\n", cmd.c_str());
    // 执行命令，返回0表示成功（可达）
    int ret = system(cmd.c_str());
    return (ret == 0);
}

int HWDecoder::Init(const DecoderConfig &config, int sensor_id, int frame_interval_ms) {
	msop_buf.udp_packet.resize(UDP_BUFFER_SIZE);
	decoder_config = config;
	interval_ms_ = frame_interval_ms;
	sensor_id_ = sensor_id;
	return 0;
}

// void HWDecoder::SendCheckAliveMsg() {
// 	return;
// 	CheckAlive check_alive;
// 	check_alive_client_.SendData((char*)&check_alive, sizeof(CheckAlive));
// }

// void HWDecoder::SendLidarConfigMsg(int phase_lock)
// {

// }

// void HWDecoder::SendVehicleMsg()
// {

// }

bool HWDecoder::IsPktForOneFrame() {
	return one_frame_.load() == true ? true : false ;
}

void HWDecoder::ConvertPacktsToPoint(const ara::adsfi::CommonUdpPacket &packet, ara::adsfi::SensorRadarPointCloud &point_cloud, uint64_t &first_pkg_ts, int &pub_logic) {
	//pub_logic: 0 not fill not pub; 1 first pub then fill cloud poind; 2 only fill cloud poind; 3 fill cloud poind then pub
	pub_logic = -1;
	RadarPointHeader  *header = (RadarPointHeader *)(&(packet.udp_packet[0]));
	// parse header
	if(header->header.isHeader())
	{
		point_cloud.objs.clear();
		if(decoder_config.debug & 64)
		{
			std::stringstream its_message;
			its_message << "udp buf size:[" << std::dec << sizeof(RadarPointHeader) << "], header data:\n ";
			for (uint32_t i = 0; i < sizeof(RadarPointHeader); ++i)
				its_message << std::hex << std::setw(2) << std::setfill('0')
							<< (int) (packet.udp_packet)[i] << " ";
			std::cout << its_message.str() << "\n";
		}

		timeval tv{};
		gettimeofday(&tv, nullptr);
		long long cur_ts = tv.tv_sec*1000 + tv.tv_usec / 1000;
		// if(abs(cur_ts-header->frame_ts_ms())>500)//500ms
		// {
		// 	AERROR << sensor_id_ << " radar time sync failed, cur_ts[" << cur_ts << "], radar ts[" << header->frame_ts_ms() << "] ms\n";
		// 	ec_add("1091"+std::to_string(sensor_id_)+"04");
		// }
		// else
		// {
		// 	ec_remove("1091"+std::to_string(sensor_id_)+"04");
		// }

		if(0 != frame_ts_)
		{
			int interval = abs(header->frame_ts_ms() - frame_ts_);

			if(interval>(interval_ms_*1.5))
			{
				printf("   radar frame interval invalid[defalt %d ms], last ts %.3f, current ts %.3f, interval_ms %d\n",
					interval_ms_, frame_ts_/1000.0, header->frame_ts_ms()%1000, interval);
			}
			if(header->frame_ts_ms() < frame_ts_)
			{
				printf("   radar time back to old, last ts %.3f, current ts %.3f, interval_ms %d\n",
					frame_ts_/1000.0, header->frame_ts_ms()%1000, -interval);
				ec_add("1072008", "radar time back to old", "input");
			}
			else
			{
				ec_remove("1072008");
			}
		}

		frame_ts_ = header->frame_ts_ms();

		if(header->frame_id() != frame_id_)//new frame
		{
			if(decoder_config.debug & 2)
			{
				printf("\n/***********************************/\n   frame_id_ is: %d, udp_pkg_num_ is: %d, obj_num_ is: %d, ts: %.3f\n",
						header->frame_id(), header->pkg_num(), header->obj_num(), frame_ts_/1000.0);
			}

			if(-1==frame_id_)//first frame
			{
				if(decoder_config.debug & 2)
				{
					std::cout << "   first frame" << "\n";
				}

				is_new = true;
				udp_pkg_count_ = 0;
				udp_pkg_num_ = header->pkg_num();
				obj_num_ = header->obj_num();
				first_pkg_ts = header->frame_ts_ms(); //ms
				frame_id_ = header->frame_id() % 65536;
				pub_logic = 0;
			}
			else //not first frame
			{
				if((((frame_id_+1)%65536) != header->frame_id())&&(-1!=frame_id_))
				{
					std::cout << "   lost frame, last:current -> " << frame_id_ << ":" <<   header->frame_id() << "\n";
					ec_add("1072007", "lost frame", "input");
				}
				else
				{
					ec_remove("1072007");
				}
				if(!is_new)//finish last frame process
				{
					is_new = true;
					udp_pkg_count_ = 0;
					udp_pkg_num_ = header->pkg_num();
					obj_num_ = header->obj_num();
					first_pkg_ts = header->frame_ts_ms(); //ms
					frame_id_ = header->frame_id() % 65536;
					pub_logic = 0;
				}
				else //not finish last frame process
				{
					if(decoder_config.debug & 2)
					{
						std::cout << "   receive a new frame , while not finish receive last frame's packets" << "\n";
					}
					is_new = true;
					udp_pkg_count_ = 0;
					udp_pkg_num_ = header->pkg_num();
					obj_num_ = header->obj_num();
					first_pkg_ts = header->frame_ts_ms(); //ms
					frame_id_ = header->frame_id() % 65536;
					pub_logic = 1;
				}
			}
			if(1 == header->block_flag)
			{
				// printf("-------------------- there is block! --------------------\n");
				ec_add("1072005", "radar block", "parse", 3);
			}
			else
			{
				ec_remove("1072005", 3);
			}
		}
		return;
	}

	RadarPointContent  *content = (RadarPointContent *)(&(packet.udp_packet[0]));
	// parse content
	if(content->header.isContent())
	{
		if(decoder_config.debug & 64)
		{
			std::stringstream its_message;
			its_message << "udp buf size:[" << std::dec << sizeof(RadarPointContent) << "], content data:\n ";
			for (uint32_t i = 0; i < sizeof(RadarPointContent); ++i)
				its_message << std::hex << std::setw(2) << std::setfill('0')
							<< (int) (packet.udp_packet)[i] << " ";
			std::cout << its_message.str() << "\n";
		}

		if(decoder_config.debug & 2)
		{
			printf("   recv frame[%d]'s content, pkg id is: %d, obj_num_in_pkg_ is: %d\n",
					frame_id_, content->GetPackageId(), content->GetPackageClusterNum());
		}

		if((1+udp_pkg_id_) != content->GetPackageId())
		{
			ec_add("1072006", "lost udp packet", "input", 10);
			if(decoder_config.debug & 2)
				printf("   frame %d lost udp packet, last %d, current %d \n", frame_id_, udp_pkg_id_, content->GetPackageId());
		}
		else
		{
			ec_remove("1072006", 10);
		}
		udp_pkg_count_ ++;
		udp_pkg_id_ = content->GetPackageId();
		obj_num_in_pkg_ = content->GetPackageClusterNum();
		obj_recv_sum_+=obj_num_in_pkg_;

		if(is_new&&(udp_pkg_num_ > content->GetPackageId()))
		{
			pub_logic = 2;
		}
		else if(is_new&&(udp_pkg_num_ == content->GetPackageId())){
			if(decoder_config.debug & 2)
			{
				std::cout << "   receive end packet\n";
			}
			if(content->GetPackageId() != udp_pkg_count_)
			{
				printf("   recv end pkg, pkg recv sum is: %d, less than pkg num: %d\n", udp_pkg_count_, udp_pkg_num_);
			}
			if(obj_recv_sum_ != obj_num_)
			{
				printf("   recv end pkg, obj recv sum is: %d, less than obj num: %d\n", obj_recv_sum_, obj_num_);
			}
			pub_logic = 3;
			is_new = false;
			udp_pkg_count_ = 0;
			udp_pkg_id_ = 0;
			obj_num_ = -1;
			obj_recv_sum_ = 0;
		}

		// point_cloud.points.reserve(50);
		point_cloud.objs.clear();


		double t = frame_ts_/1000.0;

		std::cout << "frame_ts_:"<<frame_ts_<<","<< t<<std::endl;
		point_cloud.header.time.sec = ADSFI_GET_SEC(t);
		point_cloud.header.time.nsec = ADSFI_GET_NSEC(t);
		for(int j=0; j< content->GetPackageClusterNum(); j++)
		{
			/*
				x=rcos(a)cos(b)
				y=rcos(a)sin(b)
				z=rsin(a)
				r为距离，a为俯仰角，b为方位角
			*/
			point.id = content->cluster_info[j].GetClusterId();
			point.motionStatus = content->cluster_info[j].motion_state;
			point.azimuth = content->cluster_info[j].GetAzimuth();
			point.elevation = content->cluster_info[j].GetElevation();
			point.range = content->cluster_info[j].GetRange();
			point.range_rate = content->cluster_info[j].GetRangeRate();

			point.rangeStdDev = other_std[content->cluster_info[j].range_std_dev];
			point.rangeRateStdDev = other_std[content->cluster_info[j].range_rate_std_dev];
			point.azimuthStdDev = angle_std[content->cluster_info[j].azimuth_std_dev];
			point.elevationStdDev = angle_std[content->cluster_info[j].elevation_std_dev];

			point.existStatus  = content->cluster_info[j].exist_status;
			point.falseAlarmProb  = content->cluster_info[j].false_alarm_prob;
			point.heightConfidence = content->cluster_info[j].height_confidence;
			point.snr = content->cluster_info[j].snr;
			point.attribState = content->cluster_info[j].attrib_state;
			point.ambg_state = content->cluster_info[j].ambg_state;
			point.modeFlag = content->cluster_info[j].mode_flag;
			if(decoder_config.debug & 128)
				printf("   id: %d, motionStatus: %d, existStatus: %d, falseAlarmProb: %d, confidence: %d, modeFlag:%d, "
						"range: %.3f, range_rate: %.3f, azimuth: %.3f, elevation: %.3f\n",
						point.id, point.motionStatus, point.existStatus, point.falseAlarmProb, point.heightConfidence, point.modeFlag,
						content->cluster_info[j].GetRange(), content->cluster_info[j].GetRangeRate(), content->cluster_info[j].GetAzimuth(), content->cluster_info[j].GetElevation());
			// point.x  = content->cluster_info[j].GetRange()*GetActCosValue(content->cluster_info[j].GetElevation())*GetActCosValue(content->cluster_info[j].GetAzimuth());
			// point.y  = content->cluster_info[j].GetRange()*GetActCosValue(content->cluster_info[j].GetElevation())*GetActSinValue(content->cluster_info[j].GetAzimuth());
			// point.z  = content->cluster_info[j].GetRange()*GetActSinValue(content->cluster_info[j].GetElevation());
			// point.vx  = content->cluster_info[j].GetRangeRate()*GetActCosValue(content->cluster_info[j].GetElevation())*GetActCosValue(content->cluster_info[j].GetAzimuth());
			// point.vy  = content->cluster_info[j].GetRangeRate()*GetActCosValue(content->cluster_info[j].GetElevation())*GetActSinValue(content->cluster_info[j].GetAzimuth());
			// point.vz  = content->cluster_info[j].GetRangeRate()*GetActSinValue(content->cluster_info[j].GetElevation());

			content->cluster_info[j].GetObjectData(point.x, point.y, point.z, point.vx, point.vy, point.vz);

			if(decoder_config.debug & 256)
				printf("   id: %d, motionStatus: %d,   x: %.3f, y: %.3f, z:%.3f,   vx: %.3f, vy: %.3f, vz:%.3f\n",
						point.id, point.motionStatus, point.x, point.y, point.z, point.vx, point.vy, point.vz);

			// calibration
			ConvertPoint(point, decoder_config);
			if(decoder_config.debug & 1)
			{
				if(1 == point.motionStatus)
				{
					point_cloud.objs.push_back(point);
					if(decoder_config.debug & 512)
						printf("   id: %d, motionStatus: %d, existStatus: %d, falseAlarmProb: %d, confidence: %d, modeFlag:%d, "
							"range: %.3f, range_rate: %.3f, azimuth: %.3f, elevation: %.3f\n",
							point.id, point.motionStatus, point.existStatus, point.falseAlarmProb, point.heightConfidence, point.modeFlag,
							content->cluster_info[j].GetRange(), content->cluster_info[j].GetRangeRate(), content->cluster_info[j].GetAzimuth(), content->cluster_info[j].GetElevation());
				}
			}
			else
			{
				point_cloud.objs.push_back(point);
			}
		}
	}
}
}
}


