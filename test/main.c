#include <stdio.h>
#include <libdrawable.h>

int main(int argc, char* argv[]) {
    FILE* t = fopen("D:\\RedDead\\nopq_03_06_slod4.ydr", "rb");
    
    fseek(t, 0, SEEK_END);
    long size = ftell(t);
    fseek(t, 0, SEEK_SET);

    char* buf = (char*)malloc(size);
    size_t bytesRead = fread(buf, 1, size, t);

    UStream strm;
    UInitStream(&strm, buf, size, ENDIAN_LITTLE);

    UDrawable drw;
    DeserializeDrawable(&strm, &drw);

    fclose(t);
}