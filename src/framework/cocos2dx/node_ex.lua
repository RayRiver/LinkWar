
local c = cc
local Node = c.Node

function Node:pos()
    local x, y = self:getPosition()
    return cc.p(x, y)
end

function Node:rect()
    local x, y = self:getPosition()
    local size = self:getContentSize()
    local anchorPoint = self:getAnchorPoint()
    return cc.rect(x-size.width*anchorPoint.x, y-size.height*anchorPoint.y, size.width, size.height)
end

function Node:schedule(callback, interval)
    local seq = cc.Sequence:create(
        cc.DelayTime:create(interval),
        cc.CallFunc:create(callback)
    )
    local action = cc.RepeatForever:create(seq)
    self:runAction(action)
    return action
end

function Node:onEnter()
end

function Node:onExit()
end

function Node:onEnterTransitionFinish()
end

function Node:onExitTransitionStart()
end

function Node:onCleanup()
end

function Node:setNodeEventEnabled(enabled, listener)
    if enabled then
        if not listener then
            listener = function(event)
                local name = event
                if name == "enter" then
                    self:onEnter()
                elseif name == "exit" then
                    self:onExit()
                elseif name == "enterTransitionFinish" then
                    self:onEnterTransitionFinish()
                elseif name == "exitTransitionStart" then
                    self:onExitTransitionStart()
                elseif name == "cleanup" then
                    self:onCleanup()
                end
            end
        end
        self:registerScriptHandler(listener)
    else
        self:unregisterScriptHandler()
    end
    return self
end
