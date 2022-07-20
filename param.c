#include "param.h"

int option_index(int arg);
void initializeOpt(OPT *opt);


OPT *readOptions(int argc, char *argv[])
{
    int arg;
    const char *options = "n:s:m:u:f:t:a0h";
    // all long options available
    static struct option long_opt[] =
    {
        {"name",        required_argument, 0, 'n'},
        {"sort",        required_argument, 0, 's'},
        {"mask",        required_argument, 0, 'm'},
        {"user",        required_argument, 0, 'u'},
        {"mindepth",    required_argument, 0, 'f'},
        {"maxdepth",    required_argument, 0, 't'},
        {"all",         no_argument, 0, 'a'},
        {"nullnewline", no_argument, 0, '0'},
        {"help",        no_argument, 0, 'h'},
        {0, 0, 0, 0}
    };

    OPT *opt = calloc(1, sizeof(OPT));
    initializeOpt(opt);

    if (argc == 1) {    // no arguments given
        return opt;
    }
    
    if (*argv[1] != '-') {    // path is first argument
        opt->path = malloc(strlen(argv[1]) + 1);
        strcpy(opt->path, argv[1]);
    }

    while (1)
    {
        int option_index = 0;    // stores index of read option
        
        // read next option
        arg = getopt_long(argc, argv, options, long_opt, &option_index);
        if (arg == -1) {    // end of main arguments
            break;
        } else if (arg == '?') {    // invalid option
            freeAlloc(opt);
            return NULL;
        }
        // path not yet assigned && optind points to a valid memory
        if (!opt->path && optind != argc && *argv[optind] != '-') {    // assign path
            opt->path = malloc(strlen(argv[optind]) + 1);
            strcpy(opt->path, argv[optind]);
        }
        if (!writeNew(opt, arg)) {    // assign option information
            freeAlloc(opt);
            return NULL;
        }
    }
    return opt;
}

bool writeNew(OPT *opt, int arg)
{
    // basic options (no allocation needed)
    switch (arg) {
        case 'h':
            help();
            return false;
        case '0':
            opt->null_bytes = true;
            return true;
        case 'a':
            opt->hidden = true;
            return true;
    }
    
    // options needed allocation
    Param **options[6] = { &opt->name, &opt->mask, &opt->user, &opt->sort, &opt->f, &opt->t };
    int where = option_index(arg);

    // Param doesn't exist yet
    if (!(*options[where])) {
        *options[where] = calloc(1, sizeof(Param));
    }

    // write new / rewrite values if duplicate
    (*options[where])->type = arg;
    char *tmp = realloc((*options[where])->argument, strlen(optarg) + 1);
    if (!tmp) {    // valid realloc
        memfail();
        return false;
    }

    (*options[where])->argument = tmp;
    strcpy((*options[where])->argument, optarg);

    return true;
}

void freeAlloc(OPT *opt)
{
    Param *options[6] = { opt->name, opt->mask, opt->user, opt->sort, opt->f, opt->t };
    for (int i = 0; i < 6; i++) {
        if (options[i]) {
            if (options[i]->argument) {
                free(options[i]->argument);
            }
            free(options[i]);
        }
    }
    if (opt->path)
        free(opt->path);
    free(opt);
}

/*
int main(int argc, char *argv[])
{
    OPT *options = readOptions(argc, argv);

    if (options) {
        freeAlloc(options);
    }
}*/

// ========== SUPPORT FUNCTIONS ==========

/**
 * Returns number representing current option in array (used in writeNew)
 * @param arg - current option
 */
int option_index(int arg)
{
    switch (arg) {
        case 'n':
            return 0;
        case 'm':
            return 1;
        case 'u':
            return 2;
        case 's':
            return 3;
        case 'f':
            return 4;
        case 't':
            return 5;
        default:
            return -1;
    }
}

void memfail()
{
    fprintf(stderr, "Allocating memory for parameters failed\n");
}

void help()
{
    fprintf(stderr, "Usage:\n"
                    "-n NAME\t Filter by substring NAME\n"
                    "-s SORT\t Sort 's' by size / 'f' by path\n"
                    "-m MASK\t Filter by permission\n"
                    "-u USER\t Filter by user name\n"
                    "-f NUM \t Filter by minimal depth\n"
                    "-t NUM \t Filter by maximum depth\n"
                    "-a     \t Hidden dirs\n"
                    "-0     \t Use null byte as line separator\n"
                    "-h     \t Show help\n");
}

void initializeOpt(OPT *opt)
{
    opt->name = NULL;
    opt->mask = NULL;
    opt->user = NULL;
    opt->sort = NULL;
    opt->f    = NULL;
    opt->t    = NULL;
    opt->path = NULL;
    opt->null_bytes = false;
    opt->hidden     = false;
}
