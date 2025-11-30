#pragma once

#include <string>

#include <imgui.h>

namespace VRC {

    enum class PlayerRank {
        Visitor,
        NewUser,
        User,
        Known,
        Trusted,
        // Special ranks derived from tags
        ProbableTroll,
        Troll,
        Moderator
    };

    inline ImColor getRankColor(PlayerRank rank) {
        switch (rank) {
        case PlayerRank::Visitor:
            return { 202, 202, 202, 255 };
        case PlayerRank::NewUser:
            return { 22, 119, 254, 255 };
        case PlayerRank::User:
            return { 43, 207, 92, 255 };
        case PlayerRank::Known:
            return { 254, 122, 66, 255 };
        case PlayerRank::Trusted:
            return { 129, 67, 230, 255 };
        case PlayerRank::ProbableTroll:
            return { 128, 0, 0, 255 };
        case PlayerRank::Troll:
            return { 255, 0, 0, 255 };
        case PlayerRank::Moderator:
            return { 255, 0, 0, 255 };
        default:
            return { 202, 202, 202, 255 };
        }
    }

    inline std::string getRankName(PlayerRank rank) {
        switch (rank) {
        case PlayerRank::Visitor:
            return "Visitor";
        case PlayerRank::NewUser:
            return "New User";
        case PlayerRank::User:
            return "User";
        case PlayerRank::Known:
            return "Known User";
        case PlayerRank::Trusted:
            return "Trusted User";
        case PlayerRank::ProbableTroll:
            return "Probable Troll";
        case PlayerRank::Troll:
            return "Troll";
        case PlayerRank::Moderator:
            return "MODERATOR";
        default:
            return "Unknown";
        }
    }
}
