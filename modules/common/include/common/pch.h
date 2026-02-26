#ifndef __PCH_H__
#define __PCH_H__

// C++ Standard Library - Core types and utilities
#include <algorithm>
#include <deque>
#include <functional>
#include <iostream>
#include <list>
#include <map>
#include <memory>
#include <numeric>
#include <set>
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <vector>

// C++ Standard Library - Time and threading
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <mutex>
#include <thread>

// C++ Standard Library - File system and IO
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <sstream>

// Third-party Libraries
#include <spdlog/fmt/fmt.h>
#include <spdlog/spdlog.h>
#include <yaml-cpp/yaml.h>

// Note: Eigen is not included here by default because it's very heavy.
// It's better to include it only where needed, or in a specific PCH for modules
// that use it extensively. #include <Eigen/Core> #include <Eigen/Geometry>

#endif  //__PCH_H__
