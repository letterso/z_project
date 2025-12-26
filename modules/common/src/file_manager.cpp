#include "common/file_manager.hpp"
#include "common/logger.hpp"

std::unique_ptr<FileManager> FileManager::instance_ = nullptr;
std::once_flag FileManager::init_flag_;

FileManager& FileManager::GetInstance() {
    std::call_once(FileManager::init_flag_, []() {
        // Use new since constructor is private
        FileManager::instance_ = std::unique_ptr<FileManager>(new FileManager());
    });
    return *FileManager::instance_;
}

std::optional<fs::path> FileManager::GetExecutablePath() const noexcept {
    try {
        auto exe_path = fs::canonical("/proc/self/exe");
        spdlog::debug("[FILE_MANAGER] Executable path: {}", exe_path.string());
        return exe_path;
    } catch (const std::exception& e) {
        spdlog::error("[FILE_MANAGER] Failed to get executable path: {}", e.what());
        return std::nullopt;
    }
}

fs::path FileManager::GetDirectory(const fs::path& file_path) const noexcept {
    try {
        if (file_path.empty()) {
            spdlog::warn("[FILE_MANAGER] Empty file path provided");
            return fs::path();
        }
        auto parent = file_path.parent_path();
        spdlog::debug("[FILE_MANAGER] Parent directory: {}", parent.string());
        return parent;
    } catch (const std::exception& e) {
        spdlog::error("[FILE_MANAGER] Error getting directory: {}", e.what());
        return fs::path();
    }
}

std::optional<fs::path> FileManager::GetConfigDirectory() const noexcept {
    try {
        // Get the executable path
        auto exe_path = GetExecutablePath();
        if (!exe_path.has_value()) {
            spdlog::error("[FILE_MANAGER] Could not determine executable path");
            return std::nullopt;
        }

        // Get the directory of the executable (bin directory)
        auto exe_dir = GetDirectory(exe_path.value());
        
        // Go up one level from the executable directory to get the install root
        // Assuming structure: <root>/install/bin/<executable>
        auto parent_dir = GetDirectory(exe_dir);
        
        // Construct the config directory path
        auto config_dir = parent_dir / "config";
        spdlog::debug("[FILE_MANAGER] Config directory: {}", config_dir.string());
        
        // Create the directory if it doesn't exist
        if (!fs::exists(config_dir)) {
            fs::create_directories(config_dir);
            spdlog::info("[FILE_MANAGER] Created config directory: {}", config_dir.string());
        }
        
        return fs::canonical(config_dir);
    } catch (const std::exception& e) {
        spdlog::error("[FILE_MANAGER] Error getting config directory: {}", e.what());
        return std::nullopt;
    }
}

std::optional<fs::path> FileManager::GetLogDirectory() const noexcept {
    try {
        // Get the executable path
        auto exe_path = GetExecutablePath();
        if (!exe_path.has_value()) {
            spdlog::error("[FILE_MANAGER] Could not determine executable path");
            return std::nullopt;
        }

        // Get the directory of the executable (bin directory)
        auto exe_dir = GetDirectory(exe_path.value());
        
        // Go up one level from the executable directory to get the install root
        // Assuming structure: <root>/install/bin/<executable>
        auto parent_dir = GetDirectory(exe_dir);
        
        // Construct the log directory path
        auto log_dir = parent_dir / "log";
        spdlog::debug("[FILE_MANAGER] Log directory: {}", log_dir.string());
        
        // Create the directory if it doesn't exist
        if (!fs::exists(log_dir)) {
            fs::create_directories(log_dir);
            spdlog::info("[FILE_MANAGER] Created log directory: {}", log_dir.string());
        }
        
        return fs::canonical(log_dir);
    } catch (const std::exception& e) {
        spdlog::error("[FILE_MANAGER] Error getting log directory: {}", e.what());
        return std::nullopt;
    }
}
