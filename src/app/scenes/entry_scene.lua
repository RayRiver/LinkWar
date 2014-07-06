
local SCENE_NAME = "EntryScene"

local SceneClass = class(SCENE_NAME, function() 
    return display.newScene(SCENE_NAME)
end)

function SceneClass:ctor()
    local label = cc.Label:createWithSystemFont("Start", "Arial", 64)
    local menuItem = cc.MenuItemLabel:create(label)
    menuItem:registerScriptTapHandler(function(tag) 
        app:enterScene("game_scene")
    end)
        
    local menu = cc.Menu:create(menuItem)
    menu:alignItemsVerticallyWithPadding(5)
    menu:setPosition(display.cx, display.cy)
    self:addChild(menu)
end

return SceneClass
