#include "bstream.h"

#include <libdrawable.hpp>
#include <drawable.hpp>
#include <drawabledictionary.hpp>

#include <iostream>
#include <fstream>

int main(int argc, char* argv[]) {
    UDrawable* drw = ImportYdr("D:\\RedDead\\q_13__hd_0_1_0.ydr");
    delete drw;

    //UDrawableDictionary* dict = ImportYdd("D:\\RedDead\\player_merge_10.ydd");
    //delete dict;
}