#pragma once

#include "ara/adsfi/impl_type_msghaflocation.h"
void Location_(const ara::adsfi::MsgHafLocation& dr_loc,
               const ara::adsfi::MsgHafLocation& tpimu_loc,
               ara::adsfi::MsgHafLocation& out_loc);
