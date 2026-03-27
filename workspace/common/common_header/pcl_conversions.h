
#ifndef AVOS_PCL_CONVERSIONS_H__
#define AVOS_PCL_CONVERSIONS_H__

#include <vector>

#include <pcl/conversions.h>

#include <pcl/PCLHeader.h>
//#include <std_msgs/Header.h>
#include "std_header.pb.h"

#include <pcl/PCLImage.h>
//#include <sensor_msgs/Image.h>

#include <pcl/PCLPointField.h>
//#include <sensor_msgs/PointField.h>

#include <pcl/PCLPointCloud2.h>
//#include <sensor_msgs/PointCloud2.h>
#include "pointcloud2.pb.h"

#include <pcl/PointIndices.h>
// #include <pcl_msgs/PointIndices.h>

#include <pcl/ModelCoefficients.h>
// #include <pcl_msgs/ModelCoefficients.h>

#include <pcl/Vertices.h>
// #include <pcl_msgs/Vertices.h>

#include <pcl/PolygonMesh.h>
// #include <pcl_msgs/PolygonMesh.h>

#include <pcl/io/pcd_io.h>

#include <Eigen/StdVector>
#include <Eigen/Geometry>
namespace pcl_conversions
{

    /** PCLHeader <=> Header **/

    inline void fromPCL(const std::uint64_t &pcl_stamp, avos::std_msg::Time &stamp)
    {
        stamp.set_sec(static_cast<int32_t>(pcl_stamp * 1000ull / 1000000000UL));
        stamp.set_nsec(pcl_stamp * 1000ull - stamp.sec() * 1000000000UL);
        //    stamp.fromNSec(pcl_stamp * 1000ull);  // Convert from us to ns
    }

    inline void toPCL(const avos::std_msg::Time &stamp, std::uint64_t &pcl_stamp)
    {
        pcl_stamp = (stamp.sec() * 1000000000UL + stamp.nsec()) / 1000ull; // Convert from ns to us
    }

    inline avos::std_msg::Time fromPCL(const std::uint64_t &pcl_stamp)
    {
        avos::std_msg::Time stamp;
        fromPCL(pcl_stamp, stamp);
        return stamp;
    }

    inline std::uint64_t toPCL(const avos::std_msg::Time &stamp)
    {
        std::uint64_t pcl_stamp;
        toPCL(stamp, pcl_stamp);
        return pcl_stamp;
    }

    /** PCLHeader <=> Header **/

    inline void fromPCL(const pcl::PCLHeader &pcl_header, avos::std_msg::Header &header)
    {
        avos::std_msg::Time stamp;
        fromPCL(pcl_header.stamp, stamp);
        header.mutable_stamp()->CopyFrom(stamp);
        header.set_frame_id(pcl_header.frame_id);
        header.set_sequence_num(pcl_header.seq);
        //      header.seq = pcl_header.seq;
        //      header.frame_id = pcl_header.frame_id;
    }

    inline void toPCL(const avos::std_msg::Header &header, pcl::PCLHeader &pcl_header)
    {
        toPCL(header.stamp(), pcl_header.stamp);
        pcl_header.seq = header.sequence_num();
        pcl_header.frame_id = header.frame_id();
    }

    inline avos::std_msg::Header fromPCL(const pcl::PCLHeader &pcl_header)
    {
        avos::std_msg::Header header;
        fromPCL(pcl_header, header);
        return header;
    }

    inline pcl::PCLHeader toPCL(const avos::std_msg::Header &header)
    {
        pcl::PCLHeader pcl_header;
        toPCL(header, pcl_header);
        return pcl_header;
    }

    /** PCLPointField <=> PointField **/

    inline void fromPCL(const pcl::PCLPointField &pcl_pf, avos::sensor_msgs::PointField &pf)
    {
        pf.set_name(pcl_pf.name);
        pf.set_offset(pcl_pf.offset);
        pf.set_datatype((avos::sensor_msgs::PointField::Type)pcl_pf.datatype);
        pf.set_count(pcl_pf.count);
    }

    inline void fromPCL(const std::vector<pcl::PCLPointField> &pcl_pfs, std::vector<avos::sensor_msgs::PointField> &pfs)
    {
        pfs.resize(pcl_pfs.size());
        std::vector<pcl::PCLPointField>::const_iterator it = pcl_pfs.begin();
        int i = 0;
        for (; it != pcl_pfs.end(); ++it, ++i)
        {
            fromPCL(*(it), pfs[i]);
        }
    }

    inline void toPCL(const avos::sensor_msgs::PointField &pf, pcl::PCLPointField &pcl_pf)
    {
        pcl_pf.name = pf.name();
        pcl_pf.offset = pf.offset();
        pcl_pf.datatype = pf.datatype();
        pcl_pf.count = pf.count();
    }

    inline void toPCL(const std::vector<avos::sensor_msgs::PointField> &pfs, std::vector<pcl::PCLPointField> &pcl_pfs)
    {
        pcl_pfs.resize(pfs.size());
        std::vector<avos::sensor_msgs::PointField>::const_iterator it = pfs.begin();
        int i = 0;
        for (; it != pfs.end(); ++it, ++i)
        {
            toPCL(*(it), pcl_pfs[i]);
        }
    }

