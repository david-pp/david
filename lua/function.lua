

-- 普通函数定义
function max(num1, num2)
	local result
	if (num1 > num2) then
		result = num1;
	else
		result = num2;
	end

	return result
end

print(max(5, 10))

-- 函数作为变量

max2 = function(num1, num2) 
	if (num1 > num2) then
		return num1
	else
		return num2
	end
end

print(max2(5, 10))


-- 多返回值
function swap(a, b) 
	return b, a
end

print(swap(5, 10))

-- 可变参数
function sum(...)
	local s = 0
	for i, v in ipairs{...} do
		s = s + v
	end
	return s
end

print(sum(1, 2, 3, 4, 5))
