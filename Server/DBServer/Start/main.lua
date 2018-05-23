function main()
	print("====================")
end
function traceback(msg)
	print("---------------------------------------------")
	print("LUA ERROR: "..tostring(msg).."\n")
	print(debug.traceback(nil,3))
	print("---------------------------------------------")
end
xpcall(main,traceback)