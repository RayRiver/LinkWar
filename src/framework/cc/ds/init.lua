
local ds = {}

local ds_table = {
    binary_heap = "BinaryHeap",
    set = "Set",
    map = "Map",
}
for filename, classname in pairs(ds_table) do
    ds[classname] = import("." .. filename)
end

return ds
