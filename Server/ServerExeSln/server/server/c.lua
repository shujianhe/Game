print("zhixing c Lua")
local aaa = {}
print(type(aaa))
print("------------------------")
print(_WG)
for k,v in pairs(_WG) do
	if type(v) == "table" then
		for kk,vv in pairs(v) do
			print("k = "..k.." k = "..kk.." v = "..tostring(vv))
		end
	else
		print("k = "..k.." v = "..tostring(v))
	end
end