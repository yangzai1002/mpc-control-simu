#ifndef LATERAL_CONTROLLER_BASE_HPP_
#define LATERAL_CONTROLLER_BASE_HPP_

#include <boost/optional.hpp>

#include "comm/comm_struct.hpp"
#include "trajectory_follower_base/input_data.hpp"

namespace trajectory_follower
{

struct LateralOutput
{
    commStruct::AckermannLateralCommand control_cmd;
    LateralSyncData sync_data;
};

class LateralControllerBase
{

public:
    virtual ~LateralControllerBase()
    {
    }

    void sync(LongitudinalSyncData const &longitudinal_sync_data);
    virtual LateralOutput run(InputData const &input_data) = 0;
    virtual bool isReady(const InputData &input_data) = 0;

protected:
    LongitudinalSyncData longitudinal_sync_data_;

};

}    // namespace

#endif  // LATERAL_CONTROLLER_BASE_HPP_
