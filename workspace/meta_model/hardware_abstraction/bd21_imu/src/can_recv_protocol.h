#ifndef __CANRECV_PROTOCOL_H__
#define __CANRECV_PROTOCOL_H__

#include <algorithm>
#include <vector>
#include <iostream>
#include <iomanip>


#define ACC_PARAM 0.0001220703125
#define GYRO_PARAM 0.0076293
#define ANGLGE_PARAM 0.010986
#define HEIGHT_PARAM 0.001
#define SPEED_PARAM 0.0030517
#define GRAVITY_ACC 9.80665

#define RATIO 10.6533
#define RADIUS 0.2608
#define WHEEL_SPEED_OFFSET (-15000)
#define U_PI 3.1415926

namespace avos
{
namespace BD21
{
    typedef struct InsTx1 {
        float ins_acc_x;
        float ins_acc_y;
        float ins_acc_z;
        int inss_gnssflag_heading;
        int inss_gnssflag_pos;
        InsTx1() {
            ins_acc_x = 0.0;
            ins_acc_y = 0.0;
            ins_acc_z = 0;
            inss_gnssflag_heading = 0;
            inss_gnssflag_pos = 0;

        }


        int GetFromCanFrame(const char *data, int len) {
            if (len != 8)
                return -1;
            ins_acc_x = GRAVITY_ACC*((((unsigned short)data[0] << 8) + (unsigned char)data[1]) * ACC_PARAM  - 4);
            ins_acc_y = GRAVITY_ACC*((((unsigned short)data[2] << 8) + (unsigned char)data[3]) * ACC_PARAM  - 4);
            ins_acc_z = GRAVITY_ACC*((((unsigned short)data[4] << 8) + (unsigned char)data[5]) * ACC_PARAM  - 4);

            inss_gnssflag_heading = data[6];
            inss_gnssflag_pos = data[7];
            return 0;
        }

        void Print(const std::string &prefix) {
            std::cout << prefix << "\n";
            std::cout << "\t\t" << "ins_acc_x:" << ins_acc_x << "\n";
            std::cout << "\t\t" << "ins_acc_y:" << ins_acc_y << "\n";
            std::cout << "\t\t" << "ins_acc_z:" << ins_acc_z << "\n";
            std::cout << "\t\t" << "inss_gnssflag_heading:" << inss_gnssflag_heading << "\n";
            std::cout << "\t\t" << "inss_gnssflag_pos:" << inss_gnssflag_pos << "\n";
        }

    } InsTx1;


    typedef struct InsTx2 {
        float raw_acc_x;
        float raw_acc_y;
        float raw_acc_z;
        int ins_gnss_age;
        int ins_num_sv;
        InsTx2() {
            raw_acc_x = 0.0;
            raw_acc_y = 0.0;
            raw_acc_z = 0;
            ins_gnss_age = 0;
            ins_num_sv = 0;

        }


        int GetFromCanFrame(const char *data, int len) {
            if (len != 8)
                return -1;
            raw_acc_x = GRAVITY_ACC*((((unsigned short)data[0] << 8) + (unsigned char)data[1]) * ACC_PARAM  - 4);
            raw_acc_y = GRAVITY_ACC*((((unsigned short)data[2] << 8) + (unsigned char)data[3]) * ACC_PARAM  - 4);
            raw_acc_z = GRAVITY_ACC*((((unsigned short)data[4] << 8) + (unsigned char)data[5]) * ACC_PARAM  - 4);

            ins_gnss_age = data[6];
            ins_num_sv = data[7];
            return 0;
        }

        void Print(const std::string &prefix) {
            std::cout << prefix << "\n";
            std::cout << "\t\t" << "raw_acc_x:" << raw_acc_x << "\n";
            std::cout << "\t\t" << "raw_acc_y:" << raw_acc_y << "\n";
            std::cout << "\t\t" << "raw_acc_z:" << raw_acc_z << "\n";
            std::cout << "\t\t" << "ins_gnss_age:" << ins_gnss_age << "\n";
            std::cout << "\t\t" << "ins_num_sv:" << ins_num_sv << "\n";
        }

    } InsTx2;


