//
// Created by Mpho Mbotho on 2020-10-06.
//

#include "../include/maker.hpp"

int main(int argc, char* argv[])
{
    maker::EmbeddedScripts::main(argc, argv);
    maker::EmbeddedScripts::load("maker_main");
    return EXIT_SUCCESS;
}