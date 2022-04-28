//
// Created by Mpho Mbotho on 2020-10-06.
//

#include "../include/lush.hpp"

int main(int argc, char* argv[])
{
    lush::EmbeddedScripts::main(argc, argv);
    lush::EmbeddedScripts::load("lush_main");
    return EXIT_SUCCESS;
}