    typedef struct InsTx3 {
        float ins_gyro_x;
        float ins_gyro_y;
        float ins_gyro_z;
        int inss_status;
        InsTx3() {
            ins_gyro_x = 0.0;
            ins_gyro_y = 0.0;
            ins_gyro_z = 0;
            inss_status = 0;

        }


        void Print(const std::string &prefix) {
            std::cout << prefix << "\n";
            std::cout << "\t\t" << "ins_gyro_x:" << ins_gyro_x << "\n";
            std::cout << "\t\t" << "ins_gyro_y:" << ins_gyro_y << "\n";
            std::cout << "\t\t" << "ins_gyro_z:" << ins_gyro_z << "\n";
            std::cout << "\t\t" << "inss_status:" << inss_status << "\n";
        }

        int GetFromCanFrame(const char *data, int len) {
            if (len != 8)
                return -1;
            ins_gyro_x = (((unsigned short)data[0] << 8) + (unsigned char)data[1]) * GYRO_PARAM  - 250;
            ins_gyro_y = (((unsigned short)data[2] << 8) + (unsigned char)data[3]) * GYRO_PARAM  - 250;
            ins_gyro_z = (((unsigned short)data[4] << 8) + (unsigned char)data[5]) * GYRO_PARAM  - 250;

            inss_status = data[6];
            return 0;
        }

    } InsTx3;



    typedef struct InsTx4 {
        float raw_gyro_x;
        float raw_gyro_y;
        float raw_gyro_z;
        unsigned short inss_error_status;
        InsTx4() {
            raw_gyro_x = 0.0;
            raw_gyro_y = 0.0;
            raw_gyro_z = 0;
            inss_error_status = 0;

        }


        void Print(const std::string &prefix) {
            std::cout << prefix << "\n";
            std::cout << "\t\t" << "raw_gyro_x:" << raw_gyro_x << "\n";
            std::cout << "\t\t" << "raw_gyro_y:" << raw_gyro_y << "\n";
            std::cout << "\t\t" << "raw_gyro_z:" << raw_gyro_z << "\n";
            std::cout << "\t\t" << "inss_error_status:" << inss_error_status << "\n";
        }

        int GetFromCanFrame(const char *data, int len) {
            if (len != 8)
                return -1;
            raw_gyro_x = (((unsigned short)data[0] << 8) + (unsigned char)data[1]) * GYRO_PARAM  - 250;
            raw_gyro_y = (((unsigned short)data[2] << 8) + (unsigned char)data[3]) * GYRO_PARAM  - 250;
            raw_gyro_z = (((unsigned short)data[4] << 8) + (unsigned char)data[5]) * GYRO_PARAM  - 250;

            inss_error_status = ((unsigned short)data[6] << 8) + (unsigned char)data[7];
            return 0;
        }

    } InsTx4;



    typedef struct InsTx5 {
        float ins_heading_angle;
        float ins_pitch_angle;
        float ins_roll_angle;
        InsTx5() {
            ins_heading_angle = 0.0;
            ins_pitch_angle = 0.0;
            ins_roll_angle = 0;

        }


        void Print(const std::string &prefix) {
            std::cout << prefix << "\n";
            std::cout << "\t\t" << "ins_heading_angle:" << ins_heading_angle << "\n";
            std::cout << "\t\t" << "ins_pitch_angle:" << ins_pitch_angle << "\n";
            std::cout << "\t\t" << "ins_roll_angle:" << ins_roll_angle << "\n";
        }

        int GetFromCanFrame(const char *data, int len) {
            if (len != 8)
                return -1;
            ins_heading_angle = (((unsigned short)data[0] << 8) + (unsigned char)data[1]) * ANGLGE_PARAM  - 360;
            ins_pitch_angle = (((unsigned short)data[2] << 8) + (unsigned char)data[3]) * ANGLGE_PARAM  - 360;
            ins_roll_angle = (((unsigned short)data[4] << 8) + (unsigned char)data[5]) * ANGLGE_PARAM  - 360;

            return 0;
        }

    } InsTx5;



