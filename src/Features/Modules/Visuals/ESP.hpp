#pragma once

#include "Core/Events/RenderEvents.hpp"
#include "ESPComponents/IESPElement.hpp"
#include "Features/IFeature.hpp"

class ESP : public IFeature {
public:
    ESP();
    void onEnable() override;
    void onMenuRender() override;

private:
    void onRender(RenderEvent& e);
    std::vector<std::unique_ptr<IESPElement>> m_elements;
};
