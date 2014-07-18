
local SCENE_NAME = "BattleScene"

local SceneClass = class(SCENE_NAME, function() 
    return display.newScene(SCENE_NAME) 
end)

function SceneClass:ctor()
    local mapManager = MapManager:getInstance()
    self:addChild(mapManager)
    
    local objectManager = GameObjectManager:getInstance()
    self:addChild(objectManager)
    
    local ret = mapManager:loadData("config/map1.json")
    
    -- 持续产生逻辑帧
    LogicFrameManager:getInstance():init()

    printInfo("map manager load return " .. tostring(ret))

end

function SceneClass:onEnter()

end

return SceneClass
