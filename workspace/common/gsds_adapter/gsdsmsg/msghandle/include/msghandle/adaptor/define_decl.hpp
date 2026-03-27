//
// MessagePack for C++ static resolution routine
//
// Copyright (C) 2016 KONDO Takatoshi
//
//    Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//    http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef MSGHANDLE_DEFINE_DECL_HPP
#define MSGHANDLE_DEFINE_DECL_HPP

#if defined(MSGHANDLE_NO_BOOST)

// MSGHANDLE_PP_VARIADICS is defined in msghandle/preprocessor/config/config.hpp
// http://www.boost.org/libs/preprocessor/doc/ref/variadics.html
// However, supporting compiler detection is not complete. msghandle-c requires
// variadic macro arguments support. So MSGHANDLE_PP_VARIADICS is defined here explicitly.
#if !defined(MSGHANDLE_PP_VARIADICS)
#define MSGHANDLE_PP_VARIADICS
#endif

#include <msghandle/preprocessor.hpp>

#define MSGHANDLE_BASE_ARRAY(base) (*const_cast<base *>(static_cast<base const*>(this)))
#define MSGHANDLE_NVP(name, value) (name) (value)

#define MSGHANDLE_DEFINE_MAP_EACH_PROC(r, data, elem) \
    MSGHANDLE_PP_IF( \
        MSGHANDLE_PP_IS_BEGIN_PARENS(elem), \
        elem, \
        (MSGHANDLE_PP_STRINGIZE(elem))(elem) \
    )

#define MSGHANDLE_DEFINE_MAP_IMPL(...) \
    MSGHANDLE_PP_SEQ_TO_TUPLE( \
        MSGHANDLE_PP_SEQ_FOR_EACH( \
            MSGHANDLE_DEFINE_MAP_EACH_PROC, \
            0, \
            MSGHANDLE_PP_VARIADIC_TO_SEQ(__VA_ARGS__) \
        ) \
    )

#define MSGHANDLE_DEFINE_MAP(...) \
    template <typename Packer> \
    void msghandle_pack(Packer& msghandle_pk) const \
    { \
        msghandle::type::make_define_map \
            MSGHANDLE_DEFINE_MAP_IMPL(__VA_ARGS__) \
            .msghandle_pack(msghandle_pk); \
    } \
    void msghandle_unpack(msghandle::object const& msghandle_o) \
    { \
        msghandle::type::make_define_map \
            MSGHANDLE_DEFINE_MAP_IMPL(__VA_ARGS__) \
            .msghandle_unpack(msghandle_o); \
    }\
    template <typename MSGHANDLE_OBJECT> \
    void msghandle_object(MSGHANDLE_OBJECT* msghandle_o, msghandle::zone& msghandle_z) const \
    { \
        msghandle::type::make_define_map \
            MSGHANDLE_DEFINE_MAP_IMPL(__VA_ARGS__) \
            .msghandle_object(msghandle_o, msghandle_z); \
    }

#define MSGHANDLE_BASE_MAP(base) \
    (MSGHANDLE_PP_STRINGIZE(base))(*const_cast<base *>(static_cast<base const*>(this)))

#else  // defined(MSGHANDLE_NO_BOOST)

// BOOST_PP_VARIADICS is defined in boost/preprocessor/config/config.hpp
// http://www.boost.org/libs/preprocessor/doc/ref/variadics.html
// However, supporting compiler detection is not complete. msghandle-c requires
// variadic macro arguments support. So BOOST_PP_VARIADICS is defined here explicitly.
#if !defined(BOOST_PP_VARIADICS)
#define BOOST_PP_VARIADICS
#endif

#include <boost/preprocessor.hpp>

#define MSGHANDLE_BASE_ARRAY(base) (*const_cast<base *>(static_cast<base const*>(this)))
#define MSGHANDLE_NVP(name, value) (name) (value)

#define MSGHANDLE_DEFINE_MAP_EACH_PROC(r, data, elem) \
    BOOST_PP_IF( \
        BOOST_PP_IS_BEGIN_PARENS(elem), \
        elem, \
        (BOOST_PP_STRINGIZE(elem))(elem) \
    )

#define MSGHANDLE_DEFINE_MAP_IMPL(...) \
    BOOST_PP_SEQ_TO_TUPLE( \
        BOOST_PP_SEQ_FOR_EACH( \
            MSGHANDLE_DEFINE_MAP_EACH_PROC, \
            0, \
            BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__) \
        ) \
    )

