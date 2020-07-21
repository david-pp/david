
extern "C" {
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
}

void stackDump(lua_State* L) {
    int top = lua_gettop(L);

    for (int i = 1; i <= top; i++) {
        int t = lua_type(L, i);
        switch(t) {
            case LUA_TSTRING:
                printf("%s", lua_tostring(L, i));
                break;
            case LUA_TBOOLEAN:
                printf(lua_toboolean(L,i) ? "true" : "false");
                break;
            case LUA_TNUMBER:
                if (lua_isinteger(L, i))
                    printf("%lld", lua_tointeger(L,i));
                else
                    printf("%g", lua_tonumber(L,i));
                break;
            default: 
                printf("%s", lua_typename(L, t));
                break;
        }

        printf(" ");
    }

    printf("\n"); 
}

void error(lua_State* L, const char* fmt, ...) {
    va_list argp;
    va_start(argp, fmt);
    vfprintf(stderr, fmt, argp);
    va_end(argp);
    lua_close(L);
    exit(EXIT_FAILURE);
}

// ----------

void demo_lua() {
    char buff[256];
    int error;
    lua_State* L = luaL_newstate();
    luaL_openlibs(L);

    while(fgets(buff, sizeof(buff), stdin) != NULL) {
        error = luaL_loadstring(L, buff) || lua_pcall(L, 0,0,0);
        if (error) {
            fprintf(stderr, "%s\n", lua_tostring(L, -1));
            lua_pop(L, 1);
        }
    }

    lua_close(L);
}

void demo_dump() {
    lua_State* L = luaL_newstate();

    lua_pushboolean(L, 1);
    lua_pushnumber(L, 10);
    lua_pushnil(L);
    lua_pushstring(L, "hello");

    stackDump(L);

    lua_close(L);
}

int main(void) {
    // demo_lua();
    demo_dump();
    return 0;
}
