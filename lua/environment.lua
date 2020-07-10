

--
-- _G -> Environment(全局变量保存在该table中)
--
print("------- _G --------")
do 
    myint = 10
    mystring = "david"

    function showall() 
        for n in pairs(_G) do
            print(n)
        end
    end

    showall()
end


print("------- 反射 --------")
do
    function getfield(f)
        local v = _G
        for w in string.gmatch(f, "[%w_]+") do
            v = v[w]
        end
        return v
    end

    function setfield(f, v)
        local t = _G
        for w, d in string.gmatch(f, "([%w_]+)(%.?)") do
            if d == "." then
                t[w] = t[w] or {}
                t = t[w]
            else
                t[w] = v
            end
        end
    end

    setfield("t.x.y", 10)

    print(t.x.y)
    print(getfield("t.x.y"))
end

print("------- 强制全局变量声明 --------")
do
    --[[
    setmetatable(_G, {
        __newindex = function(_, n)
            error("attempt to write to undeclaed variable " .. n, 2)
        end,
        __index = function(_, n)
            error("attempt to read undeclaed variable " .. n, 2)
        end
    })

    local function declare(name, initval)
         rawset(_G, name, initval or nil)
    end
 
    --declare("a")
    --a = 10
    --print(a)
    --
    --]]
end

print("------- 非全局环境 --------")
do
    local print, sin = print, math.sin
    _ENV = nil

    print(13)
    print(sin(13))
    -- print(math.sin(13))
end

-- to continue
