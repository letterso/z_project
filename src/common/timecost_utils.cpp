#include "timecost_utils.h"

std::map<std::string, TimeCostEva::TimerRecord> TimeCostEva::records_;

void TimeCostEva::PrintAll() {
    LOGI("\033[32m>>> ===== Printing run time =====\033[0m");
    for (const auto& r : records_) {
        LOGI("> [ {} ] average time usage: {:.6f} ms , called times: {}", r.first,
             std::accumulate(r.second.time_usage_in_ms_.begin(), r.second.time_usage_in_ms_.end(), 0.0) /
                 double(r.second.time_usage_in_ms_.size()),
             r.second.time_usage_in_ms_.size());
    }
    LOGI("\033[32m>>> ===== Printing run time end =====\033[0m");
}

void TimeCostEva::DumpIntoFile() {
    // 写标题
    size_t max_length = 0;
    std::ostringstream header_oss;
    for (const auto& iter : records_) {
        header_oss << iter.first << ", ";
        if (iter.second.time_usage_in_ms_.size() > max_length) {
            max_length = iter.second.time_usage_in_ms_.size();
        }
    }
    EVALOGTIME("{}", header_oss.str());

    // 写数据
    for (size_t i = 0; i < max_length; ++i) {
        std::ostringstream time_data_oss;
        for (const auto& iter : records_) {
            if (i < iter.second.time_usage_in_ms_.size()) {
                time_data_oss << iter.second.time_usage_in_ms_[i] << ",";
            } else {
                time_data_oss << ",";
            }
        }
        EVALOGTIME("{}", time_data_oss.str());
    }
}

double TimeCostEva::GetMeanTime(const std::string& func_name) {
    if (records_.find(func_name) == records_.end()) {
        return 0.0;
    }

    auto r = records_[func_name];
    return std::accumulate(r.time_usage_in_ms_.begin(), r.time_usage_in_ms_.end(), 0.0) /
           double(r.time_usage_in_ms_.size());
}