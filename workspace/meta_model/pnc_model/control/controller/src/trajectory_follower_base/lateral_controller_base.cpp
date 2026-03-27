#include "trajectory_follower_base/lateral_controller_base.hpp"

namespace trajectory_follower
{

void LateralControllerBase::sync(
        LongitudinalSyncData const &longitudinal_sync_data)
{
    longitudinal_sync_data_ = longitudinal_sync_data;
}

}    // namespace trajectory_follower
