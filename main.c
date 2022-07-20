
#include "find_and_path.h"

int main(int argc, char *argv[])
{
    OPT *opt = readOptions(argc, argv);
    if (!opt) {
        return 1;
    }
    if (!opt->path) {    // path not assigned -> current directory
        opt->path = malloc(sizeof(char) * 2);
        strcpy(opt->path, ".");
    }
    int result = process(opt);    // process options and find files 
    freeAlloc(opt);
    return result;
}