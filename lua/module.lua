
module = {}

module.constant = "a constant varaible"

function module.func1() 
	print("public function")
end

local function func2()
	print("private function")
end

return module
