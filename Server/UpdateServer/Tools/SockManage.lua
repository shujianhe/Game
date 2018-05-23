local SockManage = {}
require("json")
function SockManage:init()
	self.__ConnectBack = function ()end
	self.__MsgBack = function ()end
	self.__ConnectPool = {}
	self.__ErrorBackHandle = function (...)print(...)assert(false)end
	self.__LogBackHandle = function ( ... )print(...)end
	self.Socket = _MyG.Net
	return self
end
function SockManage:getSidData(sid)
	local t = self.__ConnectPool[sid]
	if t == nil then
		t = {sid = sid,IsUser = true}
		local mt = {__newindex = function(t,k,v)
			self.__ErrorBackHandle("禁止擅自修改soket标志表")
		end,__call = function (t,tData)
			return self:send(tData,t)
		end}
		setmetatable(t,mt)
		self.__ConnectPool[sid] = t
	end
	return t
end
function SockManage:getInstance()
	self.__instance = self.__instance or self:init()
	return self.__instance
end
function SockManage:setNewConnectBack(back)
	if type(back) == "function" then
		self.__ConnectBack = back
		return true
	end
	return false
end
function SockManage:setRecvBack(back)
	if type(back) == "function" then
		self.__MsgBack = back
		return true
	end
	return false	
end
function SockManage:setErrorBackHandle(back)
	if type(back) == "function" then
		self.__ErrorBackHandle = back
	end
end
function SockManage:setLogBackHandle(back)
	if type(back) == "function" then
		self.__LogBackHandle = back
	end
end
function SockManage:update()
	--通过更新
	local t = {self.Socket:getmsg()}
	if t then
		assert(type(t[1]) == "number")
		local sockTable = self:getSidData(""..t[1])
		local Data = json.decode(t[5])
		--[[%Y年%m月%d日:%H时%M分%S秒]]
		local OtherData = {["有效数据长度"] = t[2],["发送时间"] = t[3],["接受时间"] = t[4]}
		self.__MsgBack(sockTable,Data,OtherData)
	end
end
function SockManage:connect(ip,port)
	local Id = self.Socket:Connect(ip,port)
	if Id == nil then
		return nil
	else
		return self:getSidData(""..Id)
	end
end
function SockManage:close(tData)
	if type(tData) == "table" and type(tData.sid) == "string" then
		local t = self.__ConnectPool[tData.sid]
		if t == nil or t ~= tData then
			self.__ErrorBackHandle("不存在网络连接 不能发送")
			return false
		else
			self.Socket:Close(tonumber(tData.sid))
		end
	else
		self.__ErrorBackHandle("错误的参数:"..tostring(tData))
		return false
	end
end
function SockManage:send(tData,handData)
	if type(tData) == "table" and type(tData.sid) == "string" then
		local t = self:getSidData(tData.sid)
		if t == nil or t ~= tData then
			self.__ErrorBackHandle("不存在网络连接 不能发送")
			return false
		else
			local strmsg = json.encode(tab)
			return self.Socket:Send(tData.sid,strmsg,string.len(strmsg))
			--[[print("-----------------------")
			print(tData.sid,strmsg)
			print("-----------------------")]]--
		end
	else
		self.__ErrorBackHandle("错误的参数:"..tostring(tData))
		return false
	end
end
function SockManage:StartServer(port)--开始启动服务
	local Id = self.Socket:Server(port)
	if Id then
		local t = self:getSidData(""..Id)
		return true
	else
		return false
	end
end
return SockManage:getInstance()