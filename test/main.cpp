#include "bstream.h"

#include <libdrawable.hpp>
#include <drawable.hpp>

#include <iostream>
#include <fstream>

int main(int argc, char* argv[]) {
    UDrawable* drw = ImportYdr("D:\\RedDead\\w_sp_bowarrow.ydr");
    delete drw;
}