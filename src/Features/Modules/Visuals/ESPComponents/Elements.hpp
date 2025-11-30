#pragma once

#include <fmt/format.h>

#include "IESPElement.hpp"

class BoxElement : public IESPElement {
public:
    BoxElement()
        : IESPElement("Box 2D", ElementType::Geometry, ESPAnchor::Center) {
        m_style.colorMode    = VisualsUtils::ColorMode::Rainbow;
        m_style.primaryColor = Color(1, 0, 0, 1);
        m_style.outline      = true;
    }

    ImVec2 getSize(const DrawPlayer& p, const ESPContext& ctx) override { return { 0, 0 }; }

    void render(ImDrawList* dl, const DrawPlayer& p, const ESPContext& ctx, ImVec2 pos) override {
        auto style                 = getStyle(p);
        style.time                 = ctx.curTime;
        VisualsUtils::ScreenRect r = { p.rectMin, p.rectMax };
        VisualsUtils::drawBox2D(dl, r, style);
    }
};

class Box3DElement : public IESPElement {
public:
    Box3DElement()
        : IESPElement("Box 3D", ElementType::Geometry, ESPAnchor::Center) {
        m_enabled            = false;
        m_style.colorMode    = VisualsUtils::ColorMode::Solid;
        m_style.primaryColor = Color(1.0f, 1.0f, 1.0f, 0.3f);
        m_style.outline      = false;
    }

    ImVec2 getSize(const DrawPlayer& p, const ESPContext& ctx) override { return { 0, 0 }; }

    void render(ImDrawList* dl, const DrawPlayer& p, const ESPContext& ctx, ImVec2 pos) override {
        auto style = getStyle(p);
        style.time = ctx.curTime;
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
            VisualsUtils::drawBox3D(dl, cube, style);
        }
    }
};

class SkeletonElement : public IESPElement {
public:
    SkeletonElement()
        : IESPElement("Skeleton", ElementType::Geometry, ESPAnchor::Center) {
        m_enabled            = false;
        m_style.colorMode    = VisualsUtils::ColorMode::Solid;
        m_style.primaryColor = Color(1, 1, 1, 1);
    }

    ImVec2 getSize(const DrawPlayer& p, const ESPContext& ctx) override { return { 0, 0 }; }

    void render(ImDrawList* dl, const DrawPlayer& p, const ESPContext& ctx, ImVec2 pos) override {
        if (!p.bones.empty()) {
            auto style = getStyle(p);
            style.time = ctx.curTime;
            VisualsUtils::drawSkeleton(dl, p.bones, style);
        }
    }
};

class NametagElement : public IESPElement {
public:
    NametagElement()
        : IESPElement("Nametag", ElementType::Text, ESPAnchor::Top) {
        m_style.colorMode    = VisualsUtils::ColorMode::Solid;
        m_style.primaryColor = Color(1, 1, 1, 1);
        m_style.textOutline  = true;
    }

    ImVec2 getSize(const DrawPlayer& p, const ESPContext& ctx) override {
        return ImGui::GetFont()->CalcTextSizeA(m_style.fontSize, FLT_MAX, 0.0f, p.name.c_str());
    }

    void render(ImDrawList* dl, const DrawPlayer& p, const ESPContext& ctx, ImVec2 pos) override {
        auto style = getStyle(p);
        style.time = ctx.curTime;
        VisualsUtils::drawText(dl, pos, p.name, style, false);
    }
};

class DistanceElement : public IESPElement {
public:
    DistanceElement()
        : IESPElement("Distance", ElementType::Text, ESPAnchor::Bottom) {
        m_style.colorMode    = VisualsUtils::ColorMode::Solid;
        m_style.primaryColor = Color(1, 1, 1, 1);
        m_style.textOutline  = true;
    }

    ImVec2 getSize(const DrawPlayer& p, const ESPContext& ctx) override {
        std::string s = fmt::format("[{:.0f}m]", p.distance);
        return ImGui::GetFont()->CalcTextSizeA(m_style.fontSize, FLT_MAX, 0.0f, s.c_str());
    }

    void render(ImDrawList* dl, const DrawPlayer& p, const ESPContext& ctx, ImVec2 pos) override {
        std::string s = fmt::format("[{:.0f}m]", p.distance);
        auto style    = getStyle(p);
        style.time    = ctx.curTime;
        VisualsUtils::drawText(dl, pos, s, style, false);
    }
};
