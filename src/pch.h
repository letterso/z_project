#ifndef __PCH_H__
#define __PCH_H__

// Standard Library
#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <chrono>
#include <thread>
#include <atomic>
#include <filesystem>
#include <sstream>
#include <iomanip>
#include <cstdlib>

// Third-party Libraries
#include <spdlog/spdlog.h>
#include <yaml-cpp/yaml.h>

// Note: Eigen is not included here by default because it's very heavy.
// It's better to include it only where needed, or in a specific PCH for modules that use it extensively.
// #include <Eigen/Core>
// #include <Eigen/Geometry>


#endif //__PCH_H__
