//
// Created by Mpho Mbotho on 2020-10-06.
//

#ifndef TOOLS_LUSH_HPP
#define TOOLS_LUSH_HPP

#include <cstddef>
#include <map>
#include <string>

struct lua_State;

namespace lush {

    struct embedded_script {
        const char *name;
        size_t      size;
        const char *buffer;
    };

    struct EmbeddedScripts {
        static int load(const char *name);
        static void main(int argc, char **argv);

    private:
        static int lushNow(lua_State *L);
        static int lushLoad(lua_State* L);
        static int lushCwd(lua_State* L);
        static int lushEnv(lua_State* L);
        static int lushGc(lua_State* L);
        static int lushExit(lua_State* L);
        static int panicHandler(lua_State* L);

    private:
        static int error(const std::string& msg);
        static std::string currdir();
        static size_t count();
        static void exportApis();
        static int loadScript(const embedded_script& es);
        static lua_State *state();
        static lua_State *L;
    };
}
#endif //TOOLS_LUSH_HPP
