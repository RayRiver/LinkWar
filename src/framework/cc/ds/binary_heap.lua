
local BinaryHeap = class("ds.BinaryHeap")

function BinaryHeap:ctor(compare_function, print_function)
    self.container = {} 
    
    if type(compare_function) ~= "function" then
        self.compare = function(a, b) return a > b end -- 默认大顶堆
    else
        self.compare = compare_function
    end
    
    if type(print_function) ~= "function" then
        self.print_function = function(a) return a end
    else
        self.print_function = print_function
    end
end

function BinaryHeap:print()
    local str = ""
    for _, val in pairs(self.container) do
        str = str .. " " .. tostring(self.print_function(val))
    end
    print(str)
end

function BinaryHeap:data()
    return self.container
end

function BinaryHeap:size()
    return #self.container
end

function BinaryHeap:empty()
    return #self.container == 0
end

function BinaryHeap:clear()
    self.container = {}
end

-- 此函数会破坏HEAP结构，可以使用makeHeap来恢复HEAP结构
function BinaryHeap:_pushBack(val)
    self.container[#self.container+1] = val
end

-- 此函数会破坏HEAP结构，可以使用makeHeap来恢复HEAP结构
function BinaryHeap:_popBack()
    local val = self.container[#self.container]
    self.container[#self.container] = nil
    return val
end

function BinaryHeap:pushHeap(val)
    self:_pushBack(val)
    self:makeHeap()
end

function BinaryHeap:popHeap()
    if #self.container == 0 then
        return nil
    elseif #self.container == 1 then
        return self:_popBack()
    end
    
    --堆顶与最后一个元素交换
    local val = self.container[1]
    self.container[1] = self.container[#self.container-1+1]
    self.container[#self.container-1+1] = val
    
    -- 除最后一个元素外的其余元素继续堆化
    self:_heapAdjust(0, #self.container-2)
    
    return self:_popBack()
end

function BinaryHeap:makeHeap()
    if #self.container <= 1 then return end
    
    -- 从后往前将所有非叶节点调整成堆，循环结束后，整个容器已经是一个堆
    for i = (#self.container-#self.container%2)/2-1, 0, -1 do
        self:_heapAdjust(i, #self.container-1)
    end
end

function BinaryHeap:_heapAdjust(parentIndex, endIndex)
    -- 保存父节点的索引和值
    local pindex = parentIndex
    local pvalue = self.container[pindex+1]

    -- 2*pindex+1表示startIndex的子节点
    local i = 2 * pindex + 1
    while i <= endIndex do
        -- i, i+1分别表示pindex的左右子节点，选择其中较大（小）的一个
        -- i<endIndex条件：当i==endIndex的时候表示最后只有左子节点，不需要也不可以比较
        if i<endIndex and self.compare(self.container[i+1+1], self.container[i+1]) then 
            i = i + 1 -- 目前i表示左右子节点较优先的一个
        end
        
        -- 父节点的值优先于较优先的子节点的值，所以该子堆不需要调整
        if self.compare(self.container[pindex+1], self.container[i+1]) then break end
        
        -- 较优先的子节点值移到父节点位置
        self.container[pindex+1] = self.container[i+1]
        
        -- 继续比较较优先子节点的子节点
        pindex = i
        
        i = i*2 + 1
    end
    
    self.container[pindex+1] = pvalue
end

function BinaryHeap:heapSort()
    if #self.container <= 1 then return end
    self:makeHeap()
    
    for i = #self.container-1, 1, -1 do
        -- 堆顶与最后一个元素交换
        local val = self.container[1]
        self.container[1] = self.container[i+1]
        self.container[i+1] = val
        
        -- 除最后一个元素外的其余元素继续堆化
        self:_heapAdjust(0, i-1)
    end
end

return BinaryHeap