    inline void toPCL(const google::protobuf::RepeatedPtrField<::avos::sensor_msgs::PointField> &pfs,
                      std::vector<pcl::PCLPointField> &pcl_pfs)
    {
        pcl_pfs.resize(pfs.size());

        for (int var = 0; var < pfs.size(); ++var)
        {
            toPCL(pfs.Get(var), pcl_pfs[var]);
        }
    }

    /** PCLPointCloud2 <=> PointCloud2 **/

    inline void copyPCLPointCloud2MetaData(const pcl::PCLPointCloud2 &pcl_pc2, avos::sensor_msgs::PointCloud2 &pc2)
    {
        avos::std_msg::Header header;
        fromPCL(pcl_pc2.header, header);
        pc2.mutable_header()->CopyFrom(header);

        pc2.set_height(pcl_pc2.height);
        pc2.set_width(pcl_pc2.width);

        std::vector<avos::sensor_msgs::PointField> fields;
        fromPCL(pcl_pc2.fields, fields);
        pc2.mutable_fields()->CopyFrom({fields.begin(), fields.end()});

        pc2.set_is_bigendian(pcl_pc2.is_bigendian);
        pc2.set_point_step(pcl_pc2.point_step);
        pc2.set_row_step(pcl_pc2.row_step);
        pc2.set_is_dense(pcl_pc2.is_dense);
    }

    inline void fromPCL(const pcl::PCLPointCloud2 &pcl_pc2, avos::sensor_msgs::PointCloud2 &pc2)
    {
        copyPCLPointCloud2MetaData(pcl_pc2, pc2);
        pc2.mutable_data()->assign(pcl_pc2.data.begin(), pcl_pc2.data.end());
    }

    inline void moveFromPCL(pcl::PCLPointCloud2 &pcl_pc2, avos::sensor_msgs::PointCloud2 &pc2)
    {
        copyPCLPointCloud2MetaData(pcl_pc2, pc2);
        pc2.mutable_data()->assign(pcl_pc2.data.begin(), pcl_pc2.data.end());

        std::vector<pcl::uint8_t> tmp_vec;
        pcl_pc2.data.swap(tmp_vec);
    }

    inline void copyPointCloud2MetaData(const avos::sensor_msgs::PointCloud2 &pc2, pcl::PCLPointCloud2 &pcl_pc2)
    {
        toPCL(pc2.header(), pcl_pc2.header);
        pcl_pc2.height = pc2.height();
        pcl_pc2.width = pc2.width();
        toPCL(pc2.fields(), pcl_pc2.fields);
        pcl_pc2.is_bigendian = pc2.is_bigendian();
        pcl_pc2.point_step = pc2.point_step();
        pcl_pc2.row_step = pc2.row_step();
        pcl_pc2.is_dense = pc2.is_dense();
    }

    inline void toPCL(const avos::sensor_msgs::PointCloud2 &pc2, pcl::PCLPointCloud2 &pcl_pc2)
    {
        copyPointCloud2MetaData(pc2, pcl_pc2);
        pcl_pc2.data.assign(pc2.data().begin(), pc2.data().end());
    }

    inline void moveToPCL(avos::sensor_msgs::PointCloud2 &pc2, pcl::PCLPointCloud2 &pcl_pc2)
    {
        copyPointCloud2MetaData(pc2, pcl_pc2);
        pcl_pc2.data.assign(pc2.data().begin(), pc2.data().end());

        std::string tmp_str;
        pc2.mutable_data()->swap(tmp_str);
    }

} // namespace pcl_conversions

namespace pcl
{

    /** Provide to/fromROSMsg for avos::sensor_msgs::PointCloud2 <=> pcl::PointCloud<T> **/

    template <typename T>
    void toROSMsg(const pcl::PointCloud<T> &pcl_cloud, avos::sensor_msgs::PointCloud2 &cloud)
    {
        pcl::PCLPointCloud2 pcl_pc2;
        pcl::toPCLPointCloud2(pcl_cloud, pcl_pc2);
        pcl_conversions::moveFromPCL(pcl_pc2, cloud);
    }

    template <typename T>
    void fromROSMsg(const avos::sensor_msgs::PointCloud2 &cloud, pcl::PointCloud<T> &pcl_cloud)
    {
        pcl::PCLPointCloud2 pcl_pc2;
        pcl_conversions::toPCL(cloud, pcl_pc2);
        pcl::fromPCLPointCloud2(pcl_pc2, pcl_cloud);
    }

    template <typename T>
    void moveFromROSMsg(avos::sensor_msgs::PointCloud2 &cloud, pcl::PointCloud<T> &pcl_cloud)
    {
        pcl::PCLPointCloud2 pcl_pc2;
        pcl_conversions::moveToPCL(cloud, pcl_pc2);
        pcl::fromPCLPointCloud2(pcl_pc2, pcl_cloud);
    }

} // namespace pcl

#endif /* PCL_CONVERSIONS_H__ */
