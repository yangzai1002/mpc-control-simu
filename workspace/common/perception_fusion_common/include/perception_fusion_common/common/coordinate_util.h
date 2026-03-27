#pragma once

#include <Eigen/Dense>
#include <Eigen/Eigen>
#include <Eigen/Core>

#include "imu_message.h"

namespace avos {
namespace perception {
namespace algorithm {
    
LocationType  ConvertLocationStruct2Dr(PerceptionLocType loc);
LocationType  ConvertLocationStruct2Global(PerceptionLocType loc);

Eigen::Vector3d ConvertPointVCS2UTM(const LocationType &locpos,
                                    const Eigen::Vector3d &pos_vcs);

Eigen::Vector3d ConvertPointUTM2VCS(const LocationType &locpos,
                                    const Eigen::Vector3d &pos_utm);

Eigen::Vector3d ConvertPointVCS2DR(const LocationType &locpos,
                                   const Eigen::Vector3d &pos_vcs);

Eigen::Vector3d ConvertPointDR2VCS(const LocationType &locpos,
                                   const Eigen::Vector3d &pos_dr);

Eigen::Vector3d ConvertPointVCS2UTM(const PerceptionLocType &locpos,
                                    const Eigen::Vector3d &pos_vcs);

Eigen::Vector3d ConvertPointUTM2VCS(const PerceptionLocType &locpos,
                                    const Eigen::Vector3d &pos_utm);

Eigen::Vector3d ConvertPointVCS2DR(const PerceptionLocType &locpos,
                                   const Eigen::Vector3d &pos_vcs);

Eigen::Vector3d ConvertPointDR2VCS(const PerceptionLocType &locpos,
                                   const Eigen::Vector3d &pos_dr);

Eigen::Vector3d ConvertVelocityVCS2DR(const PerceptionLocType &locpos,
                                      const Eigen::Vector3d &obj_vcs_pos,
                                      const Eigen::Vector3d &obj_vcs_vel);

Eigen::Vector3d ConvertVelocityDR2VCS(const PerceptionLocType & locpos,
                                      const Eigen::Vector3d & obj_vcs_pos,
                                      const Eigen::Vector3d & obj_dr_vel);
Eigen::Vector3d ConvertVelocityVCS2DR(const LocationType &locpos,
                                      const Eigen::Vector3d &obj_vcs_pos,
                                      const Eigen::Vector3d &obj_vcs_vel);
Eigen::Vector3d ConvertVelocityVCS2UTM(const PerceptionLocType &locpos,
                                      const Eigen::Vector3d &obj_vcs_pos,
                                      const Eigen::Vector3d &obj_vcs_vel);                                      
}  // namespace algorithm
}  // namespace perception
}  // namespace avos

