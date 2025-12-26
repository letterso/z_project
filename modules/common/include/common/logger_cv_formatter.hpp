#ifndef __LOGGER_CV_FORMATTER_H__
#define __LOGGER_CV_FORMATTER_H__

#include <spdlog/spdlog.h>
#include <opencv2/core/mat.hpp>
#include <sstream>

// Custom formatter for cv::Mat types
template <>
struct fmt::formatter<cv::Mat> {
    // 解析格式字符串，如果不需要自定义格式选项，可以直接返回 ctx.end()
    constexpr auto parse(format_parse_context& ctx) -> decltype(ctx.begin()) {
        // 例如，你可以支持像 "{:brief}" 或 "{:full}" 这样的格式选项
        // 这里为了简化，我们不处理任何格式选项
        return ctx.end();
    }

    // 格式化 cv::Mat 对象
    template <typename FormatContext>
    auto format(const cv::Mat& mat, FormatContext& ctx) const -> decltype(ctx.out()) {
        std::stringstream ss;
        ss << "Mat (rows=" << mat.rows << ", cols=" << mat.cols << ", type=" << mat.type() << ", channels=" << mat.channels() << ")\n";

        // 决定要打印的行数和列数，避免输出过大的矩阵
        int rows_to_print = std::min(mat.rows, 5); // 示例：最多打印5行
        int cols_to_print = std::min(mat.cols, 8); // 示例：最多打印8列

        for (int i = 0; i < rows_to_print; ++i) {
            ss << "[";
            for (int j = 0; j < cols_to_print; ++j) {
                // 根据Mat的类型来获取元素并打印
                // 这是一个简化版本，实际应用中可能需要更完善的类型处理
                switch (mat.type() & CV_MAT_DEPTH_MASK) {
                    case CV_8U: ss << static_cast<int>(mat.at<uchar>(i, j)); break;
                    case CV_8S: ss << static_cast<int>(mat.at<schar>(i, j)); break;
                    case CV_16U: ss << mat.at<ushort>(i, j); break;
                    case CV_16S: ss << mat.at<short>(i, j); break;
                    case CV_32S: ss << mat.at<int>(i, j); break;
                    case CV_32F: ss << mat.at<float>(i, j); break;
                    case CV_64F: ss << mat.at<double>(i, j); break;
                    default: ss << "?"; break; // 未知类型
                }
                if (j < cols_to_print - 1) {
                    ss << ", ";
                }
            }
            ss << "]";
            if (i < rows_to_print - 1) {
                ss << "\n";
            }
        }

        if (mat.rows > rows_to_print || mat.cols > cols_to_print) {
            ss << "\n... (truncated)";
        }

        return fmt::format_to(ctx.out(), "{}", ss.str());
    }
};

#endif // __LOGGER_CV_FORMATTER_H__
