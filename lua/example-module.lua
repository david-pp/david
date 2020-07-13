local m = require("module")
local complex = require("complex") -- module
local bp = require("base.print")   -- submodule

print(m.constant)
m.func1()


c1 = complex.new(1, 2)
c2 = complex.new(3, 4)
print(complex.tostring(complex.i))
print(complex.tostring(complex.add(c1, c2)))

bp.print()
