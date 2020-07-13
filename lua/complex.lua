local M = {}

local function new(r, i)
    return {r = r, i = i}
end

M.new = new

M.i = new(0, 1)

function M.add(c1, c2)
    return new(c1.r + c2.r, c1.i + c2.i)
end

function M.sub(c1, c2)
    return new(c1.r - c2.r, c1.i - c2.i)
end

function M.mul(c1, c2)
    return new(c1.r*c2.r - c1.i*c2.i, c1.r*c2.i + c1.i*c2.r)
end

function M.tostring(c)
    return string.format("(%g, %g)", c.r, c.i)
end

-- return M
package.loaded["complex"] = M -- 和上面的return意义相同
