
print("----------- object ------------")
do
    Account = { balance = 0 }

    function Account.withdraw(v)
        Account.balance = Account.balance - v 
    end

    Account.withdraw(100)
    print(Account.balance)
end

do
    Account = { balance = 0 }

    --[[
    function Account.withdraw(self, v)
    self.balance = self.balance - v
    end
    --]]

    function Account:withdraw(v)
        self.balance = self.balance - v
    end

    a1 = Account
    -- Account = nil
    a1.withdraw(a1, 100)
    print(a1.balance)

    a2 = {balance = 0, withdraw = Account.withdraw }
    a2.withdraw(a2, 200)
    print(a2.balance)

end

do
    Account = {
        balance = 0,
        withdraw = function(self, v)
            self.balance = self.balance -v
        end
    }

    function Account:deposit(v)
        self.balance = self.balance + v
    end

    Account.deposit(Account, 100)
    Account:deposit(200)

    print(Account.balance)
end

print("----------- class ------------")
do
    Account = {
        balance = 0,
        withdraw = function(self, v)
            self.balance = self.balance -v
        end
    }

    function Account:deposit(v)
        self.balance = self.balance + v
    end

    --[[
    local mt = { __index = Account }

    function Account.new(o)
    o = o or {}
    setmetatable(o, mt)
    return o
    end
    --]]

    function Account:new(o)
        o = o or {}
        self.__index = self
        setmetatable(o, self)
        return o
    end

    a = Account:new()
    a:deposit(100)
    a.deposit(a, 100)

    print(a.balance)
    print(Account.balance)
end


print("----------- inheritance ------------")
do
    Account = { balance = 0}

    function Account:new(o)
        o = o or {}
        self.__index = self
        setmetatable(o, self)
        return o
    end

    function Account:deposit(v)
        self.balance = self.balance + v
    end

    function Account:withdraw(v)
        if v > self.balance then
            print("insufficent funds")
            return
        end
        self.balance = self.balance - v
    end

    -- Inheritance
    SpecialAccount = Account:new()

    -- Overload
    function SpecialAccount:withdraw(v)
        if v - self.balance >= self:getLimit() then
            print("insufficent sepcial funds")
            return
        end
        self.balance = self.balance - v
    end

    function SpecialAccount:getLimit()
        return self.limit or 0
    end

    s = SpecialAccount:new{limit = 1000}
    s:withdraw(1001)


    -- Multiple inheritance
    local function search(k, plist)
        for i = 1, #plist do
            local v = plist[i][k]
            if v then 
                return v 
            end
        end
    end

    function createClass(...)
        local c = {}
        local parents = {...}

        setmetatable(c, {__index = function(t, k) 
            return search(k, parents)
        end})

        c.__index = c

        function c:new(o)
            o = o or {}
            setmetatable(o, c)
            return o
        end
        return c
    end

    Named = {}
    function Named:getname()
        return self.name
    end

    function Named.setname(n)
        self.name = n
    end

    -- 多重继承
    NamedAccount = createClass(Account, Named)

    account = NamedAccount:new{name = "Paul"}
    print(account:getname())

end
