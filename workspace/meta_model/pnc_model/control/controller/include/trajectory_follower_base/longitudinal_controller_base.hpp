#ifndef LONGITUDINAL_CONTROLLER_BASE_HPP_
#define LONGITUDINAL_CONTROLLER_BASE_HPP_

#include "trajectory_follower_base/input_data.hpp"

#include <boost/optional.hpp>
#include "comm/comm_struct.hpp"

namespace trajectory_follower
{

struct LongitudinalOutput
{
    commStruct::LongitudinalCommand control_cmd;
    LongitudinalSyncData sync_data;
};

class LongitudinalControllerBase
{

public:
    virtual ~LongitudinalControllerBase()
    {
    }

    virtual bool isReady(const InputData &input_data) = 0;

    virtual LongitudinalOutput run(InputData const &input_data) = 0;

    void sync(LateralSyncData const &lateral_sync_data);

    void reset();

protected:
    LateralSyncData lateral_sync_data_;

};

}    // namespace

#endif  // LONGITUDINAL_CONTROLLER_BASE_HPP_
