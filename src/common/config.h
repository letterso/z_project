#pragma once

#ifndef __CONFIG_H__
#define __CONFIG_H__

#include <memory>
#include <mutex>
#include <optional>

#include <yaml-cpp/yaml.h>
#include "logger.h"

class Config {
private:
    static std::unique_ptr<Config> instance_;
    static std::once_flag init_flag_;
    YAML::Node param_file_;

    Config() = default;  // private constructor makes a singleton
public:
    ~Config();  // close the file when deconstructing

    // Delete copy constructor and assignment operator
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;

    /**
     * @brief Get singleton instance of Config (thread-safe)
     * @return Reference to Config singleton
     */
    static Config& GetInstance();

    /**
     * @brief Load YAML configuration file
     * @param filename Path to the YAML file
     * @return true if loaded successfully, false otherwise
     */
    bool LoadFile(const std::string& filename);

    // Deprecated: Use GetInstance().LoadFile() instead
    static bool SetParameterFile(const std::string &filename);

    /**
     * @brief Get parameter value with optional default
     * @tparam T Type of the parameter
     * @param father_key Parent key in YAML
     * @param child_key Child key in YAML
     * @return Optional containing the value, or std::nullopt if not found
     */
    template <typename T>
    static std::optional<T> GetParam(const std::string& father_key, const std::string& child_key) {
        auto& instance = GetInstance();
        if (!instance.param_file_) {
            LOGE("[CONFIG] Configuration not loaded. Call LoadFile() first.");
            return std::nullopt;
        }

        try {
            auto node = instance.param_file_[father_key][child_key];
            if (!node) {
                LOGW("[CONFIG] Parameter {}.{} does not exist.", father_key, child_key);
                return std::nullopt;
            }
            return node.as<T>();
        } catch (const YAML::Exception& e) {
            LOGE("[CONFIG] Error reading {}.{}: {}", father_key, child_key, e.what());
            return std::nullopt;
        }
    }

    /**
     * @brief Get parameter value with default fallback
     * @tparam T Type of the parameter
     * @param father_key Parent key in YAML
     * @param child_key Child key in YAML
     * @param default_value Default value if parameter not found
     * @return Parameter value or default value
     */
    template <typename T>
    static T GetParam(const std::string& father_key, const std::string& child_key, const T& default_value) {
        auto result = GetParam<T>(father_key, child_key);
        if (result.has_value()) {
            return result.value();
        }
        LOGW("[CONFIG] Using default value for {}.{}", father_key, child_key);
        return default_value;
    }
};

#endif  // __CONFIG_H__
