--[[
变量：
 - 全局变量
 - 局部变量
--]]


a = 5               -- global
local b = 5         -- local

function joke() 
	c = 5           -- global
	local d = 6     -- local
end

joke()
print(a, b, c, d)

-- 赋值语句

a = "hello" .. "world"
a, b = 10, 20      -- a=10, b=20
a, b = 10, 20, 30  -- a=10, b=20
a, b = 10          -- a=10, b=nil

function swap(a, b) 
	return b, a
end

a, b = swap(10, 20) -- a=20, b=10

--[[
  循环语句
--]]

a = 0
while (a < 10) do
	print(a)
	a = a+1
end

-- for i=初值, 终值, 步长
for i=0, 10, 2 do
	print(i)
end

for i, v in ipairs({"apple", "banana", "grape"}) do
	print(i, v)
end

--[[
  条件语句
--]]

if(0) then
	print("0 is true")
end

a = 20
if (a > 10) then
	print("a > 10")
else
	print("a <= 10")
end

