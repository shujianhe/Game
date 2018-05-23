local tools = {}
function tools.addclassSuper(cls,tab)
    if cls.class.__supers == nil then
        cls.class.__supers = {tab}
    else
        cls.class.__supers[#cls.class.__supers + 1] = tab
    end
    setmetatable(cls,{__index = function (t,k)
        local v = rawget(t,k)
        if v ~= nil then return v end
        v = rawget(cls,"class")
        v = rawget(v,k)
        if v ~= nil then return v end
        v = rawget(cls,"class")
        local __supers = rawget(v,"__supers") or {}
        for i = 1,#__supers do
            local super = __supers[i]
            if super[k] then return super[k] end
        end
    end})
end
function tools.tableAddSetmetatable(tab,mt)
    for k,v in pairs(tab) do
        if type(v) == "table" then
            if v == tab then
                _WG.error("禁止单例模式的表添加元表")
            end
            tools.tableAddSetmetatable(v,mt)
        end
    end
    setmetatable(tab,mt)
end
function tools._clonetable(tab)
    local t = {}
    for k,v in pairs(tab) do
        if type(v) == "table" then
            if v == tab then
                _WG.error("不能拷贝表和子项互相引用的表")
            end
            t[k] = tools._clonetable(v)
        else
            t[k] = tostring(v)
        end
    end
    return t
end
function tools.dump(tab)
    local s = ""
    function _dump(t,_s)
        s = s.."{\n"
        for k,v in pairs(t) do
            s = s.._s.."["..tostring(k).."] = "
            if type(v) == "table" then
                _dump(v,_s.."   ")
                --s = s.._s.."}"
            elseif type(v) == "string" then
                s = s.."\""..tostring(v).."\""
            else
                s = s..tostring(v)
            end
            s = s..",\n"
        end
        s = s..string.sub(_s,1,-2).."}"
    end
    if type(tab) ~= "table" then
        _dump({tab},"   ")
    else
        _dump(tab," ")
    end
    print(s)
end
return tools