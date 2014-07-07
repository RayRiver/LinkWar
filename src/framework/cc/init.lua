
--[[--

初始化 cc 扩展

cc 扩展在 cocos2dx C++ API 和 quick 基本模块的基础上，提供了符合脚本风格的事件接口、组件架构等扩展。

]]

-- init base classes
cc.Registry   = import(".registry")
cc.GameObject = import(".game_object")

-- init components
local components = {
    "components.behavior.state_machine",
    "components.behavior.animation_controller",
    "components.behavior.astar",
}
for _, packageName in ipairs(components) do
    cc.Registry.add(import("." .. packageName), packageName)
end

-- cc = cc.GameObject.extend()
local GameObject = cc.GameObject
local ccmt = {}
ccmt.__call = function(self, target)
    if target then
        return GameObject.extend(target)
    end
    printError("cc() - invalid target")
end
setmetatable(cc, ccmt)

ds = import(".ds.init")
