
print("----- 数据文件：统计数量 ------")
do
    local count = 0
    function Entry( _ )
        count = count + 1
    end

    dofile('data.lua')

    print("number of entries: " .. count)
end

print("----- 数据文件：读取作者 ------")
do
    local authors = {}
    function Entry(b) 
        authors[b[1]] = true
    end

    dofile('data.lua')
    for k,v in pairs(authors) do 
        print(k)
    end
end

print("----- 数据文件：自描述数据文件 ------")
-- do
    local authors = {}
    function Entry(b)
        if b.author then
            authors[b.author] = true
        end
    end
    dofile('data2.lua')
    for k,v in pairs(authors) do 
        print(k)
    end
-- end

print("----- 串行化： ------")

function serialize(o)
    if type(o) == "number" then
        io.write(o)
    elseif type(o) == "string" then
        io.write(string.format("%q", o)) -- q -> quote
    elseif type(o) == "table" then
        io.write("{\n")
        for k, v in pairs(o) do
            --io.write(" ", k, "=")
            io.write(" ["); serialize(k) ; io.write("] = ")
            serialize(v)
            io.write(",\n")
        end
        io.write("}\n")
    else
   --     error("cannot serialize a " .. type(o))
    end
end

serialize{
    author = "David",
    title = "Title",
    books = {
        "book1",
        "book2",
    },
}

