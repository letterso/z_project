#pragma once

#ifndef __CONFIG_H__
#define __CONFIG_H__

#include <memory>
#include <fstream>

#include <yaml-cpp/yaml.h>
#include "logger.h"

class Config {
private:
    static std::shared_ptr<Config> config_;
    YAML::Node param_file_;

    Config() {}  // private constructor makes a singleton
public:
    ~Config();  // close the file when deconstructing

    static bool SetParameterFile(const std::string &filename);

    // access the parameter values
    static YAML::Node GetParam(const std::string &key) {
        return Config::config_->param_file_[key];
    }
    // template <typename T>
    // static T GetParam(const std::string &key) {
    //     try {
    //         return Config::config_->param_file_[key].as<T>();
    //     } catch (...) {
    //         spdlog::error("[CONFIG] {} parameter does not exist.", key);
    //     }
    // }
    // template <typename T>
    // static T GetParam(const std::string &key, T intial) {
    //     T value = intial;
    //     if (Config::config_->param_file_[key]) {
    //         value = Config::config_->param_file_[key].as<T>();
    //     } else {
    //         // LOGI("[CONFIG] {0} parameter does not exist, use default value {1}", key, value);
    //     }
    //     return value;
    // }
    template <typename T>
    static T GetParam(const std::string &father_key, const std::string &child_key) {
        try {
            // LOGI("[CONFIG] {} {}: {}", father_key, child_key, Config::config_->param_file_[father_key][child_key].as<T>());
            return Config::config_->param_file_[father_key][child_key].as<T>();
        } catch (...) {
            LOGE("[ CONFIG ] {0} {1} parameter does not exist.", father_key, child_key);
            return T();
        }
    }
    template <typename T>
    static T GetParam(const std::string &father_key, const std::string &child_key, T intial) {
        T value = intial;
        if (Config::config_->param_file_[father_key][child_key]) {
            value = Config::config_->param_file_[father_key][child_key].as<T>();
        } else {
            LOGW("[ CONFIG ] {0} {1} parameter does not exist, use default value {2}", father_key, child_key, value);
        }
        return value;
    }
};

#endif  // __CONFIG_H__
