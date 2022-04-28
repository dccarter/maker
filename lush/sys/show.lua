--
-- Created by IntelliJ IDEA.
-- User: carter
-- Date: 2020-10-06
-- Time: 4:15 p.m.
-- To change this template use File | Settings | File Templates.
--
local function help(err)
    local printer = io.stderr
    if not err then
        printer = io.stdout
        printer:write("lushg v"..Lush.VER.."\n")
        printer:write([[

Lush is a lua script runner which integrates nicely with shell scripting. i.e
shell commands can be executed directly from a lua script to be run by lush.

]])
    end
    printer:write([[
Usage:
    lush [[-v|--version]|[-h|--help]|[script <args...>]

    -v|--version     Show the version of lush
    -h|--help        Show this current help
    script <args...> Executes the given lua script with the given arguments

]])
end

function show(arg)
    local printer = io.stdout
    if not arg then
        io.stderr:write("error: missing script to run or command line arguments\n")
        help(true)
        return false
    elseif arg == '-h' or arg == '--help' then
        help(false)
        return true
    elseif arg == '-v' or arg == '--version' then
        io.stdout:write("lush version: "..Lush.VER.."\n")
        return true
    else
        io.stderr:write("error: unrecognized command line argument '"..arg.."'")
        help(true)
        return false
    end
end

return function() end

