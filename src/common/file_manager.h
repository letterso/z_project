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
     * @brief Get the absolute path of a config file
     * @param filename The name of the config file (e.g., "params.yaml")
     * @return Optional containing the absolute path to the config file, or std::nullopt if failed
     */
    std::optional<fs::path> GetConfigFilePath(std::string_view filename) const noexcept;

    /**
     * @brief Check if a config file exists
     * @param filename The name of the config file
     * @return true if the config file exists, false otherwise
     */
    bool ConfigFileExists(std::string_view filename) const noexcept;
};

#endif // __FILE_MANAGER_H__
