//
// Created by Mpho Mbotho on 2020-10-06.
//

#include "../include/maker.hpp"
#include "maker/decls.h"

#include <lua.hpp>
#include <cstring>
#include <sstream>
#include <cstddef>
#include <type_traits>
#include <unistd.h>

namespace {
    void exportLuaFunc(lua_State *L, lua_CFunction func, const char *name)
    {
        lua_pushstring(L, name);
        lua_pushcfunction(L, func);
        lua_settable(L, -3);
    }
}

#define CALL_SYNTAX(func) \
if (!lua_istable(L, 1)) {   \
    return luaL_error(L, "use Swept:%s instead of Swep.%s", func); \
}

namespace maker {

    static const embedded_script sScripts[] = {
#include "maker/defs.h"
    };

    lua_State *EmbeddedScripts::L{nullptr};

    int EmbeddedScripts::makerLoad(lua_State *L)
    {
        CALL_SYNTAX("load")

        if (!lua_isstring(L, 2)) {
            return luaL_error(L, "invalid parameter type, only 'string' accepted");
        }
        else {
            const char *name = lua_tostring(L, 2);
            return EmbeddedScripts::load(name);
        }
    }

    int EmbeddedScripts::makerNow(lua_State *L)
    {
        CALL_SYNTAX("now")
        lua_pushnumber(L, time(nullptr));
        return 1;
    }

    int EmbeddedScripts::makerEnv(lua_State *L)
    {
        CALL_SYNTAX("env")
        if (!lua_isstring(L, 2)) {
            return luaL_error(L, "Swept:env takes name name of the environment variable");
        }

        if (const char *val = std::getenv(luaL_checkstring(L, 2))) {
            lua_pushstring(L, val);
        }
        else {
            lua_pushstring(L, "");
        }
        return 1;
    }

    int EmbeddedScripts::makerExit(lua_State *L)
    {
        CALL_SYNTAX("exit")
        if (!lua_isnumber(L, 2)) {
            return luaL_error(L, "Swept:exit takes an exit code which is a number");
        }
        auto num = static_cast<int>(luaL_checknumber(L, 2));
        lua_close(L);
        exit(num);
    }

    int EmbeddedScripts::makerGc(lua_State *L)
    {
        CALL_SYNTAX("gc")
        lua_gc(L, LUA_GCCOLLECT, 0);
        return 0;
    }

    int EmbeddedScripts::makerCwd(lua_State *L)
    {
        CALL_SYNTAX("cwd");
        char buf[PATH_MAX] = {0};
        if (const char *path = getcwd(buf, PATH_MAX)) {
            lua_pushstring(L, path);
        }
        else {
            lua_pushboolean(L, false);
        }
        return 1;
    }

    int EmbeddedScripts::panicHandler(lua_State *L)
    {
        auto msg = luaL_checkstring(L, -1);
        if (msg) {
            fprintf(stderr, "error: %s\n", msg);
        }
        else {
            fprintf(stderr, "unknown error in swept\n");
        }
        exit(EXIT_FAILURE);
        return 0;
    }

    int EmbeddedScripts::error(const std::string &msg)
    {
        return luaL_error(state(), msg.c_str());
    }

    size_t EmbeddedScripts::count() {
        static size_t numScripts{sizeof(sScripts)/sizeof(embedded_script)};
        return numScripts;
    }

    lua_State* EmbeddedScripts::state() {
        if (L == nullptr) {
            // create lua runtime
            L = luaL_newstate();
            if (L == nullptr) {
                throw std::runtime_error("failed to create lua runtime");
            }
            luaL_openlibs(L);
        }

        return L;
    }

    int EmbeddedScripts::load(const char *name)
    {
        const embedded_script *found{nullptr};

        for (auto i = 0; i < count()-1; i++) {
            if (strcmp(name, sScripts[i].name) == 0) {
                found = &sScripts[i];
                break;
            }
        }

        if (found == nullptr) {
            std::stringstream ss;
            ss << "internal module '" << name << "' does not exist";
            return EmbeddedScripts::error(ss.str());
        }
        else {
            return loadScript(*found);
        }
    }

    int EmbeddedScripts::loadScript(const embedded_script &es)
    {
        auto status = luaL_loadbuffer(state(), es.buffer, es.size, es.name);
        if (status) {
            return luaL_error(state(), "%s", lua_tostring(state(), -1));
        }
        status = lua_pcall(state(), 0, 1, 0);
        if (status) {
            return luaL_error(state(), "%s", lua_tostring(L, -1));
        }
        return 1;
    }

    std::string EmbeddedScripts::currdir()
    {
        char buf[PATH_MAX] = {0};
        return std::string{getcwd(buf, PATH_MAX)};
    }

    void EmbeddedScripts::exportApis()
    {
        exportLuaFunc(state(), EmbeddedScripts::makerLoad, "load");
        exportLuaFunc(state(), EmbeddedScripts::makerNow,  "now");
        exportLuaFunc(state(), EmbeddedScripts::makerEnv, "env");
        exportLuaFunc(state(), EmbeddedScripts::makerGc, "gc");
        exportLuaFunc(state(), EmbeddedScripts::makerExit, "exit");
        exportLuaFunc(state(), EmbeddedScripts::makerCwd, "cwd");
    }

    static void createargtable(lua_State *L, char **argv, int argc, int script) {
        int i, narg;
        if (script == argc) script = 0;  /* no script name? */
        narg = argc - (script + 1);  /* number of positive indices */
        lua_createtable(L, narg, script + 1);
        for (i = 0; i < argc; i++) {
            lua_pushstring(L, argv[i]);
            lua_rawseti(L, -2, i - script);
        }
        lua_setglobal(L, "arg");
    }

    template <typename V, std::enable_if_t<std::is_arithmetic_v<V>, V>* = nullptr>
    inline void pushValue(lua_State* L, const V v)
    {
        lua_pushnumber(L, v);
    }
    inline void pushValue(lua_State *L, const char *value) { lua_pushstring(L, value); }
    inline void pushValue(lua_State *L, std::nullptr_t) { lua_pushnil(L); }
    inline void pushValue(lua_State *L, bool value) {lua_pushboolean(L, value); }

    template <typename V>
    void setField(lua_State* L, const char *name, V value) {
        lua_pushstring(L, name);
        pushValue(L, std::forward<V>(value));
        lua_settable(L, -3);
    }

    void EmbeddedScripts::main(int argc, char **argv)
    {
        // export some runtime native API's
        auto cwd = currdir();
        lua_atpanic(state(), EmbeddedScripts::panicHandler);
        luaL_newmetatable(state(), "Maker_mt");
        lua_pushstring(state(), "__index");
        lua_newtable(state());
        EmbeddedScripts::exportApis();
        lua_settable(state(), -3);
        lua_newtable(state());
        luaL_getmetatable(state(), "Maker_mt");
        lua_setmetatable(state(), -2);
        // add properties to the table
        setField(state(), "SRC",   SRC_DIR);
        setField(state(), "VER", MAKER_VERSION);
        setField(state(), "PWD", cwd.c_str());
        setField(state(), "OS_VARIANT", OS_VARIANT);
        setField(state(), OS_VARIANT, true);
        lua_setglobal(state(), "Maker");

        // build commandline arguments table
        lua_createtable(state(), argc, 1);
        for (int i = -1; i < argc-1; i++) {
            lua_pushstring(L, argv[i+1]);
            lua_rawseti(state(), -2, i);
        }
        lua_setglobal(state(), "arg");
    }

}
