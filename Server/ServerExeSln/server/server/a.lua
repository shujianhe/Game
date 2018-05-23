function dump(param,filepath)
	local _fun = nil
	local str = ""
	local _type = type(param)
	if _type == "string" then
		str = param
	elseif _type == "number" then
		str = ""..param
	elseif _type == "userdata" then
		str = "userdata:"..tostring(param)
	elseif _type == "function" then
		str = "function:"..tostring(param)
	elseif _type == type(true) then
		str = tostring(param)
	elseif _type == type(nil) then
		str = "{\n param = nil \n}"
	elseif _type == "table" then
		local Prve = ""
		_fun = function (tab)
			if type(tab) == "table" then
				for k,v in pairs(tab) do
					local ktype = type(k)
					local vtype = type(v)
					str = str..Prve
					if ktype == "string" then
						str = str.."[\""..k.."\"] = "
					elseif ktype == "number" then
						str = str.."["..k.."] = "
					elseif ktype == "function" then
						str = str.."[\"func:"..tostring(k).."\"] = "
					elseif ktype == "userdata" then
						str = str.."[\"user:"..tostring(k).."\"] = "
					elseif ktype == "table" then
						str = str.."[\"table:"..tostring(k).."\"] = "
					else--其他未知类型，不支持
						assert(false,"Unknown key type, temporarily unsupported "..ktype)
					end
					if vtype == "string" then
						str = str.."[["..v.."]],\n"
					elseif vtype == "number" then
						str = str..v..",\n"
					elseif vtype == "function" then
						str = str.."func:"..tostring(v)..",\n"
					elseif vtype == "userdata" then
						str = str.."user:"..tostring(v)..",\n"
					elseif vtype == "table" then
						local OldPrve = Prve
						str = str..Prve.."{\n"
						Prve = Prve.."  "
						_fun(v)
						Prve = OldPrve
						str = str..Prve.."},\n"
					elseif vtype == type(true) then
						str = str..tostring(v).."\n"
					else
						assert(false,"Unknown value type, temporarily unsupported "..vtype)
					end
				end
			end
		end
		str = str.."{\n"
		Prve = "  "..Prve
		_fun(param)
		if string.byte(str,string.len(str)-1) == string.byte(",",1) then
			str = string.sub(str,1,-3).."\n"
		end
		str = str.."}\n"
	else
		assert(false,"Unknown param type, temporarily unsupported "..vtype)
	end
	if type(filepath) == "string" then
		local f = io.open(filepath,"a+")
		if f then
			str = str.."\n"
			f:write(str)
			f:close()
			return
		end
	end
	print(str)
	return
end
print("------------------------------------")
dump(_MyG)
dump(_WG)
local hand = _MyG.Net:Server(10101)
print("ServerId = ",hand)
local GlobalFuncs = {}
function AddEvent(t,loop,back)
	local key = _MyG.Time:addScheduler(t,loop)
	print(key)
	GlobalFuncs[key] = back
end
_MyG.Time:SetFps(10)
AddEvent(0.25,true,function (...)
	print("-------------------------------")
	print(os.time(),"    ",os.clock())
end)
local count = 0
while count < 200 do
	local t = {_MyG.Net:getmsg()}
	if #t > 0 then
		print(t[5])
		count = count +1
	end
	local allbackkey = _MyG.Time:getAllCallBack()
	if allbackkey then
		for k,v in pairs(allbackkey) do
			GlobalFuncs[v]()
		end
	end
	_MyG.Time:Update()
end