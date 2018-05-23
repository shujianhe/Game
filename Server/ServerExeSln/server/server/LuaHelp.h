#pragma once
#include <functional>
#include "lua.hpp"
namespace userlua{
	class LuaHelp
	{
	private:
		static LuaHelp* LH;
		lua_State *L;
	private:
		LuaHelp();
		~LuaHelp(); 
		bool Clear();
	public:
		static LuaHelp *getInstance(){
			LH = LH ? LH : new LuaHelp;
			return LH;
		}
		static void Destroy(){
			if (LH)
				delete LH;
			LH = NULL;
		}
		bool init(std::function<void(lua_State*)> initBack);//��ʼ������ͨ���ص������Զ����ʼ��
		inline bool init(){
			return init([&](lua_State* L){});
		}
		bool RunLuaFile(std::string stratFilePath);
		inline lua_State* getLua(){
			return L;
		}
	};
}