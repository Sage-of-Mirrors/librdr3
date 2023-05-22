#include "bstream.h"

#include <libdrawable.hpp>
#include <drawable.hpp>

#include <iostream>
#include <fstream>

int main(int argc, char* argv[]) {
    UDrawable* drw = ImportYdr("D:\\RedDead\\w_repeater_winchester01.ydr");
    delete drw;
}