#ifndef SERIALIZATIONUTILS_HPP
#define SERIALIZATIONUTILS_HPP

#include "msghandle.hpp"

#define USE_MSGHANDLE

class CSerialzationUtils
{
public:
    CSerialzationUtils() {}

    template <typename T>
    static void serialzation(const T& wvalue,std::string& str_buffer)
    {
        std::stringstream buffer;
        SMsgHandleInfo info_value;
        msghandle::v1::pack(buffer, wvalue,info_value);
        str_buffer = std::move(buffer.str());
    }
};

class CUnSerialzationUtils
{
public:
    CUnSerialzationUtils() {}

    template <typename T>
    static void unserialzation(T& rvalue,std::string& str_buffer)
    {
        const char* data_ptr = str_buffer.c_str();
        int data_len = str_buffer.size();

        if(data_ptr!=NULL&&data_len!=0)
        {
            msghandle::object_handle handle = msghandle::unpack( data_ptr,data_len );
            msghandle::object obj    = handle.get();
            rvalue = obj.as< T >();
        }
    }
};


#endif // SERIALIZATIONUTILS_HPP