    typedef struct InsTx6 {
        float ins_locat_height;
        unsigned int ins_time;
        InsTx6() {
            ins_locat_height = 0.0;
            ins_time = 0;

        }


        void Print(const std::string &prefix) {
            std::cout << prefix << "\n";
            std::cout << "\t\t" << "ins_locat_height:" << ins_locat_height << "\n";
            std::cout << "\t\t" << "ins_time:" << ins_time << "\n";
        }

        int GetFromCanFrame(const char *data, int len) {
            if (len != 8)
                return -1;
            ins_locat_height = (((unsigned int)data[0] << 24) + ((unsigned int)data[1] << 16) +
                                ((unsigned int)data[2] << 8) + ((unsigned int)data[1])) * HEIGHT_PARAM  - 10000;
            ins_time = (((unsigned int)data[4] << 24) + ((unsigned int)data[5] << 16) +
                        ((unsigned int)data[6] << 8) + ((unsigned int)data[7]));

            return 0;
        }

    } InsTx6;



    typedef struct InsTx7 {
        double ins_latitude;
        double ins_longitude;

        InsTx7() {
            ins_latitude = 0.0;
            ins_longitude = 0.0;


        }


        void Print(const std::string &prefix) {
            std::cout << prefix << "\n";
            std::cout << "\t\t" << "ins_latitude:" << std::setprecision(15) << ins_latitude << "\n";
            std::cout << "\t\t" << "ins_longitude:" << std::setprecision(15) << ins_longitude << "\n";
        }

        int GetFromCanFrame(const char *data, int len) {
            if (len != 8)
                return -1;
            ins_latitude = (((unsigned int)data[0] << 24) +
                            ((unsigned int)data[1] << 16) +
                            ((unsigned int)data[2] << 8) +
                            (unsigned int)data[3]   ) * 0.0000001  - 180;
            ins_longitude = (((unsigned int)data[4] << 24) +
                            ((unsigned int)data[5] << 16) +
                            ((unsigned int)data[6] << 8) +
                            ((unsigned int)data[7]   )) * 0.0000001  - 180;

            return 0;
        }

    } InsTx7;




    typedef struct InsTx8 {
        float ins_east_speed;
        float ins_north_speed;
        float ins_ground_speed;
        InsTx8() {
            ins_east_speed = 0.0;
            ins_north_speed = 0.0;
            ins_ground_speed = 0.0;

        }


        void Print(const std::string &prefix) {
            std::cout << prefix << "\n";
            std::cout << "\t\t" << "ins_east_speed:" << ins_east_speed << "\n";
            std::cout << "\t\t" << "ins_north_speed:" << ins_north_speed << "\n";
            std::cout << "\t\t" << "ins_ground_speed:" << ins_ground_speed << "\n";
        }

        int GetFromCanFrame(const char *data, int len) {
            if (len != 8)
                return -1;
            ins_east_speed = (((unsigned short)data[0] << 8) + (unsigned char)data[1]) * SPEED_PARAM  - 100;
            ins_north_speed = (((unsigned short)data[2] << 8) + (unsigned char)data[3]) * SPEED_PARAM  - 100;
            ins_ground_speed = (((unsigned short)data[4] << 8) + (unsigned char)data[5]) * SPEED_PARAM  - 100;

            return 0;
        }

    } InsTx8;


    typedef struct InsTx9 {
        float ins_std_heading;
        float ins_std_lat;
        float ins_std_height;
        float ins_std_lon;
        InsTx9() {
            ins_std_heading = 0.0;
            ins_std_lat = 0.0;
            ins_std_height = 0.0;
            ins_std_lon = 0.0;

        }


        void Print(const std::string &prefix) {
            std::cout << prefix << "\n";
            std::cout << "\t\t" << "ins_std_heading:" << ins_std_heading << "\n";
            std::cout << "\t\t" << "ins_std_lat:" << ins_std_lat << "\n";
            std::cout << "\t\t" << "ins_std_height:" << ins_std_height << "\n";
            std::cout << "\t\t" << "ins_std_lon:" << ins_std_lon << "\n";
        }

        int GetFromCanFrame(const char *data, int len) {
            if (len != 8)
                return -1;
            ins_std_heading = (((unsigned short)data[0] << 8) + (unsigned char)data[1]) * 0.01  ;
            ins_std_lat = (((unsigned short)data[2] << 8) + (unsigned char)data[3]) * 0.001  ;
            ins_std_height = (((unsigned short)data[4] << 8) + (unsigned char)data[5]) * 0.001  ;

            ins_std_lon = (((unsigned short)data[6] << 8) + (unsigned char)data[7]) * 0.001;
            return 0;
        }

    } InsTx9;



