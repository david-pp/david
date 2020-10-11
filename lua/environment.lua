

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
            local w = debug.getinfo(2, "S").what
            if w ~= "main" and w ~= "c" then
                error("attempt to write to undeclaed variable " .. n, 2)
            end
            rawset(t, n, v)
        end,
        __index = function(_, n)
            error("attempt to read undeclaed variable " .. n, 2)
        end
    })

    local function declare(name, initval)
        rawset(_G, name, initval or nil)
    end

    declare("a", 10)
    print(a)

    a = 20
    print(a)
    --]]
end

print("------- _EVN.1 --------")
do
    -- _ENV 默认为 _G
    -- _ENV 始终指定当前的环境 _G 为全局环境
    a = 13
    local a = 12
    print(a)
    print(_ENV.a)
    print(_G.a)

end

print("------- _EVN.2 --------")
do
    -- 改变当前环境
    local env = _ENV

    a = 15             -- create a global variable
    _ENV = { _G = _G }  -- change current environment
    a = 1              -- create a field in _ENV
    _G.print(a, _ENV.a, _G.a)

    _ENV = env
end

print("------- _EVN.3 --------")
do
    local env = _ENV
    ---

    a = 1
    local newgt = {}
    setmetatable(newgt, {__index = _G })

    local _ENV = newgt
    print(a)

    a = 10
    print(a, _G.a) -- 10, 1

    _G.a = 20
    print(a, _G.a) -- 10, 20

    ---
    _ENV = env
end

print("------- _EVN.4 --------")
do
    local env = _ENV
    -----
    _ENV = { _G = _G }

    local function foo() 
        _G.print(a)
    end

    a = 10
    foo() -- 10

    _ENV = { _G = _G, a = 20 }
    foo() -- 20

    -----
    _ENV = env
end

print("------- _EVN.5 --------")
do
    a = 2
    do 
        local _ENV = { print = print, a = 14 }
        print(a) -- 14
    end
    print(a) --2
end

print("------- _EVN.6 --------")
do
    function factory(_ENV)
        return function() return a end
    end

    f1 = factory{a = 6}
    f2 = factory{a = 7}
    print(f1()) -- 6
    print(f2()) -- 7
end

print("------- _EVN.7 --------")
do
    a = 7
    env = { a = 20 }
    f = load("b = 10; return a")
    debug.setupvalue(f, 1, env)
    print(f())   -- 20
    print(env.b) -- 10
end

print("------- _EVN.8 --------")
do
    require("config")
    print(width, height)

    env = {}
    loadfile("config2.lua", "t", env)
    print(width, height)
    --print(env.width, env.height)
end


print("------- 非全局环境 --------")
do
    ---[[
    local print, sin = print, math.sin
    _ENV = nil

    print(13)
    print(sin(13))
    -- print(math.sin(13))
    --]]
end


-- to continue
