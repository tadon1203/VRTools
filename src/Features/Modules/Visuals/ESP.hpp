#pragma once

#include <memory>
#include <vector>

#include "ESPComponents/IESPElement.hpp"
#include "Features/IFeature.hpp"

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
};
