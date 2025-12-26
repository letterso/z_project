
#include "common/timecost_utils.hpp"
#include <numeric>

// --- TicToc Implementation ---

TicToc::TicToc() {
    tic();
}

void TicToc::tic() {
    start = std::chrono::steady_clock::now();
}

double TicToc::toc() {
    end = std::chrono::steady_clock::now();
    std::chrono::duration<double> elapsed_seconds = end - start;
    return elapsed_seconds.count() * 1000;
}

// --- TimeCost Implementation ---

TimeCost::TimeCost(const std::string &tag, const Unit &unit)
    : m_tag(tag), m_unit(unit), m_start_time(std::chrono::steady_clock::now()) {}

TimeCost::~TimeCost() {
    auto end_time = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - m_start_time).count();
    switch (m_unit) {
        case Unit::S:
            LOGI("[{}], cost: {} s", m_tag, duration / 1e9);
            break;
        case Unit::MS:
            LOGI("[{}], cost: {} ms", m_tag, duration / 1e6);
            break;
        case Unit::US:
            LOGI("[{}], cost: {} us", m_tag, duration / 1e3);
            break;
        case Unit::NS:
            LOGI("[{}], cost: {} ns", m_tag, duration);
            break;
    }
}

double TimeCost::GetTime() {
    auto end_time = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - m_start_time).count();
    switch (m_unit) {
        case Unit::S:
            return duration / 1e9;
        case Unit::MS:
            return duration / 1e6;
        case Unit::US:
            return duration / 1e3;
        case Unit::NS:
            return duration;
    }
    return 0.0;
}

// --- TimeCostEva::TimerRecord Implementation ---
std::map<std::string, TimeCostEva::TimerRecord> TimeCostEva::records_;

TimeCostEva::TimerRecord::TimerRecord(const std::string& name, double time_usage)
    : func_name_(name) {
    time_usage_in_ms_.emplace_back(time_usage);
}

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

void TimeCostEva::SetEvaluatet(const std::string& func_name, const double& time_usage) {
    if (records_.find(func_name) != records_.end()) {
        records_[func_name].time_usage_in_ms_.emplace_back(time_usage);
    } else {
        records_.insert({func_name, TimerRecord(func_name, time_usage)});
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