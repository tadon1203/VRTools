#pragma once

#include <memory>
#include <vector>

#include "ESPComponents/IESPElement.hpp"
#include "Features/IFeature.hpp"
#include "VisualsUtils.hpp"

class ESP : public IFeature {
public:
    ESP();

    void onRender() override;
    void onMenuRender() override;
    void onLoadConfig(const nlohmann::json& j) override;
    void onSaveConfig(nlohmann::json& j) const override;

    [[nodiscard]] bool isBoneEspEnabled() const;
    [[nodiscard]] bool isBox3dEnabled() const;

private:
    std::vector<std::unique_ptr<IESPElement>> m_elements;

    VisualsUtils::ColorMode m_mode = VisualsUtils::ColorMode::Rainbow;
    Gradient m_customGradient;

    float m_speed     = 0.5f;
    float m_thickness = 1.0f;
    bool m_outline    = true;

    Color m_box2DColor = Color(1.0f, 0.0f, 0.0f, 1.0f);
    Color m_box3DColor = Color(1.0f, 1.0f, 1.0f, 0.3f);
    Color m_boneColor  = Color(1.0f, 1.0f, 1.0f, 1.0f);
    Color m_textColor  = Color(1.0f, 1.0f, 1.0f, 1.0f);
};
