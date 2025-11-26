#include "SettingsManager.hpp"

#include <filesystem>
#include <fstream>

#include <Windows.h>

#include "Core/Logger.hpp"
#include "Core/Version.hpp"

namespace fs = std::filesystem;

fs::path getSettingsFilePath(const std::string& filename) {
    char exePath[MAX_PATH];
    GetModuleFileNameA(nullptr, exePath, MAX_PATH);
    return fs::path(exePath).parent_path() / Version::PROJECT_NAME / filename;
}

SettingsManager& SettingsManager::instance() {
    static SettingsManager inst;
    return inst;
}

nlohmann::json SettingsManager::loadFromFile(const std::string& filename) {
    const auto filePath = getSettingsFilePath(filename);
    nlohmann::json root = nlohmann::json::object();

    if (!fs::exists(filePath)) {
        Logger::instance().warn("Settings file not found: {}. A new one will be created.", filePath.string());
        return root;
    }

    std::ifstream file(filePath);
    if (!file.is_open()) {
        Logger::instance().error("Failed to open settings file: {}", filePath.string());
        return root;
    }

    if (file.peek() == std::ifstream::traits_type::eof()) {
        Logger::instance().warn("Settings file is empty. Resetting to defaults.");
        return root;
    }

    try {
        file >> root;

        if (!root.is_object() && !root.is_null()) {
            throw std::runtime_error("Root element is not a JSON object.");
        }

        Logger::instance().info("Settings loaded from {}", filePath.string());
    } catch (const nlohmann::json::parse_error& e) {
        Logger::instance().error("Settings file corrupted (JSON parse error: {}). Backing up and resetting.", e.what());
        file.close();

        try {
            const auto backupPath = filePath.string() + ".bak";
            fs::copy_file(filePath, backupPath, fs::copy_options::overwrite_existing);
            Logger::instance().info("Corrupted settings backed up to {}", backupPath);
        } catch (const std::exception& ex) {
            Logger::instance().error("Failed to backup corrupted settings: {}", ex.what());
        }

        return nlohmann::json::object();
    } catch (const std::exception& e) {
        Logger::instance().error("Failed to load settings: {}", e.what());
        return nlohmann::json::object();
    }

    return root;
}

void SettingsManager::saveToFile(const nlohmann::json& root, const std::string& filename) {
    const auto filePath = getSettingsFilePath(filename);

    try {
        fs::create_directories(filePath.parent_path());
        std::ofstream file(filePath);
        if (file.is_open()) {
            file << root.dump(4);
            Logger::instance().info("Settings saved to {}", filePath.string());
        }
    } catch (const std::exception& e) {
        Logger::instance().error("Failed to save settings: {}", e.what());
    }
}
