#include <Geode/Geode.hpp>

#include <Geode/modify/EditorPauseLayer.hpp>

using namespace geode::prelude;

class $modify(ProEditorPauseLayer, EditorPauseLayer) {
    
    bool createTrigger(GameObject* obj, const CCPoint& pos, int id) {
        auto action = m_editorLayer->m_levelSettings->m_effectManager->getColorAction(id);
        
        if (!action) {
            return false;
        }
        
        auto newObj = static_cast<EffectGameObject*>(m_editorLayer->m_editorUI->createObject(899, pos));
        
        newObj->m_duration = 0.f;
        newObj->m_usesBlending = action->m_blending;
        newObj->m_copyColorID = action->m_copyID;
        newObj->m_usesPlayerColor1 = action->m_playerColor == 1;
        newObj->m_usesPlayerColor2 = action->m_playerColor == 2;
        newObj->m_copyOpacity = action->m_copyOpacity;
        newObj->m_legacyHSV = action->m_legacyHSV;
        newObj->m_triggerTargetColor = action->m_fromColor;
        newObj->m_targetColor = id;
        newObj->m_opacity = action->m_fromOpacity;
        newObj->m_hsvValue = action->m_copyHSV;
        
        m_editorLayer->removeSpecial(newObj);
        m_editorLayer->addSpecial(newObj);
        
        return true;
    }
  
    void onCreateChannels(CCObject*) {
        std::vector<GameObject*> objects;
        
        if (m_editorLayer->m_editorUI->m_selectedObjects) {
            for (auto object : CCArrayExt<GameObject*>(m_editorLayer->m_editorUI->m_selectedObjects)) {
                objects.push_back(object);
            }
        }
        
        if (m_editorLayer->m_editorUI->m_selectedObject) {
            objects.push_back(m_editorLayer->m_editorUI->m_selectedObject);
        }
        
        if (objects.empty()) {
            return;
        }
        
        auto pos = CCPoint{0, -m_editorLayer->m_objectLayer->getPositionY() + m_editorLayer->getContentHeight() * 0.7f};
        
        for (const auto& obj : objects) {
            if (obj->getPositionX() < pos.x || obj == objects[0]) {
                pos.x = obj->getPositionX();
            }
        }
        
        pos.x -= 30.f;
        
        std::unordered_set<int> alreadyAdded;
        
        for (auto obj : objects) {
            if (obj->m_baseColor) {
                int id = obj->m_baseColor->m_colorID;
                
                if (!alreadyAdded.contains(id) && createTrigger(obj, pos, id)) {
                    alreadyAdded.insert(id);
                    pos.y += 30.f;
                }
            }
            
            if (!obj->m_detailColor) {
                continue;
            }
            
            int id = obj->m_detailColor->m_colorID;
            
            if (!alreadyAdded.contains(id) && createTrigger(obj, pos, id)) {
                alreadyAdded.insert(id);
                pos.y += 30.f;
            }
        }
        
        if (static_cast<int>(alreadyAdded.size()) > 0) {
            m_editorLayer->m_triggersChanged = true;
            m_editorLayer->m_colorTriggersChanged = true;
            m_editorLayer->m_pulseTriggersChanged = true;
            m_editorLayer->m_alphaTriggersChanged = true;
            m_editorLayer->m_spawnTriggersChanged = true;
        }
    }
    
    bool init(LevelEditorLayer* p0) {
        if (!EditorPauseLayer::init(p0)) {
            return false;
        }
        
        if (auto menu = getChildByID("small-actions-menu")) {
            auto spr = ButtonSprite::create("Create\nColors", 30, 0, 0.4f, true, "bigFont.fnt", "GJ_button_04.png", 30.f);
            spr->setScale(0.8f);
            
            auto btn = CCMenuItemSpriteExtra::create(spr, this, menu_selector(ProEditorPauseLayer::onCreateChannels));
            btn->setZOrder(-1);
            
            menu->addChild(btn);
            menu->updateLayout();
        }
        
        return true;
    }
    
};