
if type(DEBUG_FPS) ~= "boolean" then DEBUG_FPS = false end

local FRAMEWORK_PACKAGE_NAME = "framework"

cc = cc or {}
cc.PACKAGE_NAME = FRAMEWORK_PACKAGE_NAME
cc.VERSION = "3.1.1"

require(cc.PACKAGE_NAME .. ".functions")
require(cc.PACKAGE_NAME .. ".cocos2dx")

display = require(cc.PACKAGE_NAME .. ".display")

require(cc.PACKAGE_NAME .. ".cc.init")

if DEBUG_FPS then
    cc.Director:getInstance():setDisplayStats(true)
else
    cc.Director:getInstance():setDisplayStats(false)
end
