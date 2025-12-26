#pragma once

#ifndef __FILE_MANAGER_H__
#define __FILE_MANAGER_H__

#include <string>
#include <memory>
#include <optional>
#include <filesystem>
#include <mutex>

namespace fs = std::filesystem;

class FileManager {
private:
    static std::unique_ptr<FileManager> instance_;
    static std::once_flag init_flag_;

    // Private constructor
    FileManager() = default;

public:
    ~FileManager() = default;

    // Delete copy constructor and assignment operator
    FileManager(const FileManager&) = delete;
    FileManager& operator=(const FileManager&) = delete;

    /**
     * @brief Get singleton instance of FileManager (thread-safe)
     * @return Reference to FileManager singleton
     */
    static FileManager& GetInstance();

    /**
     * @brief Get the absolute path of the current executable
     * @return Optional containing the absolute path, or std::nullopt if failed
     */
    std::optional<fs::path> GetExecutablePath() const noexcept;

    /**
     * @brief Get the directory from a file path
     * @param file_path The file path
     * @return The parent directory path
     */
    fs::path GetDirectory(const fs::path& file_path) const noexcept;

    /**
     * @brief Get the absolute path of the config directory
     * @return Optional containing the absolute path to the config directory, or std::nullopt if failed
     * @note Directory structure: <root>/install/config
     */
    std::optional<fs::path> GetConfigDirectory() const noexcept;

    /**
     * @brief Get the absolute path of the log directory
     * @return Optional containing the absolute path to the log directory, or std::nullopt if failed
     * @note Directory structure: <root>/install/log
     */
    std::optional<fs::path> GetLogDirectory() const noexcept;
};

#endif // __FILE_MANAGER_H__
