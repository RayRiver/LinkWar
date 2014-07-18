/*
** Lua binding: lua_cocos2dx_userext
** Generated automatically by tolua++-1.0.92 on 07/18/14 11:00:06.
*/

/****************************************************************************
 Copyright (c) 2011 cocos2d-x.org

 http://www.cocos2d-x.org

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ****************************************************************************/

#include "tolua_fix.h"

#include "cocos2d.h"
#include "CCLuaValue.h"
#include "LuaBasicConversions.h"

#include "Battle/LogicFrame.h"
#include "Battle/MapManager.h"
#include "Battle/MapView.h"
#include "Battle/GameObjectManager.h"

USING_NS_CC;



/* Exported function */
TOLUA_API int  tolua_lua_cocos2dx_userext_open (lua_State* tolua_S);


/* function to register type */
static void tolua_reg_types (lua_State* tolua_S)
{
 tolua_usertype(tolua_S,"MapManager");
 tolua_usertype(tolua_S,"cc.Layer");
 tolua_usertype(tolua_S,"LogicFrameManager");
 tolua_usertype(tolua_S,"MapView");
 tolua_usertype(tolua_S,"cc.Node");
 tolua_usertype(tolua_S,"GameObjectManager");
}

/* method: getInstance of class  MapManager */
#ifndef TOLUA_DISABLE_tolua_lua_cocos2dx_userext_MapManager_getInstance00
static int tolua_lua_cocos2dx_userext_MapManager_getInstance00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertable(tolua_S,1,"MapManager",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  {
   MapManager* tolua_ret = (MapManager*)  MapManager::getInstance();
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"MapManager");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'getInstance'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: loadData of class  MapManager */
#ifndef TOLUA_DISABLE_tolua_lua_cocos2dx_userext_MapManager_loadData00
static int tolua_lua_cocos2dx_userext_MapManager_loadData00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"MapManager",0,&tolua_err) ||
     !tolua_isstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  MapManager* self = (MapManager*)  tolua_tousertype(tolua_S,1,0);
  const char* config = ((const char*)  tolua_tostring(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'loadData'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->loadData(config);
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'loadData'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: getInstance of class  GameObjectManager */
#ifndef TOLUA_DISABLE_tolua_lua_cocos2dx_userext_GameObjectManager_getInstance00
static int tolua_lua_cocos2dx_userext_GameObjectManager_getInstance00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertable(tolua_S,1,"GameObjectManager",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  {
   GameObjectManager* tolua_ret = (GameObjectManager*)  GameObjectManager::getInstance();
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"GameObjectManager");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'getInstance'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: getInstance of class  LogicFrameManager */
#ifndef TOLUA_DISABLE_tolua_lua_cocos2dx_userext_LogicFrameManager_getInstance00
static int tolua_lua_cocos2dx_userext_LogicFrameManager_getInstance00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertable(tolua_S,1,"LogicFrameManager",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  {
   LogicFrameManager* tolua_ret = (LogicFrameManager*)  LogicFrameManager::getInstance();
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"LogicFrameManager");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'getInstance'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: init of class  LogicFrameManager */
#ifndef TOLUA_DISABLE_tolua_lua_cocos2dx_userext_LogicFrameManager_init00
static int tolua_lua_cocos2dx_userext_LogicFrameManager_init00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"LogicFrameManager",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  LogicFrameManager* self = (LogicFrameManager*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'init'", NULL);
#endif
  {
   self->init();
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'init'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* Open function */
TOLUA_API int tolua_lua_cocos2dx_userext_open (lua_State* tolua_S)
{
 tolua_open(tolua_S);
 tolua_reg_types(tolua_S);
 tolua_module(tolua_S,NULL,0);
 tolua_beginmodule(tolua_S,NULL);
  tolua_cclass(tolua_S,"MapManager","MapManager","cc.Node",NULL);
  tolua_beginmodule(tolua_S,"MapManager");
   tolua_function(tolua_S,"getInstance",tolua_lua_cocos2dx_userext_MapManager_getInstance00);
   tolua_function(tolua_S,"loadData",tolua_lua_cocos2dx_userext_MapManager_loadData00);
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"MapView","MapView","cc.Layer",NULL);
  tolua_beginmodule(tolua_S,"MapView");
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"GameObjectManager","GameObjectManager","cc.Node",NULL);
  tolua_beginmodule(tolua_S,"GameObjectManager");
   tolua_function(tolua_S,"getInstance",tolua_lua_cocos2dx_userext_GameObjectManager_getInstance00);
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"LogicFrameManager","LogicFrameManager","",NULL);
  tolua_beginmodule(tolua_S,"LogicFrameManager");
   tolua_function(tolua_S,"getInstance",tolua_lua_cocos2dx_userext_LogicFrameManager_getInstance00);
   tolua_function(tolua_S,"init",tolua_lua_cocos2dx_userext_LogicFrameManager_init00);
  tolua_endmodule(tolua_S);
 tolua_endmodule(tolua_S);
 return 1;
}


#if defined(LUA_VERSION_NUM) && LUA_VERSION_NUM >= 501
 TOLUA_API int luaopen_lua_cocos2dx_userext (lua_State* tolua_S) {
 return tolua_lua_cocos2dx_userext_open(tolua_S);
};
#endif

