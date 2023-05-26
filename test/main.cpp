#include "bstream.h"

#include <libdrawable.hpp>
#include <drawable.hpp>
#include <drawabledictionary.hpp>

#include <iostream>
#include <fstream>

int main(int argc, char* argv[]) {
    //UDrawable* drw = ImportYdr("D:\\RedDead\\p_coin01x.ydr");
    //delete drw;

    UDrawableDictionary* dict = ImportYdd("D:\\RedDead\\player_three_merge.ydd");
    delete dict;
}