#include "config.h"

bool Config::SetParameterFile(const std::string &filename)
{
    if (config_ == nullptr)
        config_ = std::shared_ptr<Config>(new Config);

    std::ifstream param_file(filename);
    if (!param_file) {
        LOGE("[CONFIG] {} does not exist.", filename);
        return false;
    }
    config_->param_file_ = YAML::LoadFile(filename);
    if (config_->param_file_.IsNull())
    {
        LOGE("[CONFIG] parameter file {} does not exist.", filename);
        return false;
    }
    return true;
}

Config::~Config()
{}

std::shared_ptr<Config> Config::config_ = nullptr;