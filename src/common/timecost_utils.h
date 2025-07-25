#ifndef __TIMERCOST_UTILS_H__
#define __TIMERCOST_UTILS_H__

#include <ctime>
#include <cstdlib>
#include <chrono>
#include <fstream>
#include <map>
#include <numeric>
#include <string>

#include "logger.h"

/**
* @brief 时间评估
* 
*/
class TimeCostEva {
public:
    struct TimerRecord {
        TimerRecord() = default;
        TimerRecord(const std::string& name, double time_usage): func_name_(name) {
            time_usage_in_ms_.emplace_back(time_usage);
        }
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

/**
* @brief 范围内时间评估
* 
*/
class TicToc
{
public:
    TicToc()
    {
        tic();
    }

    void tic()
    {
        start = std::chrono::steady_clock::now();
    }

    double toc()
    {
        end = std::chrono::steady_clock::now();
        std::chrono::duration<double> elapsed_seconds = end - start;
        return elapsed_seconds.count() * 1000;
    }

private:
    std::chrono::time_point<std::chrono::steady_clock> start, end;
};

// raii机制实现的耗时评估
class TimeCost {
public:
    enum class Unit : uint8_t {
        S = 0,  // 秒
        MS = 1, // 毫秒
        US = 2, // 微秒
        NS = 3  // 纳秒
    };
    explicit TimeCost(const std::string &m_tag, const Unit &unit = Unit::MS)
        : m_tag(m_tag), m_unit(unit),
            m_start_time(std::chrono::steady_clock::now()) {}

    ~TimeCost() {
        auto end_time = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - m_start_time).count();
        if (Unit::S == m_unit) {
            LOGI("[{}], cost: {} s", m_tag, duration / (1000.0 * 1000.0 * 1000.0));
        } else if (Unit::MS == m_unit) {
            LOGI("[{}], cost: {} ms", m_tag, duration / (1000.0 * 1000.0));
        } else if (Unit::NS == m_unit) {
            LOGI("[{}], cost: {} us", m_tag, duration / 1000.0);
        } else {
            LOGI("[{}], cost: {} ns", m_tag, duration);
        }
    }

    double GetTime(){
        auto end_time = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - m_start_time).count();
        if (Unit::S == m_unit) {
            return duration / (1000.0 * 1000.0 * 1000.0);
        } else if (Unit::MS == m_unit) {
            return duration / (1000.0 * 1000.0 );
        } else if (Unit::NS == m_unit) {
            return duration / 1000.0;
        } else {
            return duration;
        }
    }

private:
    std::string m_tag;
    Unit m_unit;
    std::chrono::time_point<std::chrono::steady_clock> m_start_time;
};

#endif  // __TIMERCOST_UTILS_H__
