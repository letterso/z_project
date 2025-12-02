#include "file_manager.h"
#include "logger.h"

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

std::optional<fs::path> FileManager::GetConfigFilePath(std::string_view filename) const noexcept {
    try {
        if (filename.empty()) {
            spdlog::warn("[FILE_MANAGER] Empty filename provided");
            return std::nullopt;
        }

        // Get the executable path
        auto exe_path = GetExecutablePath();
        if (!exe_path.has_value()) {
            spdlog::error("[FILE_MANAGER] Could not determine executable path");
            return std::nullopt;
        }

        // Get the directory of the executable (bin directory)
        auto exe_dir = GetDirectory(exe_path.value());
        spdlog::debug("[FILE_MANAGER] Executable directory: {}", exe_dir.string());

        // Go up one level from the executable directory to get the project root
        // Assuming structure: <root>/install/bin/<executable>
        auto parent_dir = GetDirectory(exe_dir);
        spdlog::debug("[FILE_MANAGER] Parent directory: {}", parent_dir.string());

        // Construct the config file path
        auto config_path = parent_dir / "config" / filename;
        spdlog::debug("[FILE_MANAGER] Config file path: {}", config_path.string());

        // Canonicalize the path if it exists
        if (fs::exists(config_path)) {
            return fs::canonical(config_path);
        }
        return config_path;
    } catch (const std::exception& e) {
        spdlog::error("[FILE_MANAGER] Error getting config file path: {}", e.what());
        return std::nullopt;
    }
}

bool FileManager::ConfigFileExists(std::string_view filename) const noexcept {
    try {
        auto config_path = GetConfigFilePath(filename);
        if (!config_path.has_value()) {
            return false;
        }
        return fs::exists(config_path.value());
    } catch (const std::exception& e) {
        spdlog::error("[FILE_MANAGER] Error checking config file existence: {}", e.what());
        return false;
    }
}