#define MSGHANDLE_DEFINE_MAP(...) \
    template <typename Packer> \
    void msghandle_pack(Packer& msghandle_pk) const \
    { \
        msghandle::type::make_define_map \
            MSGHANDLE_DEFINE_MAP_IMPL(__VA_ARGS__) \
            .msghandle_pack(msghandle_pk); \
    } \
    void msghandle_unpack(msghandle::object const& msghandle_o) \
    { \
        msghandle::type::make_define_map \
            MSGHANDLE_DEFINE_MAP_IMPL(__VA_ARGS__) \
            .msghandle_unpack(msghandle_o); \
    }\
    template <typename MSGHANDLE_OBJECT> \
    void msghandle_object(MSGHANDLE_OBJECT* msghandle_o, msghandle::zone& msghandle_z) const \
    { \
        msghandle::type::make_define_map \
            MSGHANDLE_DEFINE_MAP_IMPL(__VA_ARGS__) \
            .msghandle_object(msghandle_o, msghandle_z); \
    }

#define MSGHANDLE_BASE_MAP(base) \
    (BOOST_PP_STRINGIZE(base))(*const_cast<base *>(static_cast<base const*>(this)))

#endif // defined(MSGHANDLE_NO_BOOST)

#include "msghandle/versioning.hpp"

// for MSGHANDLE_ADD_ENUM
#include "msghandle/adaptor/int.hpp"

#define MSGHANDLE_IS_CONTINUOUS_MEM(ret) \
    unsigned int is_continuous_memory() const \
    { \
        return ret; \
    }

#define MSGHANDLE_DEFINE_ARRAY(...) \
    template <typename Packer> \
    void msghandle_pack(Packer& msghandle_pk,SMsgHandleInfo& msghandle_info) const \
    { \
        if(msghandle_info.is_vector_T==1) \
        { \
            msghandle_info.is_continuous_memory=is_continuous_memory(); \
            return; \
        } \
        msghandle::type::make_define_array(__VA_ARGS__).msghandle_pack(msghandle_pk,msghandle_info); \
    } \
    void msghandle_unpack(msghandle::object const& msghandle_o) \
    { \
        std::vector<msghandle::type::fix_int32> key_hash_list_src = key_hash_list; \
        std::map<int,int> index_map; \
        index_map[0] = 0; \
        msghandle::type::make_define_array(key_hash_list).msghandle_unpack(msghandle_o,index_map); \
        for (int n = 0; n < key_hash_list_src.size(); ++n) { \
            auto it = std::find(key_hash_list.begin(), key_hash_list.end(), key_hash_list_src[n]); \
            if (it != key_hash_list.end()) { \
                int postion = (it - key_hash_list.begin()); \
                index_map[n+1] = postion+1; \
                if( postion == n)  \
                { continue; } \
                else{} \
            } else { \
                index_map[n+1] = -1; \
            } \
        } \
    if( is_continuous_memory() == 1 && index_map.size() == 1 ) \
        msghandle::type::make_define_array(__VA_ARGS__).msghandle_unpack(msghandle_o); \
    else \
        msghandle::type::make_define_array(__VA_ARGS__).msghandle_unpack(msghandle_o,index_map); \
    } \
    template <typename MSGHANDLE_OBJECT> \
    void msghandle_object(MSGHANDLE_OBJECT* msghandle_o, msghandle::zone& msghandle_z) const \
    { \
        msghandle::type::make_define_array(__VA_ARGS__).msghandle_object(msghandle_o, msghandle_z); \
    }

