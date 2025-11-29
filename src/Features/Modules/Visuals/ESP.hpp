#pragma once

#include "Features/IFeature.hpp"
#include "Utils/ColorUtils.hpp"
#include "VisualsUtils.hpp"

class ESP : public IFeature {
public:
    ESP();

    void onRender() override;
    void onMenuRender() override;

    void onLoadConfig(const nlohmann::json& j) override;
    void onSaveConfig(nlohmann::json& j) const override;

    // shit code omg
    [[nodiscard]] bool isBoneEspEnabled() const { return m_enabled && m_bones; }

private:
    bool m_box2D                   = true;
    VisualsUtils::ColorMode m_mode = VisualsUtils::ColorMode::Rainbow;

    Color m_solidColor = Color(1.0f, 0.0f, 0.0f, 1.0f);
    Gradient m_customGradient;

    float m_speed     = 0.5f;
    float m_thickness = 1.0f;

    bool m_outline = true;
    bool m_inline  = true;

    // Bone ESP
    bool m_bones      = false;
    Color m_boneColor = Color(1.0f, 1.0f, 1.0f, 1.0f);
};
