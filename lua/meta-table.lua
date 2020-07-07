
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


-- 
local a = Set.new{1, 2, 3}
local b = Set.new{2, 3, 4}

Set.print(a + b)
Set.print(a * b)
Set.print((a + b) *b)

--
s1 = Set.new{2, 4}
s2 = Set.new{4, 10, 2}

print(s1 <= s2)
print(s1 < s2)
print(s1 >= s1)
print(s1 == s2*s1)
print(s1 > s1)

