#pragma once

#include <vector>

#include "ESPTypes.hpp"
#include "Features/IFeature.hpp"
#include "PlayerESPObject.hpp"

class PlayerESP : public IFeature {
public:
    PlayerESP();

    void onRender() override;
    void onMenuRender() override;

    enum class Element { Box2D, Box3D, Skeleton, Name, Distance };

private:
    void renderElement(ImDrawList* dl, const PlayerESPObject& obj, Element element);
    void renderOrderEditor();
    void renderStyleEditor(const char* label, ESPStyle& style, bool isText = false);

    ESPStyle m_box2DStyle;
    ESPStyle m_box3DStyle;
    ESPStyle m_skeletonStyle;
    ESPStyle m_nameStyle;
    ESPStyle m_distanceStyle;

    std::vector<Element> m_renderOrder;
};
