
local Set = class("ds.Set")

function Set:ctor()
    self.container = {}
end

function Set:size()
    return #self.container
end

function Set:empty()
    return #self.container == 0
end

function Set:clear()
    self.container = {}
end

function Set:insert(val)
    self.container[val] = true
end

function Set:remove(val)
    self.container[val] = nil
end

function Set:exist(val)
    return self.container[val]
end

return Set
