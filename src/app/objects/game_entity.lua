
local OBJECT_NAME = "GameEntity"

local ObjectClass = class(OBJECT_NAME, function(filename) 
    if filename then
        return cc.Sprite:create(filename)
    else
        return cc.Sprite:create()
    end
end)

function ObjectClass:ctor()

end

return ObjectClass
