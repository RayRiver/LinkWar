
local Map = class("ds.Map")

function Map:ctor()
    self.container = {}
end

function Map:size()
    return #self.container
end

function Map:empty()
    return #self.container == 0
end

function Map:clear()
    self.container = {}
end

function Map:insert(key, val)
    self.container[key] = val
end

function Map:remove(key)
    self.container[key] = nil
end

function Map:get(key)
    return self.container[key]
end

return Map
