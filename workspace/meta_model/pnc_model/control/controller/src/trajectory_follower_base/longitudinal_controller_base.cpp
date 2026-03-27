
#include "trajectory_follower_base/longitudinal_controller_base.hpp"

namespace trajectory_follower
{

void LongitudinalControllerBase::sync(LateralSyncData const &lateral_sync_data)
{
    lateral_sync_data_ = lateral_sync_data;
}

void LongitudinalControllerBase::reset()
{
    lateral_sync_data_.is_steer_converged = false;
}

}    // namespace trajectory_follower
