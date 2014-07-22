
local SCENE_NAME = "BattleScene"

local SceneClass = class(SCENE_NAME, function() 
    return display.newScene(SCENE_NAME) 
end)

function SceneClass:ctor()
    local mapManager = MapManager:getInstance()
    self:addChild(mapManager)
        
    -- 加载地图数据
    local ret = mapManager:loadData("map/map2.tmx")
    
    -- 对象管理器
    local objectManager = GameObjectManager:getInstance()
    mapManager:getView():addChild(objectManager, 100000)
    
    -- 持续产生逻辑帧
    LogicFrameManager:getInstance():init()

    printInfo("map manager load return " .. tostring(ret))

end

function SceneClass:onEnter()

end

return SceneClass
