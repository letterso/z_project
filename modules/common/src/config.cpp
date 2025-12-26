#include "common/config.hpp"

std::unique_ptr<Config> Config::instance_ = nullptr;
std::once_flag Config::init_flag_;

Config& Config::GetInstance() {
    std::call_once(init_flag_, []() {
        instance_ = std::unique_ptr<Config>(new Config());
    });
    return *instance_;
}

bool Config::LoadFile(const std::string& filename) {
    try {
        param_file_ = YAML::LoadFile(filename);
        if (param_file_.IsNull()) {
            LOGE("[CONFIG] Parameter file {} is empty or invalid.", filename);
            return false;
        }
        LOGI("[CONFIG] Successfully loaded configuration from {}", filename);
        return true;
    } catch (const YAML::BadFile& e) {
        LOGE("[CONFIG] File {} does not exist or cannot be opened.", filename);
        return false;
    } catch (const YAML::Exception& e) {
        LOGE("[CONFIG] YAML parsing error in {}: {}", filename, e.what());
        return false;
    } catch (const std::exception& e) {
        LOGE("[CONFIG] Unexpected error loading {}: {}", filename, e.what());
        return false;
    }
}

// Deprecated: Kept for backward compatibility
bool Config::SetParameterFile(const std::string& filename) {
    return GetInstance().LoadFile(filename);
}

Config::~Config() {}