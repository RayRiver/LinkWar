
local OBJECT_NAME = "Soldier"

local ObjectClass = class(OBJECT_NAME, function() 
    return cc.Node:create()
end)

function ObjectClass:ctor(isEnemy)
    cc(self)
    self:addComponent("components.behavior.animation_controller"):exportMethods()
    self:addComponent("components.behavior.astar"):exportMethods()
    
    self:setNodeEventEnabled(true)
    
    self.isEnemy = isEnemy
    
    self.target = nil
    self.locked = {}
    
    self.desiredX = nil
    self.desiredY = nil 
    
    --self:setScale(2.0)
    
    ccs.ArmatureDataManager:getInstance():addArmatureFileInfo("animation/ani_green_link.ExportJson")
    --self:loadAnimation("ani_green_link") 
    
    local drawNode = cc.DrawNode:create()
    drawNode:setAnchorPoint(0.5, 0.5)
    self:addChild(drawNode)
    
    self:setContentSize(40, 40) 
    local rect = self:rect()
    drawNode:setContentSize(self:getContentSize()) 
    
    if isEnemy then
        --self:playAnimation("move_down")
        --self:getArmature():setColor({r=255, g=0, b=0})
        drawNode:drawPolygon(cc.rect2points(rect), 4, {r=1,g=0,b=0,a=1}, 1, {r=1,g=1,b=1,a=1})
    else
        --self:playAnimation("move_up")
        drawNode:drawPolygon(cc.rect2points(rect), 4, {r=0,g=1,b=0,a=1}, 1, {r=1,g=1,b=1,a=1})
    end
    
    
    self:scheduleUpdateWithPriorityLua(handler(self, self.onFrame), 0)
end

function ObjectClass:setGrid(n)
    self.grid = n
end

function ObjectClass:setTargetGrid(n)
    self.targetGrid = n
end

function ObjectClass:getDesiredPosition()
    if not self.desiredX or not self.desiredY then
        self.desiredX, self.desiredY = self:getPosition()
    end
    return self.desiredX, self.desiredY
end

function ObjectClass:getDesiredPositionX()
    local x, y = self:getDesiredPosition()
    return x
end

function ObjectClass:getDesiredPositionY()
    local x, y = self:getDesiredPosition()
    return y 
end

function ObjectClass:desiredPos()
    local x, y = self:getDesiredPosition()
    return cc.p(x, y)
end

function ObjectClass:onExit()
    for _, lock_object in pairs(self.locked) do
        lock_object.target = nil
    end
end

function ObjectClass:onFrame()
    if self.path and #self.path > 0 then
        local grid = self.path[#self.path]
        if self.grid == grid then
            self.path[#self.path] = nil
            if #self.path == 0 then
                return
            end
            grid = self.path[#self.path]
        end
        
        local x, y = self.scene:grid2pos(grid)
        local pt = cc.pSub(cc.p(x,y), self:pos())
        pt = cc.pNormalize(pt)
        self.desiredX = x+pt.x*2
        self.desiredY = y+pt.y*2
        
    end
    do return end



    if self.state == 1 then
        local x, y = self:getPosition()
        if self.isEnemy then
            --self:setPosition(x, y-2)
            self.desiredX = x
            self.desiredY = y-2
        else
            if self.target then
                local pt = cc.pSub(self.target:pos(), self:pos())
                pt = cc.pNormalize(pt)
                self.desiredX = x+pt.x*2
                self.desiredY = y+pt.y*2
            else
                self.desiredX = x
                self.desiredY = y+2
            end
        end
    end
end

function ObjectClass:onEvaluate(scene)
    self.scene = scene

    if self.targetGrid and self.targetGrid ~= self.grid then
        local x = self.grid % scene.w
        local y = (self.grid - x) / scene.w
        local target_x = self.targetGrid % scene.w
        local target_y = (self.targetGrid - target_x) / scene.w
        self:setMapInfo(scene.grid, scene.w, scene.h)
        self.path = self:findPath(y*scene.w+x, target_y*scene.w+target_x)
    end
    do return end

    self.state = 1
    
    if self.isEnemy then
    
    elseif not self.target then
        local min_length2
        local found_enemy
        for _, enemy in pairs(scene.enemies) do
            local length2 = cc.pGetLength2(cc.pSub(enemy:pos(), self:pos()))
            if not min_length2 or min_length2 > length2 then
                min_length2 = length2
                found_enemy = enemy
            end
        end

        if found_enemy then
            self.target = found_enemy      
            found_enemy.locked[#found_enemy.locked+1] = self
        end
    end
end

return ObjectClass
