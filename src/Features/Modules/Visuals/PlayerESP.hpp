#pragma once

#include "ESPDef.hpp"
#include "Features/IFeature.hpp"

class PlayerESP : public IFeature {
public:
    PlayerESP();

    void onRender() override;
    void onMenuRender() override;

private:
    void renderStyleEditor(const char* label, ESPStyle& style, bool isText = false);

    ESPStyle m_box2DStyle;
    ESPStyle m_box3DStyle;
    ESPStyle m_skeletonStyle;
    ESPStyle m_nameStyle;
    ESPStyle m_distanceStyle;
};
