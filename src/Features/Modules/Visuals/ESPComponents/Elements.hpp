#pragma once

#include <fmt/format.h>

#include "IESPElement.hpp"

class BoxElement : public IESPElement {
public:
    BoxElement()
        : IESPElement("Box 2D", ESPAnchor::Center) {}

    ImVec2 getSize(const DrawPlayer& p, const ESPContext& ctx) override { return { 0, 0 }; }

    void render(ImDrawList* dl, const DrawPlayer& p, const ESPContext& ctx, ImVec2 pos) override {
        VisualsUtils::ScreenRect r = { p.rectMin, p.rectMax };
        VisualsUtils::drawBox2D(dl, r, ctx.styleBox2D); // Use Box2D Style
    }
};

class Box3DElement : public IESPElement {
public:
    Box3DElement()
        : IESPElement("Box 3D", ESPAnchor::Center) {
        m_enabled = false;
    }

    ImVec2 getSize(const DrawPlayer& p, const ESPContext& ctx) override { return { 0, 0 }; }

    void render(ImDrawList* dl, const DrawPlayer& p, const ESPContext& ctx, ImVec2 pos) override {
        VisualsUtils::ScreenCube cube;
        bool valid = true;
        for (int i = 0; i < 8; ++i) {
            if (p.corners3d[i].x <= -9000) {
                valid = false;
                break;
            }
            cube.corners[i] = p.corners3d[i];
        }
        if (valid) {
            VisualsUtils::drawBox3D(dl, cube, ctx.styleBox3D); // Use Box3D Style
        }
    }
};

class SkeletonElement : public IESPElement {
public:
    SkeletonElement()
        : IESPElement("Skeleton", ESPAnchor::Center) {
        m_enabled = false;
    }

    ImVec2 getSize(const DrawPlayer& p, const ESPContext& ctx) override { return { 0, 0 }; }

    void render(ImDrawList* dl, const DrawPlayer& p, const ESPContext& ctx, ImVec2 pos) override {
        if (!p.bones.empty()) {
            VisualsUtils::drawSkeleton(dl, p.bones, ctx.boneStyle); // Use Bone Style
        }
    }
};

class NametagElement : public IESPElement {
public:
    NametagElement()
        : IESPElement("Nametag", ESPAnchor::Top) {}

    ImVec2 getSize(const DrawPlayer& p, const ESPContext& ctx) override {
        return ImGui::GetFont()->CalcTextSizeA(ctx.textStyle.fontSize, FLT_MAX, 0.0f, p.name.c_str());
    }

    void render(ImDrawList* dl, const DrawPlayer& p, const ESPContext& ctx, ImVec2 pos) override {
        VisualsUtils::drawText(dl, pos, p.name, ctx.textStyle, false);
    }
};

class DistanceElement : public IESPElement {
public:
    DistanceElement()
        : IESPElement("Distance", ESPAnchor::Bottom) {}

    ImVec2 getSize(const DrawPlayer& p, const ESPContext& ctx) override {
        std::string s = fmt::format("[{:.0f}m]", p.distance);
        return ImGui::GetFont()->CalcTextSizeA(ctx.textStyle.fontSize, FLT_MAX, 0.0f, s.c_str());
    }

    void render(ImDrawList* dl, const DrawPlayer& p, const ESPContext& ctx, ImVec2 pos) override {
        std::string s = fmt::format("[{:.0f}m]", p.distance);
        VisualsUtils::drawText(dl, pos, s, ctx.textStyle, false);
    }
};