    typedef struct InsTx10 {
        unsigned int gnss_state;
        float gnss_heading;
        float gnss_pitch;
        InsTx10() {
            gnss_state = 0;
            gnss_heading = 0.0;
            gnss_pitch = 0.0;

        }


        void Print(const std::string &prefix) {
            std::cout << prefix << "\n";
            std::cout << "\t\t" << "gnss_state:" << gnss_state << "\n";
            std::cout << "\t\t" << "gnss_heading:" << gnss_heading << "\n";
            std::cout << "\t\t" << "gnss_pitch:" << gnss_pitch << "\n";
        }

        int GetFromCanFrame(const char *data, int len) {
            if (len != 8)
                return -1;
            gnss_state = (((unsigned short)data[0] << 8) + (unsigned char)data[1]) ;
            gnss_heading = (((unsigned short)data[2] << 8) + (unsigned char)data[3]) * ANGLGE_PARAM  - 360;
            gnss_pitch = (((unsigned short)data[4] << 8) + (unsigned char)data[5]) * ANGLGE_PARAM  - 360;

            return 0;
        }

    } InsTx10;




    typedef struct InsTx11 {
        double gnss_locate_height;
        unsigned int gnss_week;

        InsTx11() {
            gnss_locate_height = 0.0;
            gnss_week = 0;


        }


        void Print(const std::string &prefix) {
            std::cout << prefix << "\n";
            std::cout << "\t\t" << "gnss_locate_height:" << std::setprecision(15) << gnss_locate_height << "\n";
            std::cout << "\t\t" << "gnss_week:" << std::setprecision(15) << gnss_week << "\n";
        }

        int GetFromCanFrame(const char *data, int len) {
            if (len != 8)
                return -1;
            gnss_locate_height = (((unsigned int)data[0] << 24) +
                                ((unsigned int)data[1] << 16) +
                                ((unsigned int)data[2] << 8) +
                                (unsigned int)data[3]   ) * 0.001  - 10000;
            gnss_week = (((unsigned int)data[4] << 24) +
                        ((unsigned int)data[5] << 16) +
                        ((unsigned int)data[6] << 8) +
                        ((unsigned int)data[7]   )) ;

            return 0;
        }

    } InsTx11;



    typedef struct InsTx12 {
        double gnss_latitude;
        double gnss_longitude;

        InsTx12() {
            gnss_latitude = 0.0;
            gnss_longitude = 0.0;


        }


        void Print(const std::string &prefix) {
            std::cout << prefix << "\n";
            std::cout << "\t\t" << "gnss_latitude:" << std::setprecision(15) << gnss_latitude << "\n";
            std::cout << "\t\t" << "gnss_longitude:" << std::setprecision(15) << gnss_longitude << "\n";
        }

        int GetFromCanFrame(const char *data, int len) {
            if (len != 8)
                return -1;
            gnss_latitude = (((unsigned int)data[0] << 24) +
                            ((unsigned int)data[1] << 16) +
                            ((unsigned int)data[2] << 8) +
                            (unsigned int)data[3]   ) * 0.0000001  - 180;
            gnss_longitude = (((unsigned int)data[4] << 24) +
                            ((unsigned int)data[5] << 16) +
                            ((unsigned int)data[6] << 8) +
                            ((unsigned int)data[7]   )) * 0.0000001  - 180;

            return 0;
        }

    } InsTx12;





