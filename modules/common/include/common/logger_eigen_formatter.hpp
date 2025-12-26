#ifndef __LOGGER_EIGEN_FORMATTER_H__
#define __LOGGER_EIGEN_FORMATTER_H__

#include <spdlog/spdlog.h>
#include <Eigen/Core>
#include <Eigen/Geometry>
#include <sstream>

// Custom formatter for Eigen::DenseBase types
template<typename T>
struct fmt::formatter<T, std::enable_if_t<std::is_base_of<Eigen::DenseBase<T>, T>::value, char>> {
    constexpr auto parse(format_parse_context& ctx) -> decltype(ctx.begin()) {
        return ctx.end();
    }

    template <typename FormatContext>
    auto format(const Eigen::DenseBase<T>& mat, FormatContext& ctx) const -> decltype(ctx.out()) {
        std::stringstream ss;
        ss << mat;
        return fmt::format_to(ctx.out(), "{}", ss.str());
    }
};

#endif // __LOGGER_EIGEN_FORMATTER_H__
