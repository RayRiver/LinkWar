
local Soldier = import("..objects.soldier")

local SCENE_NAME = "GameScene"

local SceneClass = class(SCENE_NAME, function() 
    return display.newScene(SCENE_NAME) 
end)

function SceneClass:ctor()
    self.soldiers = {}
    self.enemies = {}
    
    self:createGrid()
    self:createLauncherArea()
    
    self:scheduleUpdateWithPriorityLua(handler(self, self.onFrame), 0)
    
    self:schedule(handler(self, self.onEvaluate), 1.0)
end

function SceneClass:onEnter()

end

function SceneClass:onEvaluate()
    for _, soldier in pairs(self.soldiers) do
        soldier:onEvaluate(self)
    end
    for _, enemy in pairs(self.enemies) do
        enemy:onEvaluate(self)
    end
end

function SceneClass:onFrame()
    for _, soldier in pairs(self.soldiers) do
        local desiredX, desiredY = soldier:getDesiredPosition()
        soldier:setPosition(desiredX, desiredY)
    
        local x, y = soldier:getPosition()
        if cc.rectContainsPoint(self.oppoLauncherArea, cc.p(x, y)) then
            self:removeChild(soldier)
            table.removebyvalue(self.soldiers, soldier, true)
        end
    end
    for _, enemy in pairs(self.enemies) do
        local desiredX, desiredY = enemy:getDesiredPosition()
        enemy:setPosition(desiredX, desiredY)
        
        local x, y = enemy:getPosition()
        if cc.rectContainsPoint(self.selfLauncherArea, cc.p(x, y)) then
            self:removeChild(enemy)
            table.removebyvalue(self.enemies, enemy, true)
            
        end
    end
end

function SceneClass:onCreateSoldier(pos)
    local soldier = Soldier.new()
    soldier:setPosition(pos)
    self:addChild(soldier)
    self.soldiers[#self.soldiers+1] = soldier
end

function SceneClass:onCreateEnemy(pos)
    local enemy = Soldier.new(true)
    enemy:setPosition(pos)
    self:addChild(enemy)
    self.enemies[#self.enemies+1] = enemy
end

function SceneClass:createGrid()
    local layer = cc.LayerColor:create({r=50,g=50,b=50,a=255})
    self:addChild(layer)

    local draw = cc.DrawNode:create()
    for i = 1, display.width/40 do
        local x = i*40
        draw:drawSegment(cc.p(x, display.top), cc.p(x, display.bottom), 0.5, {r=0.5, g=0.5, b=0.5, a=0.5})
    end
    for i = 1, display.height/40 do
        local y = i*40
        draw:drawSegment(cc.p(display.left, y), cc.p(display.right, y), 0.5, {r=0.5, g=0.5, b=0.5, a=0.5})
    end
    self:addChild(draw)
end

function SceneClass:createLauncherArea()
    local draw = cc.DrawNode:create()
    
    draw:drawPolygon(
        {cc.p(40, 0), cc.p(40, 160), cc.p(600, 160), cc.p(600, 0)}, 4, 
        {r=0.0, g=1.0, b=0.0, a=0.2}, 
        1.0, {r=0.0, g=1.0, b=0.0, a=0.4})

    draw:drawPolygon(
        {cc.p(40, display.height), cc.p(40, display.height-160), cc.p(600, display.height-160), cc.p(600, display.height)}, 4, 
        {r=1.0, g=0.0, b=0.0, a=0.2}, 
        1.0, {r=1.0, g=0.0, b=0.0, a=0.4})
        
    self.selfLauncherArea = cc.rect(40,0,560,160)
    self.oppoLauncherArea = cc.rect(40,display.height-160,560,160)
    
    local touchListener = cc.EventListenerTouchOneByOne:create()
    touchListener:registerScriptHandler(function(touch, event) 
        if cc.rectContainsPoint(self.selfLauncherArea, touch:getLocation()) then
            self:onCreateSoldier(touch:getLocation()) 
            
            local x = math.random(self.oppoLauncherArea.x, self.oppoLauncherArea.x+self.oppoLauncherArea.width)
            local y = math.random(self.oppoLauncherArea.y, self.oppoLauncherArea.y+self.oppoLauncherArea.height)
            self:onCreateEnemy(cc.p(x, y))
        end
        return true 
    end, cc.Handler.EVENT_TOUCH_BEGAN) 
    touchListener:registerScriptHandler(function() end, cc.Handler.EVENT_TOUCH_ENDED)
    local eventDispatcher = self:getEventDispatcher()
    eventDispatcher:addEventListenerWithSceneGraphPriority(touchListener, self)
    
    self:addChild(draw) 
end

return SceneClass