    typedef struct InsTx13 {
        int utc_day;
        int utc_hour;
        int utc_min;
        int utc_month;
        int utc_msec;
        int utc_sec;
        int utc_year;
        InsTx13() {
            utc_day = 0;
            utc_hour = 0;
            utc_min = 0;
            utc_month = 0;
            utc_msec = 0;
            utc_sec = 0;
            utc_year = 0;


        }


        int GetFromCanFrame(const char *data, int len) {
            if (len != 8)
                return -1;


            utc_day = data[0];
            utc_hour = data[1];
            utc_min = data[2];
            utc_month = data[3];
            utc_msec = (((unsigned short)data[4] << 8) + (unsigned char)data[5]) ;
            utc_sec = data[6];
            utc_year = data[7];

            return 0;
        }

        void Print(const std::string &prefix) {
            std::cout << prefix << "\n";
            std::cout << "\t\t" << "utc_day:" << utc_day << "\n";
            std::cout << "\t\t" << "utc_hour:" << utc_hour << "\n";
            std::cout << "\t\t" << "utc_min:" << utc_min << "\n";
            std::cout << "\t\t" << "utc_month:" << utc_month << "\n";
            std::cout << "\t\t" << "utc_msec:" << utc_msec << "\n";
            std::cout << "\t\t" << "utc_sec:" << utc_sec << "\n";
            std::cout << "\t\t" << "utc_year:" << utc_year << "\n";

        }

    } InsTx13;



    typedef struct InsTx14 {
        float gnss_gdop;
        float gnss_hdop;
        float gnss_pdop;
        float gnss_track_true;
        InsTx14() {
            gnss_gdop = 0.0;
            gnss_hdop = 0.0;
            gnss_pdop = 0.0;
            gnss_track_true = 0.0;

        }


        void Print(const std::string &prefix) {
            std::cout << prefix << "\n";
            std::cout << "\t\t" << "gnss_gdop:" << gnss_gdop << "\n";
            std::cout << "\t\t" << "gnss_hdop:" << gnss_hdop << "\n";
            std::cout << "\t\t" << "gnss_pdop:" << gnss_pdop << "\n";
            std::cout << "\t\t" << "gnss_track_true:" << gnss_track_true << "\n";
        }

        int GetFromCanFrame(const char *data, int len) {
            if (len != 8)
                return -1;
            gnss_gdop = (((unsigned short)data[0] << 8) + (unsigned char)data[1]) * 0.01  ;
            gnss_hdop = (((unsigned short)data[2] << 8) + (unsigned char)data[3]) * 0.01  ;
            gnss_pdop = (((unsigned short)data[4] << 8) + (unsigned char)data[5]) * 0.01  ;

            gnss_track_true = (((unsigned short)data[6] << 8) + (unsigned char)data[7]) * ANGLGE_PARAM - 360;
            return 0;
        }

    } InsTx14;



    typedef struct InsTx15 {
        float gnss_hor_speed;
        float gnss_ver_speed;
        float mileage;
        InsTx15() {
            gnss_hor_speed = 0.0;
            gnss_ver_speed = 0.0;
            mileage = 0.0;

        }


        void Print(const std::string &prefix) {
            std::cout << prefix << "\n";
            std::cout << "\t\t" << "gnss_hor_speed:" << gnss_hor_speed << "\n";
            std::cout << "\t\t" << "gnss_ver_speed:" << gnss_ver_speed << "\n";
            std::cout << "\t\t" << "mileage:" << mileage << "\n";
        }

        int GetFromCanFrame(const char *data, int len) {
            if (len != 8)
                return -1;
            gnss_hor_speed = (((unsigned short)data[0] << 8) + (unsigned char)data[1]) * SPEED_PARAM - 100 ;
            gnss_ver_speed = (((unsigned short)data[2] << 8) + (unsigned char)data[3]) * SPEED_PARAM - 100 ;
            mileage = (((unsigned int)data[4] << 24) +
                    ((unsigned int)data[5] << 16) +
                    ((unsigned int)data[6] << 8) +
                    ((unsigned int)data[7]   )) * 0.1  ;

            return 0;
        }

    } InsTx15;



