#ifndef __REGISTER_DATA_TYPE_H__
#define __REGISTER_DATA_TYPE_H__


#include <string>
#include <map>
#include <iostream>
#include "msghandle.hpp"
#include "cprint2json.h"

typedef std::string (*FPrintMsgPack)(const std::string &buffer);
typedef std::string (*FGetMsgPackmd5)();

#define REGISTER(StructType, FunctionName)                                            		 \
    inline std::string Print##FunctionName(const std::string &buffer){                              \
    	StructType type_value;																	 \
    	msghandle::object_handle handle = msghandle::unpack( buffer.data(), buffer.size() );     \
        type_value = handle.get().as< StructType >();												\
        std::stringstream ss_buffer;														\
    	msghandle::v1::print(ss_buffer, type_value);												\
    	return ss_buffer.str();															\
    }                                                                   					\
    inline std::string MD5##FunctionName(){                              \
    	StructType type_value;																	 \
        std::string buffer_str;																\
		msghandle::v1::checkcode(buffer_str, type_value); 										\
		return buffer_str;						 											\
	}																						\
inline RegisterAction g_creatorRegister##FunctionName(                        					\
       #StructType,(FPrintMsgPack)Print##FunctionName, (FGetMsgPackmd5)MD5##FunctionName); \




//工厂类的定义
class MsgPackFactory {
private:
	std::map<std::string, FPrintMsgPack> m_printFunctionMap;
	std::map<std::string, FGetMsgPackmd5> m_md5FunctionMap;
	CPrint2Json m_print2json;
	MsgPackFactory() {}; //构造函数私有化

public:
//@brief:通过类名称字符串获取类的实例
	std::string PrintMsgPack(const std::string& className, const std::string &buffer)
	{
		auto iter = m_printFunctionMap.find(className);
		if (iter == m_printFunctionMap.end())
			return "";
		else
			return iter->second(buffer);
	}

	std::string PrintJsonMsgPack(const std::string& className, const std::string &buffer)
	{
		auto iter = m_printFunctionMap.find(className);
		if (iter == m_printFunctionMap.end())
			return "";
		else
			return m_print2json.print2json(iter->second(buffer));
	}

	std::string MsgPackMd5(const std::string& className)
	{
		auto iter = m_md5FunctionMap.find(className);
		if (iter == m_md5FunctionMap.end())
			return "";
		else
			return iter->second();
	}




//@brief:将给定的类名称字符串和对应的创建类对象的函数保存到map中
	void registClass(std::string name, FPrintMsgPack method, FGetMsgPackmd5 md5_func)
	{
		m_printFunctionMap.insert(make_pair(name, method));
		m_md5FunctionMap.insert(make_pair(name, md5_func));
	}

//@brief:获取工厂类的单个实例对象
	static MsgPackFactory& getInstance()
	{
		static MsgPackFactory sLo_factory;
		return sLo_factory;
	}
};

//注册动作类
class RegisterAction {
public:

	RegisterAction(const std::string className, FPrintMsgPack ptrCreateFn, FGetMsgPackmd5 md5_func)
	{
		MsgPackFactory::getInstance().registClass(className, ptrCreateFn, md5_func);
	}
	void operator()(const std::string className, FPrintMsgPack ptrCreateFn, FGetMsgPackmd5 md5_func)
	{
		MsgPackFactory::getInstance().registClass(className, ptrCreateFn, md5_func);
	}
};
#endif