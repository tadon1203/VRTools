#pragma once

enum class FeatureCategory {
    Visuals,
    Player,
    World,
    Exploits,
    Misc,
    System,
};

inline const char* toString(FeatureCategory category) {
    switch (category) {
    case FeatureCategory::Visuals:
        return "Visuals";
    case FeatureCategory::Player:
        return "Player";
    case FeatureCategory::World:
        return "World";
    case FeatureCategory::Exploits:
        return "Exploits";
    case FeatureCategory::Misc:
        return "Misc";
    case FeatureCategory::System:
        return "System";
    default:
        return "Unknown";
    }
}

inline const char* getCategoryDescription(FeatureCategory category) {
    switch (category) {
    case FeatureCategory::Visuals:
        return "Visual modifications and information display features.";
    case FeatureCategory::Player:
        return "Features that directly affect your player or other players (e.g., Fly, Speedhack).";
    case FeatureCategory::World:
        return "Features that affect the game world and environment.";
    case FeatureCategory::Exploits:
        return "Features that leverage game exploits or vulnerabilities.";
    case FeatureCategory::Misc:
        return "Miscellaneous features that don't fit into other categories.";
    case FeatureCategory::System:
        return "Features related to the hack client itself.";
    default:
        return "Unknown";
    }
}
