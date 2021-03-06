
local display = {}

local sharedDirector = cc.Director:getInstance()

local glview = sharedDirector:getOpenGLView()
local size = glview:getFrameSize()
display.sizeInPixels = { width = size.width, height = size.height }
local w = display.sizeInPixels.width
local h = display.sizeInPixels.height

if CONFIG_SCREEN_WIDTH == nil or CONFIG_SCREEN_HEIGHT == nil then
    CONFIG_SCREEN_WIDTH = w
    CONFIG_SCREEN_HEIGHT = h
end

if not CONFIG_SCREEN_AUTOSCALE then
    if w > h then
        CONFIG_SCREEN_AUTOSCALE = "FIXED_HEIGHT"
    else
        CONFIG_SCREEN_AUTOSCALE = "FIXED_WIDTH"
    end
else
    CONFIG_SCREEN_AUTOSCALE = string.upper(CONFIG_SCREEN_AUTOSCALE)
end

local scale, scaleX, scaleY

if CONFIG_SCREEN_AUTOSCALE then
    if type(CONFIG_SCREEN_AUTOSCALE_CALLBACK) == "function" then
        scaleX, scaleY = CONFIG_SCREEN_AUTOSCALE_CALLBACK(w, h, device.model)
    end

    if not scaleX or not scaleY then
        scaleX, scaleY = w / CONFIG_SCREEN_WIDTH, h / CONFIG_SCREEN_HEIGHT
    end

    if CONFIG_SCREEN_AUTOSCALE == "FIXED_WIDTH" then
        scale = scaleX
        CONFIG_SCREEN_HEIGHT = h / scale
    elseif CONFIG_SCREEN_AUTOSCALE == "FIXED_HEIGHT" then
        scale = scaleY
        CONFIG_SCREEN_WIDTH = w / scale
    else
        scale = 1.0
        printError(string.format("display - invalid CONFIG_SCREEN_AUTOSCALE \"%s\"", CONFIG_SCREEN_AUTOSCALE))
    end

    glview:setDesignResolutionSize(CONFIG_SCREEN_WIDTH, CONFIG_SCREEN_HEIGHT, cc.ResolutionPolicy.NO_BORDER)
end

local winSize = sharedDirector:getWinSize()
display.contentScaleFactor = scale
display.size               = {width = winSize.width, height = winSize.height}
display.width              = display.size.width
display.height             = display.size.height
display.cx                 = display.width / 2
display.cy                 = display.height / 2
display.c_left             = -display.width / 2
display.c_right            = display.width / 2
display.c_top              = display.height / 2
display.c_bottom           = -display.height / 2
display.left               = 0
display.right              = display.width
display.top                = display.height
display.bottom             = 0
display.widthInPixels      = display.sizeInPixels.width
display.heightInPixels     = display.sizeInPixels.height

printInfo(string.format("# CONFIG_SCREEN_AUTOSCALE      = %s", CONFIG_SCREEN_AUTOSCALE))
printInfo(string.format("# CONFIG_SCREEN_WIDTH          = %0.2f", CONFIG_SCREEN_WIDTH))
printInfo(string.format("# CONFIG_SCREEN_HEIGHT         = %0.2f", CONFIG_SCREEN_HEIGHT))
printInfo(string.format("# display.widthInPixels        = %0.2f", display.widthInPixels))
printInfo(string.format("# display.heightInPixels       = %0.2f", display.heightInPixels))
printInfo(string.format("# display.contentScaleFactor   = %0.2f", display.contentScaleFactor))
printInfo(string.format("# display.width                = %0.2f", display.width))
printInfo(string.format("# display.height               = %0.2f", display.height))
printInfo(string.format("# display.cx                   = %0.2f", display.cx))
printInfo(string.format("# display.cy                   = %0.2f", display.cy))
printInfo(string.format("# display.left                 = %0.2f", display.left))
printInfo(string.format("# display.right                = %0.2f", display.right))
printInfo(string.format("# display.top                  = %0.2f", display.top))
printInfo(string.format("# display.bottom               = %0.2f", display.bottom))
printInfo(string.format("# display.c_left               = %0.2f", display.c_left))
printInfo(string.format("# display.c_right              = %0.2f", display.c_right))
printInfo(string.format("# display.c_top                = %0.2f", display.c_top))
printInfo(string.format("# display.c_bottom             = %0.2f", display.c_bottom))
printInfo("#")

