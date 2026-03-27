#ifndef __PARAM_PROTOCOL_H__
#define __PARAM_PROTOCOL_H__

#include <queue>
#include <iostream>
#include "macros.h"

using namespace std;

//PARAM_CMD:PARAM_LIST_CMD
//PARAM_CMD:PARAM_SET_CMD:module_name:param1:-i:1
//PARAM_CMD:PARAM_GET_CMD:module_name:param1:-i

#define PARAM_CMD                 "PARAM_CMD:"

#define PARAM_LIST_CMD            "PARAM_LIST_CMD"

#define PARAM_SET_CMD             "PARAM_SET_CMD:"
#define PARAM_GET_CMD             "PARAM_GET_CMD:"

#define PARAM_VALUE_CMD           "PARAM_VALUE_CMD:"

#define PARAM_REGISTER_NODE       "PARAM_REGISTER_NODE:"
//#define PARAM_REGISTER_VALUE      "PARAM_REGISTER_VALUE:"


#define PARAM_RESPONSE_SUCESS     "PARAM_RESPONSE_SUCESS:"
#define PARAM_RESPONSE_ERROR      "PARAM_RESPONSE_ERROR:"
#define PARAM_RESPONSE_LIST       "PARAM_RESPONSE_LIST:"

class CParamProtocol
{
public:
    CParamProtocol() {}


    BASE_DECLARE_SINGLETON(CParamProtocol)
};

#endif