    typedef struct InsTx16 {
        float ins_mod_heading;
        float ins_mod_pitch;
        float ins_mod_roll;
        InsTx16() {
            ins_mod_heading = 0;
            ins_mod_pitch = 0.0;
            ins_mod_roll = 0.0;

        }


        void Print(const std::string &prefix) {
            std::cout << prefix << "\n";
            std::cout << "\t\t" << "ins_mod_heading:" << ins_mod_heading << "\n";
            std::cout << "\t\t" << "ins_mod_pitch:" << ins_mod_pitch << "\n";
            std::cout << "\t\t" << "ins_mod_roll:" << ins_mod_roll << "\n";
        }

        int GetFromCanFrame(const char *data, int len) {
            if (len != 8)
                return -1;
            ins_mod_heading = (((unsigned short)data[0] << 8) + (unsigned char)data[1]) * ANGLGE_PARAM - 360 ;
            ins_mod_pitch = (((unsigned short)data[2] << 8) + (unsigned char)data[3]) * ANGLGE_PARAM  - 360;
            ins_mod_roll = (((unsigned short)data[4] << 8) + (unsigned char)data[5]) * ANGLGE_PARAM  - 360;

            return 0;
        }

    } InsTx16;


    typedef struct InsTx17 {
        unsigned short ins_mod_od1;
        unsigned short ins_mod_od2;
        unsigned short ins_mod_od3;
        unsigned short ins_mod_od4;
        InsTx17() {
            ins_mod_od1 = 0;
            ins_mod_od2 = 0;
            ins_mod_od3 = 0;
            ins_mod_od4 = 0;

        }


        void Print(const std::string &prefix) {
            std::cout << prefix << "\n";
            std::cout << "\t\t" << "ins_mod_od1:" << ins_mod_od1 << "\n";
            std::cout << "\t\t" << "ins_mod_od2:" << ins_mod_od2 << "\n";
            std::cout << "\t\t" << "ins_mod_od3:" << ins_mod_od3 << "\n";
            std::cout << "\t\t" << "ins_mod_od4:" << ins_mod_od4 << "\n";
        }

        int GetFromCanFrame(const char *data, int len) {
            if (len != 8)
                return -1;
            ins_mod_od1 = (((unsigned short)data[0] << 8) + (unsigned char)data[1]) ;
            ins_mod_od2 = (((unsigned short)data[2] << 8) + (unsigned char)data[3]) ;
            ins_mod_od3 = (((unsigned short)data[4] << 8) + (unsigned char)data[5]) ;

            ins_mod_od4 = (((unsigned short)data[6] << 8) + (unsigned char)data[7]);
            return 0;
        }

    } InsTx17;

    typedef struct SensorImuData
    {
        uint64_t imu_cnt;
        int	    week;	// 星期数
        double	utc_time;	// utc时间
        double	gyro_x;	// 陀螺角速度（x） °/s  [-300, 300]
        double	gyro_y;	// 陀螺角速度（y） °/s  [-300, 300]
        double	gyro_z;	// 陀螺角速度（z） °/s  [-300, 300]
        double	acce_x;	// x向加速度  m/s^2 [-490, 490]
        double	acce_y;	// y向加速度  m/s^2 [-490, 490]
        double	acce_z;	// z向加速度  m/s^2 [-490, 490]
        double	temperature;	// 温度  摄氏度   [-40, 60]
    }SensorImuData;

    typedef struct GnssData
    {
        uint64_t gnss_cnt;
        double	lon;	// 经度[-180,180]
        double	lat;	// 纬度[-180,180]
        double	height;	// 水平高度(m)
        double	heading;	// 航向角
        int     heading_stat; //双天线输出标志位。0：解算失败； 1：浮点解； 2：固定解。
        double	velocity;	// GPS速度（m/s）
        double	up_velocity;	// 北向速度（m/s）

        double  horizontal_spd; //from device's gnss_horizontal_spd
        double  vertical_spd;   //from device's gnss_vertical_spd
        double  mileage;        //from device's mileage
        double	gdop;	// 几何精度因子（误差放大倍数）[0, 100]， from device
        double	pdop;	// 位置精度因子（误差放大倍数）[0, 100]， from device

        double	track_angle;	// 接收机当前航迹角[-180, 180]
        int	    week;	// 星期数
        double	utc_time;	// utc时间
        double	hdop;	// 水平精度因子（误差放大倍数）[0, 100]
        double	diff_age;	// 差分期龄[0, 300]
        double	base_length;	// 基线长度（m）
        double	xg;	// 平移后的x坐标
        double	yg;	// 平移后的y坐标
        double	zg;	// 平移后的z坐标
        //GNSS 位置初始化标志位(设备定义)，0-定位数据无效;1-单点定位;2-伪距差分定位;3-双频定位;4-RTK定位，四定解:5-RTK定位，浮点解:6-SBAS定位:7-PPP 定位。
        int	status;	// gsds的GPS状态位 0:无定位;  1:单点定位定  2:伪距差分  4:固定解  5:浮点解
        int	satenum;	// 卫星数

        int recv_stat;//from device's gnss output GNSS_STATE。卫星定位有效位， 0：未收到北斗卫星数据； 1：收到北斗卫星数据。
        double pitch;//from device's gnss output GNSS_Pitch, GNSS俯仰角。
    }GnssData;


