#ifndef _ERROR_CODE_
#define _ERROR_CODE_

#include <string>

namespace avos {
namespace planning {

struct ErrorCode{
    std::string error_code;
    int type;
    bool report;
    bool record;
    
    bool operator==(const ErrorCode& e) const {
        return error_code == e.error_code;
    }
};
}
}
#endif // _ERROR_CODE_