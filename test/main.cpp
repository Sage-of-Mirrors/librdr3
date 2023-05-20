#include "bstream.h"

#include <iostream>
#include <fstream>
#include <libdrawable.hpp>

int main(int argc, char* argv[]) {
    bStream::CFileStream stream("D:\\RedDead\\f_15__hd_0_-1_3.ydr");

    UDrawable drw;
    drw.Deserialize(&stream);
}