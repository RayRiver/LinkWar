
local COMPONENT_NAME = "AnimationController"

local Component = import("..component")
local ComponentClass = class(COMPONENT_NAME, Component)

ComponentClass.PlayMode = 
{
    Default = -1,
    Once = 0,
    Loop = 1,
}

function ComponentClass:ctor()
    ComponentClass.super.ctor(self, COMPONENT_NAME)

    self.armature = nil
    self.armatureName = ""
end

function ComponentClass:loadAnimation(armatureName, animationEventCallback)
    local owner = self:getTarget()
    if owner then
        if armatureName == self.armatureName then
            return self
        end
    
        local armature = ccs.Armature:create(armatureName)
        if not armature then
            printError("animation_controller load error: %s", armatureName)
            return self
        end
        
        if self.armature then
            owner:removeChild(self.armature, true)
            self.armature = nil
        end

        armature:setAnchorPoint(0.5, 0.5)
        owner:addChild(armature)
        self.armature = armature
        self.armatureName = armatureName
        
        if animationEventCallback then
            local function animationEvent(armature, movementType, movementID)
                animationEventCallback(movementType, movementID)
            end
            self.armature:getAnimation():setMovementEventCallFunc(animationEvent)
        end
    end
    return self
end

function ComponentClass:playAnimation(movementName, playMode)
    if self.armature then
        if self.armature:getAnimation():getAnimationData():getMovement(movementName) then
            playMode = playMode or ComponentClass.PlayMode.Default
            self.armature:getAnimation():play(movementName, -1, playMode)
        end
    end
    return self
end

function ComponentClass:setMovementEventCallFunc(animationEventCallback)
    if animationEventCallback then
        local function animationEvent(armature, movementType, movementID)
            animationEventCallback(movementType, movementID)
        end
        self.armature:getAnimation():setMovementEventCallFunc(animationEvent)
    end
end

function ComponentClass:pauseAnimation()
    if self.armature then
        self.armature:getAnimation():pause()
    end
end

function ComponentClass:resumeAnimation()
    if self.armature then
        self.armature:getAnimation():resume()
    end
end

function ComponentClass:stopAnimation()
    if self.armature then
        self.armature:getAnimation():stop()
    end
end

function ComponentClass:getArmature()
    return self.armature
end

function ComponentClass:exportMethods()
    self:exportMethods_({
        "loadAnimation",
        "playAnimation",
        "pauseAnimation",
        "resumeAnimation",
        "stopAnimation",
        "getArmature",
    })
    return self.target_
end

function ComponentClass:onBind_()

end

function ComponentClass:onUnbind_()

end

return ComponentClass
