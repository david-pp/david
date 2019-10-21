-- 单行注释

--[[
  多行注释
  多行注释
--]]

print("hello lua")

--[[ 
  数据类型:
  - nil
  - boolean : false/true
  - number
  - string
  - function
  - userdata
  - thread
  - table
--]]

-- nil
print(variable)
print(nil)

-- bolean
print(type(true))
print(type(false))

-- number
print(type(2))
print(type(3.14))
print(type(2e+1))
print(type(0.2e-1))

-- string
local name = "david"
print(#name) -- length of name

local html = [[
<html>
<head></head>
<body>
	<p>body</p>
</body>
</html>
]]

print(html)

print(name .. " wang")

-- table
local tbl1 = {"apple", "pear", "orange", "grape"}
for k, v in pairs(tbl1) do
	print(k .. ":" .. v)
end

for k, v in ipairs(tbl1) do
	print(k .. ":" .. v)
end

-- function
function factorial(n) 
	if n == 0 then
		return 1
	else
		return n * factorial(n-1)
	end
end

fun = factorial
print(factorial(5))
print(fun(5))