    typedef struct LocFusionData
    {
        uint64_t ins_cnt;
        uint16_t week;
        double   utc_time;
        uint32_t inss_ts_s; //from device's inss output ins_time
        float  roll;
        float  pitch;
        float  yaw; //use device's inss output headingAngle
        double lat;
        double lon;
        float height;
        float east_speed;
        float north_speed;
        float up_speed;

        float yaw_std; //from device's inss output INS_Std_Heading
        float lat_std; //from device's inss output INS_Std_Lat
        float height_std; //from device's inss output INS_Std_LocalHeight
        float lon_std; //from device's inss output INS_Std_lon


        float v; //直接填充0.5 * (wheel_speed.left_rear_speed + wheel_speed.right_rear_speed)？
                 //或者 use device's inss output INS_ToGroundSpd

        float xg;
        float yg;
        float zg;
        float xg_dr;
        float yg_dr;
        float zg_dr;
        float dr_time;

        //惯导状态(设备定义): 0-惯导正在启动;1-惯导启动完成;2-正在对准，卫星辅助;3-正在对准，装订模式;4-导航状态，惯导/卫星/轮速组合模式:5-导航状态，惯导/卫星组合模式;6-导航状态，惯导/轮速组合模式;7-导航状态，纯惯导模式
        int    status; // 驱动直接填0？
        //惯导故障状态(设备定义，can中是Byte6/7)： 0-惯导周期自检正常;Bit48 置 1-陀螺X故障:Bit49 置1-陀螺Y故障;Bit50 置 1-陀螺Z故障;Bit51 置1-加速度计X故障;Bit52置1-加速度计Y故障;Bit53置1-加速度计乙故障;
        //Bit54置1-卫星辅助对准，未收到卫星定位定向数据:Bit55置1-装订模式对准，装订数据超差;Bit40置1-轮速计故障，其它bit暂时保留。
        int    err_status;
        int    gps_status; //from gnss.status
        int	   satenum;
        double hdop;
        double diff_age;
    }LocFusionData;

    typedef struct WheelSpeed289 {
        double m1_speed;
        double m2_speed;
        bool status;/*  */
        WheelSpeed289() {
            //分别是左右轮
            m1_speed = 0;
            m2_speed = 0;
            status = false;
        }

        void Print(const std::string &prefix) {
            std::cout << prefix << "m1_speed:" << m1_speed << std::endl;
            std::cout << prefix << "m2_speed:" << m2_speed << std::endl;
            std::cout << prefix << "status:" << (int)status << std::endl;
        }

        int GetFromCanFrame(const char *data, int len) {
            if (len != 8)
                return -1;
            double param = U_PI * RADIUS / (30.0 * RATIO);
            m1_speed = param * ((((unsigned short)data[0] << 8) + (unsigned char)data[1]) + WHEEL_SPEED_OFFSET)  ;
            m2_speed = param * ((((unsigned short)data[2] << 8) + (unsigned char)data[3]) + WHEEL_SPEED_OFFSET) ;
            status = ((data[4] & 0x80) != 0) ;
            return 0;
        }

    } WheelSpeed289;

    typedef struct WheelSpeedData
    {
        uint64_t wheel_speed_cnt;
    	int	    week;	// 星期数
        double	utc_time;	// utc时间
        float	left_rear_speed;	// 左后轮速（m/s）
        float	right_rear_speed;	// 右后轮速（m/s）
        float	left_front_speed;	// 左前轮速（m/s）
        float	right_front_speed;	// 右前轮速（m/s）
        int	    pluse_mask;	// 帧计数
    } WheelSpeedData;
}
}

#endif