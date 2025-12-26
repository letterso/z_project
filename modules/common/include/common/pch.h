#ifndef __PCH_H__
#define __PCH_H__

// C++ Standard Library - Core types and utilities
#include <iostream>
#include <string>
#include <string_view>
#include <vector>
#include <deque>
#include <list>
#include <map>
#include <unordered_map>
#include <set>
#include <unordered_set>
#include <memory>
#include <functional>
#include <algorithm>
#include <numeric>

// C++ Standard Library - Time and threading
#include <chrono>
#include <thread>
#include <mutex>
#include <atomic>
#include <condition_variable>

// C++ Standard Library - File system and IO
#include <filesystem>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <cstdlib>

// Third-party Libraries
#include <spdlog/spdlog.h>
#include <spdlog/fmt/fmt.h>
#include <yaml-cpp/yaml.h>

// Note: Eigen is not included here by default because it's very heavy.
// It's better to include it only where needed, or in a specific PCH for modules that use it extensively.
// #include <Eigen/Core>
// #include <Eigen/Geometry>

#endif //__PCH_H__
