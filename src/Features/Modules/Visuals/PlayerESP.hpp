#pragma once

#include "Features/IFeature.hpp"
#include "VisualsUtils.hpp"

class PlayerESP : public IFeature {
public:
    PlayerESP();

    void onRender() override;
    void onMenuRender() override;

private:
    bool m_drawBox      = true;
    bool m_drawBox3D    = false;
    bool m_fillBox      = false;
    bool m_drawName     = true;
    bool m_drawDistance = true;

    bool m_useRankColor = true;
    Color m_customColor = Color(1.0f, 1.0f, 1.0f, 1.0f);
    Color m_fillColor   = Color(1.0f, 1.0f, 1.0f, 0.3f);

    VisualsUtils::VisualsStyle m_style;
};
