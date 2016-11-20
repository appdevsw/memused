#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//g++ -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"memused.d" -MT"memused.o" -o "memused.o" "../memused.cpp"
//g++ -nodefaultlibs -o "FreeDisp.run"  ./memused.o   -lc

int main(int argc, char**argv)
{
    const char * fname = "/proc/meminfo";

    FILE *fp = fopen(fname, "r");
    if (fp == NULL)
    {
        printf("Failed to open %s\n", fname);
        exit(-1);
    }

    char buf[2048] { 0 };
    int count = 0;
    while (!feof(fp) && count != sizeof(buf))
        count += fread(buf + count, 1, sizeof(buf) - count, fp);

    fclose(fp);

    if (count == sizeof(buf))
    {
        printf("buffer too small %i\n", count);
        exit(1);
    }

    struct
    {
        long MemTotal;
        long MemFree;
        long Cached;
        long Buffers;
        long Slab;
        long SwapTotal;
        long SwapFree;
    } mem { 0 };

    const char * delim = " :\t\r\n";
    char * token = strtok(buf, delim);
    while (token != NULL)
    {
        //printf("\ntoken <%s>", token);
#define getvar(varlabel)  if (!strcmp(token, #varlabel)) mem. varlabel = atoi(strtok(NULL, delim))
        getvar(MemTotal);
        getvar(MemFree);
        getvar(Buffers);
        getvar(Cached);
        getvar(Slab);
        getvar(SwapTotal);
        getvar(SwapFree);

        token = strtok(NULL, delim);
    }

    printf("%li", (mem.MemTotal - mem.MemFree - mem.Buffers - mem.Cached - mem.Slab) / 1024);
    auto swap = (mem.SwapTotal - mem.SwapFree) / 1024;
    if (swap)
        printf(" %li", swap);

    return 0;
}

