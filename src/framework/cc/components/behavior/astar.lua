
local COMPONENT_NAME = "AStar"

local Component = import("..component")
local ComponentClass = class(COMPONENT_NAME, Component)

local SIDE_COST = 1.0
local CORNER_COST = 1.414

-- G: 从起始状态到当前状态的实际代价（准确）
-- H: 从当前状态到目标状态的估计代价（曼哈顿法）
-- F: 从起始状态到目标状态的预计代价（G+H）
local ASNode = {
    new = function()
        return {
            x = -1, 
            y = -1,
            F = 0.0,
            G = 0.0,
            H = 0.0,
            parent = nil,
        }
    end,
    
    -- ASNode比较算法
    sort = function(a, b)
        return a.F > b.F
    end,

}

function ComponentClass:ctor()
    ComponentClass.super.ctor(self, COMPONENT_NAME)
    
    self.openNodesHeap = ds.BinaryHeap.new(ASNode.sort, function(a) return a.F end)
    self.openNodes = ds.Map.new()
    self.closeNodes = ds.Set.new()
end

function ComponentClass:setMapInfo(map, w, h)
    self.w = w
    self.h = h
    self.grid = map
end

function ComponentClass:_xy2key(x, y)
    return y * self.w + x
end

function ComponentClass:_key2xy(key)
    local x, y
    x = key % self.w
    y = (key - x) / self.w
    return x, y
end

function ComponentClass:_calcH(x1, y1, x2, y2)
    -- 曼哈顿法计算H值
    return (math.abs(x1-x2) + math.abs(y1-y2)) * SIDE_COST
end

function ComponentClass:_checkChild(x, y, parent)
    -- 节点不可达或有障碍
    if x<0 or x>=self.w or y<0 or y>=self.h or self.grid[y*self.w+x] then
        return true
    end
    
    local t = false
    local t1 = 0
    local t2 = 0
    
    -- 计算当前节点的G值
    local G = 0.0
    if parent.x~=x and parent.y~=y then
        G = parent.G + CORNER_COST
    else
        G = parent.G + SIDE_COST
    end
    
    local key = self:_xy2key(x, y)
    
    -- 检查子节点是否存在于封闭列表中，如果在则什么都不做
    if self.closeNodes:exist(key) then
        return true
    end
    
    -- 检查子节点是否存在于开放列表中
    local foundNode = self.openNodes:get(key)  
    if foundNode then
        -- 判断parent到达该节点的G值是否小于原来的G值
        -- 如果小于，则设置该节点的新父亲节点
        -- （如果没有这一段，也能寻找到路径，但是不会做相对最短的调整）
        if G < foundNode.G then
            foundNode.G = G
            foundNode.F = foundNode.F - (foundNode.G - G)
            foundNode.parent = parent
            t1 = os.clock()
            self.openNodesHeap:heapSort()
            t2 = os.clock()
            t = true
        end
    else
        -- 不存在于开放列表中，则加入
        local node = ASNode.new()
        node.x = x
        node.y = y
        node.G = G
        node.H = self:_calcH(node.x, node.y, self.destination.x, self.destination.y)
        node.F = node.G + node.H
        node.parent = parent
        self.openNodes:insert(key, node)
        t1 = os.clock()
        self.openNodesHeap:pushHeap(node)
        t2 = os.clock()
        t = false
    end

    --printInfo("2 " .. t2-t1 .. "," .. tostring(t))
    
    return true
end

function ComponentClass:findPath(origin, destination)
    self.openNodesHeap:clear()
    self.openNodes:clear()
    self.closeNodes:clear()

    local node = ASNode.new()
    node.x, node.y = self:_key2xy(origin)
    self.destination = {}
    self.destination.x, self.destination.y = self:_key2xy(destination)

    self.openNodesHeap:pushHeap(node)
    self.openNodes:insert(origin, node)

    while self.openNodesHeap:size() > 0 do
        -- 在开放列表中寻找F值最小的节点
        local node = self.openNodesHeap:popHeap()
        local key = self:_xy2key(node.x, node.y)

        -- 将该节点移出开放列表，加入封闭列表
        self.openNodes:remove(key)
        self.closeNodes:insert(key)
        
        -- 找到路径回溯
        if node.x == self.destination.x and node.y == self.destination.y then
            local path = {}
            path[#path+1] = key
            while node.parent do
                path[#path+1] = self:_xy2key(node.parent.x, node.parent.y)
                node = node.parent
            end
            return path
        end
        
        -- 测试临近8点的属性
        self:_checkChild(node.x+1, node.y,   node)     -- 6
        self:_checkChild(node.x+1, node.y-1, node)     -- 3
        self:_checkChild(node.x+1, node.y+1, node)     -- 9
        self:_checkChild(node.x,   node.y-1, node)     -- 2
        self:_checkChild(node.x,   node.y+1, node)     -- 8
        self:_checkChild(node.x-1, node.y,   node)     -- 1
        self:_checkChild(node.x-1, node.y-1, node)     -- 4
        self:_checkChild(node.x-1, node.y+1, node)     -- 7
    end
    
    printInfo("cannot find " .. tostring(count))
    
    return nil
end

function ComponentClass:exportMethods()
    self:exportMethods_({
        "setMapInfo",
        "findPath",
    })
    return self.target_
end

function ComponentClass:onBind_()

end

function ComponentClass:onUnbind_()

end

return ComponentClass