display.COLOR_WHITE = {r=255, g=255, b=255}
display.COLOR_BLACK = {r=0, g=0, b=0}
display.COLOR_RED   = {r=255, g=0, b=0}
display.COLOR_GREEN = {r=0, g=255, b=0}
display.COLOR_BLUE  = {r=0, g=0, b=255}

display.AUTO_SIZE      = 0
display.FIXED_SIZE     = 1
display.LEFT_TO_RIGHT  = 0
display.RIGHT_TO_LEFT  = 1
display.TOP_TO_BOTTOM  = 2
display.BOTTOM_TO_TOP  = 3

display.CENTER        = 1
display.LEFT_TOP      = 2; display.TOP_LEFT      = 2
display.CENTER_TOP    = 3; display.TOP_CENTER    = 3
display.RIGHT_TOP     = 4; display.TOP_RIGHT     = 4
display.CENTER_LEFT   = 5; display.LEFT_CENTER   = 5
display.CENTER_RIGHT  = 6; display.RIGHT_CENTER  = 6
display.BOTTOM_LEFT   = 7; display.LEFT_BOTTOM   = 7
display.BOTTOM_RIGHT  = 8; display.RIGHT_BOTTOM  = 8
display.BOTTOM_CENTER = 9; display.CENTER_BOTTOM = 9

display.ANCHOR_POINTS = {
    cc.p(0.5, 0.5),  -- CENTER
    cc.p(0, 1),      -- TOP_LEFT
    cc.p(0.5, 1),    -- TOP_CENTER
    cc.p(1, 1),      -- TOP_RIGHT
    cc.p(0, 0.5),    -- CENTER_LEFT
    cc.p(1, 0.5),    -- CENTER_RIGHT
    cc.p(0, 0),      -- BOTTOM_LEFT
    cc.p(1, 0),      -- BOTTOM_RIGHT
    cc.p(0.5, 0),    -- BOTTOM_CENTER
}

-- add for short codes
display.ANCHOR_CENTER = display.ANCHOR_POINTS[display.CENTER]
display.ANCHOR_TOP_LEFT = display.ANCHOR_POINTS[display.TOP_LEFT]
display.ANCHOR_TOP_CENTER = display.ANCHOR_POINTS[display.TOP_CENTER]
display.ANCHOR_TOP_RIGHT = display.ANCHOR_POINTS[display.TOP_RIGHT]
display.ANCHOR_CENTER_LEFT = display.ANCHOR_POINTS[display.CENTER_LEFT]
display.ANCHOR_CENTER_RIGHT = display.ANCHOR_POINTS[display.CENTER_RIGHT]
display.ANCHOR_BOTTOM_LEFT = display.ANCHOR_POINTS[display.BOTTOM_LEFT]
display.ANCHOR_BOTTOM_RIGHT = display.ANCHOR_POINTS[display.BOTTOM_RIGHT]
display.ANCHOR_BOTTOM_CENTER = display.ANCHOR_POINTS[display.BOTTOM_CENTER]

function display.newScene(name)
    local scene = cc.Scene:create()
    scene:setNodeEventEnabled(true)
    scene.name = name or "<unknown-scene>"
    return scene
end

function display.replaceScene(newScene)
    if sharedDirector:getRunningScene() then
        sharedDirector:replaceScene(newScene)
    else
        sharedDirector:runWithScene(newScene)
    end
end

return display
