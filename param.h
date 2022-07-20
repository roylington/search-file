/**
 * 
 * Interface declaring functions that parse main parameters
 * into structure options (OPT)
 * 
 * Prints help - how to use this program
 * Prints error messages on stderr if any kind of fault occured
 * or given options / arguments are invalid
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <getopt.h>
#include <unistd.h>
#include <pwd.h>

/**
 * Struct keeping information about arguments passed at execution
 * @param type - one character defining the action to be executed
 * @param argument - bonus information required by type, can be NULL
*/
struct parameter {
    char type;
    char *argument;
};

typedef struct parameter Param;

/**
 * Struct holding all possible options and their information
 * @param name...t - pointer to Param structure with info
 * @param null_bytes @param hidden - bool determining output structure
 * @param path - where to start finding files / dirs
 */
struct options {
    Param *name;
    Param *mask;
    Param *user;
    Param *sort;
    Param *f;
    Param *t;
    bool null_bytes;
    bool hidden;
    char *path;
};

typedef struct options OPT;

/**
 * Reads options and arguments
 * Creates OPT structure where information about each option is stored
 * 
 * @param argc - amount of arguments given to main
 * @param argv[] - array of arguments
 * @return ptr to OPT structure
 */
OPT *readOptions(int argc, char *argv[]);

/**
 * Saves a new option + argument to OPT
 * If fault occurs -> frees memory, writes error and ends
 * 
 * @param opt - current options structure
 * @param arg - current option
 * @return true if successful, false otherwise
 */
bool writeNew(OPT *opt, int arg);

/**
 * Frees all allocated memory for options
 */
void freeAlloc(OPT *opt);

/**
 * Prints -h option / how to use the program
 */
void help();

/**
 * Writes standard error message during memory allocation fault
 */
void memfail();