// MSGHANDLE_ADD_ENUM must be used in the global namespace.
#define MSGHANDLE_ADD_ENUM(enum_name) \
  namespace msghandle { \
  /** @cond */ \
  MSGHANDLE_API_VERSION_NAMESPACE(MSGHANDLE_DEFAULT_API_NS) { \
  /** @endcond */ \
  namespace adaptor { \
    template<> \
    struct convert<enum_name> { \
      msghandle::object const& operator()(msghandle::object const& msghandle_o, enum_name& msghandle_v) const { \
        msghandle::underlying_type<enum_name>::type tmp; \
        msghandle::operator>>(msghandle_o, tmp);                   \
        msghandle_v = static_cast<enum_name>(tmp);   \
        return msghandle_o; \
      } \
    }; \
    template<> \
    struct object<enum_name> { \
      void operator()(msghandle::object& msghandle_o, const enum_name& msghandle_v) const { \
        msghandle::underlying_type<enum_name>::type tmp = static_cast<msghandle::underlying_type<enum_name>::type>(msghandle_v); \
        msghandle::operator<<(msghandle_o, tmp);                                    \
      } \
    }; \
    template<> \
    struct object_with_zone<enum_name> { \
      void operator()(msghandle::object::with_zone& msghandle_o, const enum_name& msghandle_v) const {  \
        msghandle::underlying_type<enum_name>::type tmp = static_cast<msghandle::underlying_type<enum_name>::type>(msghandle_v); \
        msghandle::operator<<(msghandle_o, tmp);                                    \
      } \
    }; \
    template <> \
    struct pack<enum_name> { \
      template <typename Stream> \
      msghandle::packer<Stream>& operator()(msghandle::packer<Stream>& msghandle_o, const enum_name& msghandle_v) const { \
          return msghandle::operator<<(msghandle_o, static_cast<msghandle::underlying_type<enum_name>::type>(msghandle_v)); \
      } \
    }; \
  } \
  /** @cond */ \
  } \
  /** @endcond */ \
  }

#if defined(MSGHANDLE_USE_DEFINE_MAP)
#define MSGHANDLE_DEFINE MSGHANDLE_DEFINE_MAP
#define MSGHANDLE_BASE MSGHANDLE_BASE_MAP
#else  // defined(MSGHANDLE_USE_DEFINE_MAP)
#define MSGHANDLE_DEFINE MSGHANDLE_DEFINE_ARRAY
#define MSGHANDLE_BASE MSGHANDLE_BASE_ARRAY

#define MSGHANDLE_PARAKLIST_DEFINE(...) \
    std::vector<std::string> paralist() const \
    { \
        std::vector<std::string> para_list = {__VA_ARGS__}; \
        return para_list; \
    }

#define MSGHANDLE_PARAVLIST_DEFINE(...) \
    template <typename Stream>\
void msghandle_print(msghandle::packer<Stream>& msgpack_pk,const std::string& k) const\
{\
    std::vector<std::string> para_name_vec = paralist();\
    msgpack_pk.stream() << k <<":\n";\
    std::string tmp_k = k;\
    char c = ' ';\
    int count = 0;\
    std::string pix=" ";\
    while ((count = tmp_k.find(c)) != std::string::npos) { \
        ++count;\
        tmp_k.erase(count - 1, 1);  \
        pix+=c;\
    } \
    for (int var = 0; var < para_name_vec.size(); ++var) { \
        para_name_vec[var]=pix+para_name_vec[var]; \
    }\
    msghandle::type::make_define_array(__VA_ARGS__).msghandle_print(msgpack_pk,para_name_vec);\
}\
template <typename Stream>\
void msghandle_checkcode(msghandle::packer<Stream>& msgpack_pk,const std::string& k) const\
{\
    std::vector<std::string> para_name_vec = paralist();\
    msgpack_pk.stream() << k <<"\n";\
    std::string tmp_k = k;\
    char c = ' ';\
    int count = 0;\
    std::string pix=" ";\
    while ((count = tmp_k.find(c)) != std::string::npos) {\
        ++count;\
        tmp_k.erase(count - 1, 1);  \
        pix+=c;\
    }\
    for (int var = 0; var < para_name_vec.size(); ++var) {\
        para_name_vec[var]=pix;\
    }\
    msghandle::type::make_define_array(__VA_ARGS__).msghandle_checkcode(msgpack_pk,para_name_vec);\
}\
template <typename Stream>\
void msghandle_reflectget(msghandle::packer<Stream>& msgpack_pk,const std::string& k, const std::string filter) const\
{\
    std::vector<std::string> para_name_vec = paralist();\
    msghandle::type::make_define_array(__VA_ARGS__).msghandle_reflectget(msgpack_pk,para_name_vec,filter);\
}

#endif // defined(MSGHANDLE_USE_DEFINE_MAP)


#include "msghandle/v1/adaptor/define_decl.hpp"
#include "msghandle/v2/adaptor/define_decl.hpp"
#include "msghandle/v3/adaptor/define_decl.hpp"

#endif // MSGHANDLE_DEFINE_DECL_HPP
