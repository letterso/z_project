#ifndef __TIMERCOST_UTILS_H__
#define __TIMERCOST_UTILS_H__

#include <string>
#include <vector>
#include <chrono>
#include <map>

#include "common/logger.h"

/**
* @brief 范围内时间评估
*/
class TicToc {
public:
    TicToc();
    void tic();
    double toc();

private:
    std::chrono::time_point<std::chrono::steady_clock> start, end;
};

// raii机制实现的耗时评估
class TimeCost {
public:
    enum class Unit : uint8_t { S, MS, US, NS };
    explicit TimeCost(const std::string &m_tag, const Unit &unit = Unit::MS);
    ~TimeCost();
    double GetTime();

private:
    std::string m_tag;
    Unit m_unit;
    std::chrono::time_point<std::chrono::steady_clock> m_start_time;
};

/**
* @brief 时间评估
*/
class TimeCostEva {
public:
    struct TimerRecord {
        TimerRecord() = default;
        TimerRecord(const std::string& name, double time_usage);
        std::string func_name_;
        std::vector<double> time_usage_in_ms_;
    };

    /**
    * call F and save its time usage
    * @tparam F
    * @param func
    * @param func_name
    */
    template <class F>
    static void Evaluate(F&& func, const std::string& func_name) {
        auto t1 = std::chrono::steady_clock::now();
        std::forward<F>(func)();
        auto t2 = std::chrono::steady_clock::now();
        auto time_used = std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1).count() * 1000;

        if (records_.find(func_name) != records_.end()) {
            records_[func_name].time_usage_in_ms_.emplace_back(time_used);
        } else {
            records_.insert({func_name, TimerRecord(func_name, time_used)});
        }
    }

    /// print the run time
    static void SetEvaluatet(const std::string& func_name, const double& time_usage);

    /// print the run time
    static void PrintAll();

    /// dump to a log file
    static void DumpIntoFile();

    /// get the average time usage of a function
    static double GetMeanTime(const std::string& func_name);

    /// clean the records
    static void Clear() { records_.clear(); }

private:
    static std::map<std::string, TimerRecord> records_;
};

#endif  // __TIMERCOST_UTILS_H__
