#include <math.h>

#include "huawei_decode.h"



namespace avos {
namespace driver {

uint32_t ReflectedData(uint32_t data)
{
	data = ((data & 0xffff0000) >> 16) | ((data & 0x0000ffff) << 16);
	data = ((data & 0xff00ff00) >> 8) | ((data & 0x00ff00ff) << 8);
	data = ((data & 0xf0f0f0f0) >> 4) | ((data & 0x0f0f0f0f) << 4);
	data = ((data & 0xcccccccc) >> 2) | ((data & 0x33333333) << 2);
	data = ((data & 0xaaaaaaaa) >> 1) | ((data & 0x55555555) << 1);

	return (data >> 24) & 0xff;
}


uint32_t ReflectedData32(uint32_t data)
{
	data = ((data & 0xffff0000) >> 16) | ((data & 0x0000ffff) << 16);
	data = ((data & 0xff00ff00) >> 8) | ((data & 0x00ff00ff) << 8);
	data = ((data & 0xf0f0f0f0) >> 4) | ((data & 0x0f0f0f0f) << 4);
	data = ((data & 0xcccccccc) >> 2) | ((data & 0x33333333) << 2);
	data = ((data & 0xaaaaaaaa) >> 1) | ((data & 0x55555555) << 1);

	return data;
}



uint32_t CheckCrc32(uint32_t poly, uint32_t init, bool refIn, bool refOut, uint32_t xorOut,
                    const uint8_t *buffer, uint32_t length)
{
	uint32_t i = 0;
	uint32_t crc = init;

	while (length--)
	{
		if (refIn == true)
		{
			crc ^= ReflectedData(*buffer++) << 24;
		}
		else
		{
			crc ^= (*buffer++) << 24;
		}

		for (i = 0; i < 8; i++)
		{
			if (crc & 0x80000000)
			{
				crc <<= 1;
				crc ^= poly;
			}
			else
			{
				crc <<= 1;
			}
		}
	}

	if (refOut == true)
	{
		crc = ReflectedData32(crc);
	}

	return crc ^ xorOut;
}


HWDecoder::HWDecoder()  {
	for (int i = 0; i < CULCULATE_TABLE_SIZE; i++) {
		sin_table[i] = sin(i / 180.0 / (CULCULATE_TABLE_SIZE / 360.0) * 3.1415926);
	}

	for (int i = 0; i < CULCULATE_TABLE_SIZE; i++) {
		cos_table[i] = cos(i / 180.0 / (CULCULATE_TABLE_SIZE / 360.0) * 3.1415926);
	}

	AINFO << "HWDecoder";
	memset((char *)vertical_angle_, 0, sizeof(int) * MAX_LIDAR_RING);
	memset((char *)vertical_angle_offset_, 0, sizeof(int) * 480 * 4);
	memset((char *)horizontal_angle_offset_, 0, sizeof(int) * MAX_LIDAR_RING * 4);

	vec_data_id_.clear();
	data_id_flag_.resize(9, false);

	vec_data_id_.push_back(std::make_pair(0x0001, 0x667c1ae0));
	vec_data_id_.push_back(std::make_pair(0x0010, 0x06f383a2));
	vec_data_id_.push_back(std::make_pair(0x0011, 0x4089a102));
	vec_data_id_.push_back(std::make_pair(0x0012, 0x8cc49379));
	vec_data_id_.push_back(std::make_pair(0x0013, 0xcabeb1d9));
	vec_data_id_.push_back(std::make_pair(0x0020, 0x08320075));
	vec_data_id_.push_back(std::make_pair(0x0021, 0x4e4822d5));
	vec_data_id_.push_back(std::make_pair(0x0022, 0x8f83bb98));
	vec_data_id_.push_back(std::make_pair(0x0023, 0xc9f99938));

	one_frame_.store(false);

}

HWDecoder::~HWDecoder() {

}

int HWDecoder::Init(const DecoderConfig &config) {
	difop_buf.udp_packet.resize(sizeof(SupplementaryData));
	msop_buf.udp_packet.resize(UDP_BUFFER_SIZE);
	decoder_config = config;
	return 0;
}

void HWDecoder::SetDifopConfig(const avos::SocketConfig &socket_config) {
	current_socket_config = socket_config;
	if (supple_info_client_.Init(current_socket_config) < 0) {
		std::cout << "udp server is error, supple info client" << std::endl;
	}
}

int HWDecoder::GetSupplementaryData(ara::adsfi::CommonUdpPacket &packets_ptr) {
	// if (packets_ptr.get() == nullptr) {
	// 	return -1;
	// }
	SupplementaryData supple_data;

	for (int i = 0; i < MAX_LIDAR_RING; i++) {
		supple_data.vertical_angle[i] = vertical_angle_[i];
	}

	for (int i = 0; i < 480; i++) {
		for (int j = 0; j < 4; j++) {
			supple_data.vertical_angle_offset[i][j] = vertical_angle_offset_[i][j];
		}
	}

	for (int i = 0; i < MAX_LIDAR_RING; i++) {
		for (int j = 0; j < 4; j++) {
			supple_data.horizontal_angle_offset[i][j] = horizontal_angle_offset_[i][j];
		}
	}
	memcpy(&(difop_buf.udp_packet[0]), &supple_data, sizeof(supple_data));
	packets_ptr = difop_buf;

	return 0;
}
int HWDecoder::SetSupplementaryData(const ara::adsfi::SensorLidarPacket &packets_ptr, int debug) {
	// if (packets_ptr.get() == nullptr) {
	// 	return -1;
	// }
	if (packets_ptr.difop_packet.udp_packet.size() != sizeof(SupplementaryData))
	{
		printf("error: invalid difop_packet size[%d], unqual SupplementaryData's size[%d]\n", packets_ptr.difop_packet.udp_packet.size(), sizeof(SupplementaryData));
		// exit(1);
	}
	// SupplementaryData *supple_data = (SupplementaryData *)(&(packets_ptr.difop_packet.udp_packet[0]));
	SupplementaryData supple_data;
	memcpy(&supple_data, &(packets_ptr.difop_packet.udp_packet[0]), sizeof(SupplementaryData));

	for (int i = 0; i < MAX_LIDAR_RING; i++) {
		vertical_angle_[i] = supple_data.vertical_angle[i];
		if(debug)
		{
			printf("   vertical_angle_[%d] is: %d; ", i, vertical_angle_[i]);
			if (0 == (i % 10))
			{
				printf("\n");
			}
		}
	}
	printf("\n");

	for (int j = 0; j < 4; j++) {
		for (int i = 0; i < 480; i++) {
			vertical_angle_offset_[i][j] = supple_data.vertical_angle_offset[i][j];
			if(debug)
			{
				printf("   vertical_angle_offset_[%d][%d] is: %d; ", i, j, vertical_angle_offset_[i][j]);
				if (0 == (i % 10))
				{
					printf("\n");
				}
			}
		}
		printf("\n");
	}

	for (int j = 0; j < 4; j++) {
		for (int i = 0; i < MAX_LIDAR_RING; i++) {
			horizontal_angle_offset_[i][j] = supple_data.horizontal_angle_offset[i][j];
			if(debug)
			{
				printf("   horizontal_angle_offset_[%d][%d] is: %d; ", i, j, horizontal_angle_offset_[i][j]);
				if (0 == (i % 10))
				{
					printf("\n");
				}
			}
		}
		printf("\n");
	}

	return 0;
}

bool HWDecoder::IsSupplementaryInformatFull() {
	for (int i = 0; i < data_id_flag_.size(); i++) {
		if (data_id_flag_[i] == false) {
			return false;
		}
	}
	return true;
}

int HWDecoder::SendAllSupplementaryInformation() {
	for (unsigned short i = 0; i < vec_data_id_.size(); i++) {

		if (data_id_flag_[i] == false) {
			printf("send supplementary information. data id: %04x.\n", vec_data_id_[i].first);
			SendSupplementaryInformation(vec_data_id_[i].first, vec_data_id_[i].second, i);

			// recv reply
			std::cout << "ready to recv" << "\n";
			memset(supplementary_buf, 0, SUPPLE_BUFFER_SIZE);
			int rev_num = ReceiveSuppInfo(supplementary_buf, SUPPLE_BUFFER_SIZE);
			std::cout << "IsSupplementaryInformatFull rev_num: " << rev_num << "\n";
			GetPacketAngle(supplementary_buf);
		}
	}

	return 0;
}

void HWDecoder::SendCheckAliveMsg() {
	return;
	CheckAlive check_alive;
	check_alive_client_.SendData((char*)&check_alive, sizeof(CheckAlive));
}

void HWDecoder::SendLidarConfigMsg(int phase_lock) {
	LidarConfig lidar_config;
	unsigned short phase_lock_data = phase_lock;
	lidar_config.phase_lock = REVERSE_SHORT(phase_lock_data);
	const uint32_t polynomial = 0xF4ACFB13;
	const int width = 32;
	lidar_config.e2e_check.checksum =  REVERSE_INT(CheckCrc32(polynomial, 0xFFFFFFFF, true, true, 0xFFFFFFFF,
	                                   (const uint8_t *)&lidar_config.message_id, sizeof(LidarConfig) - 4));
	// check_alive_client_.SendData((char*)&lidar_config, sizeof(lidar_config));
	supple_info_client_.SendData((char*)&lidar_config, sizeof(lidar_config));
}

void HWDecoder::SendSupplementaryInformation(unsigned short data_id, unsigned int check_sum, unsigned short index) {
	SuppleInfo supple_info;
	supple_info.data_id = REVERSE_SHORT(data_id);
	if (index == 0)
		supple_info.e2e_check.counter = REVERSE_SHORT(index);
	else {
		supple_info.e2e_check.counter = REVERSE_SHORT(index + 1);
	}
	supple_info.e2e_check.checksum = REVERSE_INT(check_sum);

	unsigned char *out = (unsigned char*)&supple_info;
	// for (int i = 0; i < sizeof(SuppleInfo); i++)
	// {
	// 	printf(" %x", out[i]);
	// }
	// printf("\n");
	supple_info_client_.SendData((char*)&supple_info, sizeof(SuppleInfo));
}

int HWDecoder::ReceiveSuppInfo(char* data, int max_len) {
	int nRet = -1;
	nRet = supple_info_client_.RecieveData(data, max_len);
	return nRet;
}


int HWDecoder::GetFOVVerticalAngle(VerticalAngle* pkt) {
	if (pkt == nullptr) {
		return -1;
	}
	for (size_t i = 0; i < MAX_LIDAR_RING; i++)
	{
		vertical_angle_[i] = pkt->Angle(i) * (100 / 256.0);
		if ((!data_id_flag_[0])&&decoder_config.decoder_debug)
		{
			printf("   vertical_angle_[%d] is: %d; ", i, vertical_angle_[i]);
			if (0 == (i % 10))
			{
				printf("\n");
			}
		}

	}
	data_id_flag_[0] = true;
	printf("\n");
	return 0;
}

int  HWDecoder::GetVerticalAngleOffset(VerticalAngleOffset* pkt, int index) {
	if (pkt == nullptr || index < 0 || index > 3) {
		return -1;
	}
	for (size_t i = 0; i < 480; i++)
	{
		vertical_angle_offset_[i][index] = pkt->Angle(i) * 0.1;//0.001
		if ((!data_id_flag_[index + 1])&&decoder_config.decoder_debug)
		{
			printf("   vertical_angle_offset_[%d][%d] is: %d; ", i, index, vertical_angle_offset_[i][index]);
			if (0 == (i % 10))
			{
				printf("\n");
			}
		}
	}
	data_id_flag_[index + 1] = true;
	printf("\n");
	return 0;
}

int HWDecoder::GetHorizontalAngleOffset(HorizontalAngleOffset* pkt, int index) {
	if (pkt == nullptr || index < 0 || index > 3) {
		return -1;
	}
	for (size_t i = 0; i < MAX_LIDAR_RING; i++)
	{
		horizontal_angle_offset_[i][index] = pkt->Angle(i) * 0.1; // 0.001
		if ((!data_id_flag_[index + 5])&&decoder_config.decoder_debug)
		{
			printf("   horizontal_angle_offset_[%d][%d] is: %d; ", i, index, horizontal_angle_offset_[i][index]);
			if (0 == (i % 10))
			{
				printf("\n");
			}
		}
	}
	data_id_flag_[index + 5] = true;
	printf("\n");
	return 0;
}

int HWDecoder::GetPacketAngle(const char *packet) {
	PointcloudCtrl *packet_angle = (PointcloudCtrl *)packet;
	if (packet_angle->msg_head.GetID() != 0x0031 ||
	        packet_angle->msg_head.GetType() != 0x80) {
		std::cout << "Response msg is error..." << std::endl;
		return -1;
	}
	printf("Receive supplementary information. data id: %04x.\n", packet_angle->pay_load.GetID());
	int index = packet_angle->pay_load.GetID() & 0x0F;
	std::cout << "Receive index: " << index << std::endl;
	int nRet = -1;
	switch (packet_angle->pay_load.GetID())
	{
	case 0x0001:
		nRet = GetFOVVerticalAngle(&packet_angle->pay_load.DataRecord.vertical_angle);
		break;
	case 0x0010:
	case 0x0011:
	case 0x0012:
	case 0x0013:
		nRet = GetVerticalAngleOffset(&packet_angle->pay_load.DataRecord.vertical_angle_offset, index);
		break;
	case 0x0020:
	case 0x0021:
	case 0x0022:
	case 0x0023:
		nRet = GetHorizontalAngleOffset(&packet_angle->pay_load.DataRecord.horizontal_angle_offset, index);
		break;
	default:
		break;
	}
	return nRet;
}

bool HWDecoder::IsPktForOneFrame() {
	return one_frame_.load() == true ? true : false ;
}

// convert avos huawei_packtes.udp packet to cloud
void HWDecoder::ConvertHuaweiPacktsToPoint(const ara::adsfi::CommonUdpPacket &packet, ara::adsfi::MsgLidarFrame &point_cloud, double header_ts) {
	HWPointCloudProtocol *pkt = (HWPointCloudProtocol *)(&(packet.udp_packet[0]));
	if (decoder_config.print_data)
	{
		std::stringstream its_message;
		its_message << "udp buf size:[" << std::dec << UDP_BUFFER_SIZE << "], data:\n ";
		for (uint32_t i = 0; i < UDP_BUFFER_SIZE; ++i)
			its_message << std::hex << std::setw(2) << std::setfill('0')
			            << (int) (packet.udp_packet)[i] << " ";
		std::cout << its_message.str() << "\n";
	}

	point_cloud.pointCloud.clear();
	double time_stamp = pkt->data_header.utc_time.GetSecondValue();
	// point_cloud.timestamp.sec = ADSFI_GET_SEC(time_stamp);
	// point_cloud.timestamp.nsec = ADSFI_GET_NSEC(time_stamp);
	// point_cloud.header.time = time_stamp;

	if (decoder_config.decoder_debug)
	{
		printf("   FrameID %d, PkgID %d, PkgPosition %d, ts %.4f\n", pkt->data_header.FrameID(),
		       pkt->data_header.PackageSeqNum(), pkt->data_header.PkgPosition(), time_stamp);
	}
	unsigned char LUTIndex = pkt->data_header.lut_index;
	unsigned short Column_ID = pkt->data_header.AzimuthIDX0();//pkt->data_header.azimuth_idx0;

	point_cloud.pointCloud.reserve(MAX_LIDAR_RING + 1);
	// static VPoint point;
	for (int j = 0; j < pkt->data_header.get_DataBlockNum(); j++) // 2 DataBlock
	{
		int Azimuth = pkt->data_block[j].Azimuth() * (100 / 256.0);
		if (Column_ID >= 480 || LUTIndex > 3 ) {
			std::cout << "*********error Column_ID || LUTIndex*************************" << "\n";
			return;
		}
		for (int i = 0; i < MAX_LIDAR_RING; i++) {

			static int horizontal_angle = 0;
			horizontal_angle = (Azimuth + horizontal_angle_offset_[i][LUTIndex] + MAX_ANGLE) % MAX_ANGLE;
			static int vertical_angle = 0;
			vertical_angle = (vertical_angle_[i] + vertical_angle_offset_[Column_ID][LUTIndex] + MAX_ANGLE) % MAX_ANGLE;;
			static float distance = 0;
			distance = pkt->data_block[j].channel_data[i].Distance() * (1 / 256.0);
			point.latency = (time_stamp-header_ts)*1e9; //nanosecond
			point.angle = horizontal_angle;
			point.ring = i;
			if (distance < 255) {
				point.x = distance * GetActCosValue(vertical_angle) * GetActCosValue(horizontal_angle);
				point.y = distance * GetActCosValue(vertical_angle) * GetActSinValue(horizontal_angle);
				point.z = distance * GetActSinValue(vertical_angle) ; ;
				point.intensity = pkt->data_block[j].channel_data[i].reflectivity;;
			} else {
				point.x = 0.0;
				point.y = 0.0;
				point.z = 0.0;
				point.intensity = 0;
			}
			// calibration
			ConvertPoint(point, decoder_config);
			// remove noise point
			if (point.x < decoder_config.car_modle_front_x &&
			        point.x > decoder_config.car_modle_back_x  &&
			        point.y < decoder_config.car_modle_left_y &&
			        point.y > decoder_config.car_modle_right_y &&
			        point.z < decoder_config.car_modle_top_z &&
			        point.z > decoder_config.car_modle_button_z  ) {
				point.x = 0.0;
				point.y = 0.0;
				point.z = 0.0;
				point.intensity = 0;
			}
			point_cloud.pointCloud.push_back(point);
		}
	}
}
}
}


