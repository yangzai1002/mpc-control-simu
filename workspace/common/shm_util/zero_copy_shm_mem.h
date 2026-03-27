#pragma once

#include <cassert>
#include <cstddef>
#include <functional>
#include <iostream>
#include <map>
#include <string>
#include <type_traits>
#include <typeindex>
#include <typeinfo>
#include <unordered_map>
#include <vector>

#include "shm_util/shm_transmitter.h"

namespace vector_hack {
    constexpr char zero_copy_key[] = "zero_cpoy";
    constexpr int  zero_release_id = 1000;
    constexpr int  non_zero_release_id = -1000;
} // namespace vector_hack

template <typename T, typename = void> struct has_mbuffer_info : std::false_type {};

template <typename T> struct has_mbuffer_info<T, std::void_t<decltype(std::declval<T>().mbufData)>> : std::true_type {};

struct vector_impl {
    void *start;
    void *finish;
    void *end_of_storage;
};

template <typename T> void set_zero_release_shm_id(T &data) {
    if (has_mbuffer_info<T>::value) {
        data.mbufData = vector_hack::zero_release_id;
    }
}

template <typename T> void set_non_zero_release_shm_id(T &data) {
    if (has_mbuffer_info<T>::value) {
        data.mbufData = vector_hack::non_zero_release_id;
    }
}

template <typename T> bool zero_copy_data_vector(T &source, void *data, size_t data_size) {
    if (!has_mbuffer_info<T>::value) {
        return false;
    }
    assert(source.rawData.size() == 0);
    set_zero_release_shm_id(source);
    vector_impl *v_ptr = reinterpret_cast<vector_impl *>(&source.rawData);
    v_ptr->start = data;
    v_ptr->finish = static_cast<char *>(data) + data_size;
    v_ptr->end_of_storage = v_ptr->finish;
    return true;
}

template <typename T> bool zero_copy_vector(std::vector<T> &source, void *data, size_t data_size) {
    assert(source.size() == 0);
    vector_impl *v_ptr = reinterpret_cast<vector_impl *>(&source);
    v_ptr->start = data;
    v_ptr->finish = static_cast<char *>(data) + data_size;
    v_ptr->end_of_storage = v_ptr->finish;
}

template <typename T> void release(std::vector<T> &vec) {
    std::cout << "[ZERO] release non-const vector" << std::endl;
    auto v_ptr = reinterpret_cast<vector_impl *>(&vec);
    v_ptr->start = nullptr;
    v_ptr->finish = nullptr;
    v_ptr->end_of_storage = nullptr;
}

template <typename T> void release(const std::vector<T> &vec) {
    std::cout << "[ZERO] release const vector (hack!)" << std::endl;
    auto non_const_vec = const_cast<std::vector<T> *>(&vec);
    auto v_ptr = reinterpret_cast<vector_impl *>(non_const_vec);
    v_ptr->start = nullptr;
    v_ptr->finish = nullptr;
    v_ptr->end_of_storage = nullptr;
}

template <typename T> bool check_release(T &data) {
    if (has_mbuffer_info<T>::value && data.mbufData == vector_hack::zero_release_id) {
        return true;
    }
    return false;
}

// template <typename T>
//  void safe_release(T &data)
// {
//     std::cout << "[ZERO] safe_release unimplemented type ???: " << typeid(T).name() << std::endl;
// }

class AnySafeReleasable {
public:
    virtual ~AnySafeReleasable() = default;
};

template <typename T> class SafeReleasable : public AnySafeReleasable {
public:
    T &ref;
    SafeReleasable(T &obj) : ref(obj) {
    }
};

using SafeReleaseFunc = std::function<void(AnySafeReleasable *)>;

inline std::unordered_map<std::type_index, SafeReleaseFunc> &safe_release_registry() {
    static std::unordered_map<std::type_index, SafeReleaseFunc> map;
    return map;
}

