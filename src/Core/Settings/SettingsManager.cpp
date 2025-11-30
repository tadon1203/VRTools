#include "SettingsManager.hpp"

#include <Windows.h>

#include <filesystem>
#include <fstream>

#include "Core/Logger.hpp"
#include "Core/Version.hpp"

namespace fs = std::filesystem;

SettingsManager& SettingsManager::instance() {
    static SettingsManager inst;
    return inst;
}

void SettingsManager::registerHandler(ISettingsHandler* handler) { m_handlers.push_back(handler); }

void SettingsManager::saveAll() {
    nlohmann::json root = nlohmann::json::object();

    for (auto* handler : m_handlers) {
        try {
            nlohmann::json section;
            handler->getSettings().save(section); // Automatic save
            root[handler->getSectionName()] = section;
        } catch (const std::exception& e) {
            Logger::instance().error("Failed to gather settings for {}: {}", handler->getSectionName(), e.what());
        }
    }
    saveToFile(root);
}

void SettingsManager::loadAll() {
    nlohmann::json root = loadFromFile();
    if (root.empty()) {
        return;
    }

    for (auto* handler : m_handlers) {
        std::string section = handler->getSectionName();
        if (root.contains(section)) {
            try {
                handler->getSettings().load(root[section]); // Automatic load
            } catch (const std::exception& e) {
                Logger::instance().error("Failed to load settings section {}: {}", section, e.what());
            }
        }
    }
}

nlohmann::json SettingsManager::loadFromFile() {
    char exePath[MAX_PATH];
    GetModuleFileNameA(nullptr, exePath, MAX_PATH);
    fs::path filePath = fs::path(exePath).parent_path() / Version::PROJECT_NAME / m_filename;

    if (!fs::exists(filePath)) {
        return nlohmann::json::object();
    }

    std::ifstream file(filePath);
    if (!file.is_open()) {
        return nlohmann::json::object();
    }

    nlohmann::json root;
    try {
        file >> root;
    } catch (...) {
        return nlohmann::json::object();
    }
    return root;
}

void SettingsManager::saveToFile(const nlohmann::json& root) {
    char exePath[MAX_PATH];
    GetModuleFileNameA(nullptr, exePath, MAX_PATH);
    fs::path filePath = fs::path(exePath).parent_path() / Version::PROJECT_NAME / m_filename;

    try {
        fs::create_directories(filePath.parent_path());
        std::ofstream file(filePath);
        if (file.is_open()) {
            file << root.dump(4);
            Logger::instance().info("Settings saved to disk.");
        }
    } catch (const std::exception& e) {
        Logger::instance().error("Failed to write settings: {}", e.what());
    }
}
