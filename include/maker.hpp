//
// Created by Mpho Mbotho on 2020-10-06.
//

#ifndef TOOLS_MAKER_HPP
#define TOOLS_MAKER_HPP

#include <cstddef>
#include <map>
#include <string>

struct lua_State;

namespace maker {

    struct embedded_script {
        const char *name;
        size_t      size;
        const char *buffer;
    };

    struct EmbeddedScripts {
        static int load(const char *name);
        static void main(int argc, char **argv);

    private:
        static int makerNow(lua_State *L);
        static int makerLoad(lua_State* L);
        static int makerCwd(lua_State* L);
        static int makerEnv(lua_State* L);
        static int makerGc(lua_State* L);
        static int makerExit(lua_State* L);
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
#endif //TOOLS_MAKER_HPP