template <typename T> void safe_release(T &obj) {
    auto it = safe_release_registry().find(std::type_index(typeid(T)));
    if (it != safe_release_registry().end()) {
        SafeReleasable<T> wrapper(obj);
        it->second(&wrapper);
    } else {
        std::cout << "[ZERO] safe_release unregistry type ???: " << typeid(T).name() << std::endl;
    }
}

template <typename T> void register_safe_release(std::function<void(T &)> f) {
    std::cout << "[ZERO] registry type ???: " << typeid(T).name() << std::endl;
    safe_release_registry()[std::type_index(typeid(T))] = [f](AnySafeReleasable *base) {
        auto *typed = dynamic_cast<SafeReleasable<T> *>(base);
        f(typed->ref);
    };
}

// template <>
//  void safe_release(SensorImage &data)
// {
//     if (!check_release(data))
//     {
//         return;
//     }
//     std::cout << "[ZERO] safe_release(T &SensorImage)" << std::endl;
//     release(data.data);
// }

// template <>
//  void safe_release(SensorImageList &data)
// {
//     std::cout << "[ZERO] safe_release(T &SensorImageList)" << std::endl;
//     for (size_t i = 0; i < data.images.size(); i++)
//     {
//         safe_release(data.images[i]);
//     }
// }

class ZeroCopyShmMem {
private:
    std::map<std::string, unsigned char *> data_base_ptr_map;

public:
    ZeroCopyShmMem() {
    }

    ~ZeroCopyShmMem() {
    }

    unsigned char *GetAllocShmMem(const std::string &key_name, const int &buffer_size, const int &current_index = 0,
                                  int queen_size = 10) {
        if (queen_size < 0) {
            queen_size = 10;
        }
        int index = current_index % queen_size;
        if (data_base_ptr_map.find(key_name) == data_base_ptr_map.end()) {
            int shm_id = avos::common::SharedMemory::sharedMemoryCreateOrGet(
                avos::common::SharedMemory::BKDRHash(key_name.c_str()), (buffer_size / 4096 + 1) * 4096 * queen_size);
            if (shm_id == -1) {
                std::cout << "error cannot get shm pointer " << key_name << "," << buffer_size << "," << queen_size
                          << std::endl;
                return nullptr;
            }

            data_base_ptr_map[key_name] = (unsigned char *)avos::common::SharedMemory::sharedMemoryAttach(shm_id);
        }
        return (data_base_ptr_map[key_name] + index * buffer_size);
    }

    // void SyncShmPtr(SensorImage &data)
    // {
    //     std::cout << "here 111------------------------------------------" << std::endl;
    //     // 特定处理
    //     SyncImageInfo(data);
    // }

    // void SyncShmPtr(SensorImageList &data)
    // {
    //     std::cout << "here 222------------------------------------------" << std::endl;
    //     // 特定处理
    //     SyncImageListInfo(data);
    // }

    template <typename T> static void SyncShmPtr(T &data) {
        std::cout << "here------------------------------------------" << std::endl;
        // 通用处理
    }

    template <typename T> int SyncDataVectorInfo(T &image) {
        if (!has_mbuffer_info<T>::value) {
            std::cout << "Error type type ???: " << typeid(T).name() << std::endl;
            return -1;
        }

        if (image.frameID == "" || image.dataSize <= 0 || image.seq < 0) {
            std::cout << "key|buffer|index error:" << image.frameID << "|" << image.dataSize << "|" << image.seq
                      << std::endl;
            return 0;
        }
        unsigned char *data = ZeroCopyShmMem::GetAllocShmMem(image.frameID, image.dataSize, image.seq, 10);
        if (data == nullptr) {
            std::cout << __FILE__ << ":" << __LINE__ << ": "
                      << "error" << std::endl;
            return -1;
        }

        zero_copy_data_vector(image, data, image.dataSize);

        return 0;
    }
};
