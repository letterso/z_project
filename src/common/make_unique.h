#ifndef __MAKE_UNIQUE_H__
#define __MAKE_UNIQUE_H__

#include <memory>

#if __cplusplus >= 201402L
// 如果支持C++14，则使用标准库中的std::make_unique
#else
// 如果不支持C++14，则使用手动实现的make_unique
namespace std {
    template <typename T, typename... Args>
    std::unique_ptr<T> make_unique(Args &&...args) {
        return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
    }
} // namespace std
#endif
#endif // __MAKE_UNIQUE_H__
