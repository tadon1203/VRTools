#pragma once

#include "Features/IFeature.hpp"
#include "VisualsUtils.hpp"

class PlayerESP : public IFeature {
public:
    PlayerESP();

    void onRender() override;
    void onMenuRender() override;

private:
    void renderStyleEditor(const char* label, VisualsUtils::ESPStyle& style, bool isText = false);

    VisualsUtils::ESPStyle m_box2DStyle;
    VisualsUtils::ESPStyle m_box3DStyle;
    VisualsUtils::ESPStyle m_skeletonStyle;
    VisualsUtils::ESPStyle m_nameStyle;
    VisualsUtils::ESPStyle m_distanceStyle;
};
