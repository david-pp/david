
Set = {}


--
-- Meta table
--
local mt = {}

function Set.new(l)
    local set = {}
    -- set meta table
    setmetatable(set, mt)
    for _, v in ipairs(l) do
        set[v] = true
    end
    return set
end

function Set.union(a, b)
    local res = Set.new{}
    for k in pairs(a) do res[k] = true end
    for k in pairs(b) do res[k] = true end
    return res
end

function Set.intersection(a, b)
    local res = Set.new{}
    for k in pairs(a) do
        res[k] = b[k]
    end
    return res
end

function Set.tostring(set)
    local l = {}
    for e in pairs(set) do
        l[#l + 1] = e
    end

    return "{" .. table.concat(l, ",") .. "}"
end

function Set.print(s)
    print(Set.tostring(s))
end

-- 算术类meta method
mt.__add = Set.union
mt.__mul = Set.intersection

-- 关系类meta method
mt.__le = function (a, b)
    for k in pairs(a) do
        if not b[k] then
            return false
        end
    end

    return true
end

mt.__lt = function (a, b)
    return a <= b and not (b <= a)
end

mt.__eq = function (a, b)
    return a <= b and b <= a
end

-- tostring
mt.__tostring = Set.tostring

mt.__metatable = "protected ..."


-- 
local a = Set.new{1, 2, 3}
local b = Set.new{2, 3, 4}

Set.print(a + b)
Set.print(a * b)
print((a + b) *b)

--
s1 = Set.new{2, 4}
s2 = Set.new{4, 10, 2}

print(s1 <= s2)
print(s1 < s2)
print(s1 >= s1)
print(s1 == s2*s1)
print(s1 > s1)


--
print(getmetatable(s1))
-- setmetatable(s1, {}) # cannot change a protected metatable


print("-----控制table的元素访问-----")
do 
    -- table's __index && __newindex
    Window = {}
    Window.prototype = { x = 0, y = 0, width = 100, height = 100}
    Window.mt = {}


    -- 当x[property]返回nil时, 尝试调用此方法
    Window.mt.__index = function(table, key)
        print("__index__")
        return Window.prototype[key]
    end


    ---[[
    Window.mt.__newindex = function(table, key, value)
        print("__newindex__")
        Window.prototype[key]  = value
    end
    --]]

    --[[ __index即可以时函数，也可以时table
    Window.mt.__index = Window.prototype
    --]]

    -- 构造函数
    function Window.new(o)
        setmetatable(o, Window.mt)
        return o
    end

    w = Window.new{x = 10, y = 20}
    print(w.x)
    print(w.width)

    w.width = 200  -- __newindex__
    print(w.width)

end

print("-----具有默认值的table-----")
do
    function setDefault(t, d)
        local mt = {  __index = function() return d end }
        setmetatable(t, mt)
    end

    table = { x = 10, y = 10}
    setDefault(table, 0)  -- 每个table对象都有一个metatable
    print(table.x)
    print(table.z)
end


print("-----具有默认值的table：高效方法1-----")
do
    -- 一个尽量不冲突的名字：___
    local mt = {  __index = function(t) return t.___ end }
    function setDefault(t, d)
        t.___ = d   
        setmetatable(t, mt)
    end

    table = { x = 10, y = 10}
    setDefault(table, 0)  -- 全部table对象公用一个metatable
    print(table.x)
    print(table.z)
end

print("-----具有默认值的table：高效方法1-----")
do
    -- 创建一个key对象，就会保持唯一性
    local key = {}
    local mt = {  __index = function(t) return t[key] end }
    function setDefault(t, d)
        t[key] = d
        setmetatable(t, mt)
    end

    table = { x = 10, y = 10}
    setDefault(table, 0)  -- 全部table对象公用一个metatable
    print(table.x)
    print(table.z)
end

print("-----跟踪table的访问-----")

do
    local index = {}
    local mt = {
        __index = function(t, k)
            print("*access ".. tostring(k))
            return t[index][k]
        end,

        __newindex = function(t, k, v)
            print("*update "..tostring(k))
            t[index][k] = v
        end
    }

    function track(t)
        local proxy = {}
        proxy[index] = t
        setmetatable(proxy, mt)
        return proxy
    end

    t = {}
    t = track(t)

    t[2] = "hello"
    print(t[2])

    -- 使用代理后pairs返回的kv不会用到__index方法
    for k,v in pairs(t) do
        print(k, v)
    end

    for k,v in pairs(t[index]) do
        print(k, v)
    end
end

print("-----read only table-----")
do
    function readOnly(t)
        local proxy = {}
        local mt = {
            __index = t,
            __newindex = function(t, k, v)
                error("attempt to update a read only table", 2)
            end
        }

        setmetatable(proxy, mt)
        return proxy
    end

    days = readOnly{"Monday", "Tues", "Wednes"}
    print(days[1])
    days[1] = "Mon"
end

