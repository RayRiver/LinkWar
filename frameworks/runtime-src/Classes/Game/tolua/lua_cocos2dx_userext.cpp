/*
** Lua binding: lua_cocos2dx_userext
** Generated automatically by tolua++-1.0.92 on 07/12/14 16:55:29.
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

#include "Battle/MapManager.h"

USING_NS_CC;



/* Exported function */
TOLUA_API int  tolua_lua_cocos2dx_userext_open (lua_State* tolua_S);


/* function to register type */
static void tolua_reg_types (lua_State* tolua_S)
{
 tolua_usertype(tolua_S,"MapManager");
 tolua_usertype(tolua_S,"cc.Layer");
}

/* method: create of class  MapManager */
#ifndef TOLUA_DISABLE_tolua_lua_cocos2dx_userext_MapManager_create00
static int tolua_lua_cocos2dx_userext_MapManager_create00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertable(tolua_S,1,"MapManager",0,&tolua_err) ||
     !tolua_isstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const char* config = ((const char*)  tolua_tostring(tolua_S,2,0));
  {
   MapManager* tolua_ret = (MapManager*)  MapManager::create(config);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"MapManager");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'create'.",&tolua_err);
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
  tolua_cclass(tolua_S,"MapManager","MapManager","cc.Layer",NULL);
  tolua_beginmodule(tolua_S,"MapManager");
   tolua_function(tolua_S,"create",tolua_lua_cocos2dx_userext_MapManager_create00);
  tolua_endmodule(tolua_S);
 tolua_endmodule(tolua_S);
 return 1;
}


#if defined(LUA_VERSION_NUM) && LUA_VERSION_NUM >= 501
 TOLUA_API int luaopen_lua_cocos2dx_userext (lua_State* tolua_S) {
 return tolua_lua_cocos2dx_userext_open(tolua_S);
};
#endif

