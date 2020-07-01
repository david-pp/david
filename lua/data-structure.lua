--[[
-- table用作数组
--]]

-- 下标从1开始的，习惯用法
a = {}
for i = 1, 1000 do 
    a[i] = i
end

print(#a)
print(a[1000])

-- 下标从0或者任意数字开始
a = {}
for i = -5, 5 do
    a[i] = i
end

print(a[-5])
print(a[0])
print(#a) -- 长度操作都是从下标1算起的


-- 构造式下标从1开始
a = {1, 3, 5, 7, 9}
print(a[1])
print(a[2])
print(#a)

--[[
  table 用作多维数组/矩阵
  1. 用一纬表示多维，下标做下转换
  2. table嵌套
--]]

do
    local M = 10
    local N = 10
    local mt = {}
    for i = 1, M do
        mt[i] = {}
        for j = 1, N do
            mt[i][j] = i*j
        end
    end

    print(#mt)
    print(#mt[1])
    print(mt[2][3])
end


--[[
  table用作链表
--]]
do
    local list = nil

    -- 表头插入 10, 20, 30
    list = { next = list, value = 10 }
    list = { next = list, value = 20 }
    list = { next = list, value = 30 }

    -- 遍历
    l = list
    while l do
        print(l.value)
        l = l.next
    end

end

--[[
  tale用作队列和双向队列
--]]

do 
    List = {}

    function List.new() 
        return { first = 0, last = -1 }
    end

    function List.pushfirst(list, value) 
        local first = list.first - 1
        list.first = first
        list[first] = value
    end

    function List.pushlast(list, value)
        local last = list.last + 1
        list.last = last
        list[last] = value
    end

    function List.popfirst(list)
        local first = list.first
        if first > list.last then 
            error("list is empty")
        end

        local value = list[first]
        list[first] = nil -- 垃圾回收
        list.first = first + 1
        return value
    end

    function List.poplast(list)
        local last = list.last
        if last < list.first then
            error("list is empty")
        end

        local value = list[last]
        list[last] = nil
        list.last = last - 1
        return value
    end

    function List:dump(list)
        print("list.first: "..list.first.." list.last: "..list.last)
        for i = list.first, list.last do
            print(list[i])
        end
    end

    local l = List.new()
    List.pushfirst(l, 1)
    List.pushfirst(l, 2)
    List.pushlast(l, 3)
    List.pushlast(l, 4)

    List.popfirst(l)
    List.poplast(l)
    List:dump(l)
end
