#ifndef AVOS_THREAD_CONSTS_H
#define AVOS_THREAD_CONSTS_H

namespace avos
{
namespace thread
{

enum RwLockType
{
    rwltNONE,
    rwltBALANCE,
    rwltREADFIRST,
    rwltWRITEFIRST
};

static const RwLockType default_lock_type = RwLockType::rwltBALANCE;

}
} // namespace avos

#endif
