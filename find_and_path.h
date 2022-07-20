/** 
 * 
 * Interface describing functions that recursively search directories
 * and functions saving the paths of files / directories found
 * 
 */

#include "param.h"

#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <ctype.h>

/**
 * Keeps information about recursion depth
 * Keeps information about depth limits in @param f and @param t
 */
struct depth {
    size_t f;
    size_t t;
    size_t depth;
};

struct path {
    char *file;
    char *path;
};

typedef struct path Path;
typedef struct depth Depth;

/**
 * Creates Depth limits and calls for recursion search
 * 
 * @param opt - pointer to OPT structure storing all information
 */
int process(OPT *opt);

/**
 * Opens DIR directory with @param path
 * 
 * @param opt - information about read options
 * @param depth - used to determine if function should keep going deeper
 * @return true if search was successful, false otherwise
 */
bool recursion_directory(OPT *opt, char *path, Depth *depth, Path ***files_to_write, size_t *index);

/**
 * Gets array of paths, calls 'sortPath' to sort them by size or name
 * Prints all paths to stdout
 * 
 * @param paths - array of paths to print
 * @param size - size of array
 * @param opt - used to determine how to sort and how to print
 */
void printOut(Path **paths, size_t size, OPT *opt);

/**
 * Assigns depth limits to @param depth using @param opt ->t and ->f values
 */
void get_depth(OPT *opt, Depth *depth);

/**
 * Allocates space for one more pointer to store path at
 * 
 * @param files_to_write - array of pointers to paths to reallocate
 * @param size - current size of array
 * @return newly allocafted array of pointers
 */
Path **files_realloc(Path **files_to_write, size_t size);

/**
 * Sets options that change search pattern
 * 
 * @param user - true if file matches user name
 * @param string - true if substring matches file path
 * @param mask - true if permissions match
 * @param opt - stores values about user, string, mask
 * @param path - check all these search patterns on this file
 * @param file_name - only the file name used to compare for substring
 */
void set_options(bool *user, bool *string, bool *mask, OPT *opt, char *path, char *file_name);

/**
 * Compares paths by size of lexicographic order
 * 
 * @param first - path to compare
 * @param second -path to compare
 * @param s_type - option 's' | 'f' for size / file comparison
 * @return true if first file is 'bigger' than the second file, false if smaller
 */
bool comparePaths(Path *first, Path *second, char s_type);

/**
 * Sorts path by wanted comparison type
 * 
 * @param paths - array of pointers to paths to order
 * @param s_type - option 's' | 'f' for size / file comparison
 * @param size - size of current array
 */
void sortPath(Path **paths, char s_type, size_t size);

/**
 * Creates new absolute path of file and allocates space for the path in array
 * 
 * @param files_to_write - array of pointers to paths
 * @param path - file path to save
 * @param file - open dirent structure to current file
 * @param index - size of array
 * @param opt - used to check if file classifies for saving
 * @return true if successfully saved, false otherwise
 */
bool save_path(Path ***files_to_write, char *path, struct dirent *file, size_t *index, OPT *opt);

/**
 * Combines directory path and file name to create absolute path
 * 
 * @param initial_path - path to directory
 * @param entry_name - file name to add
 * @return pointer to memory storing absolute path
 */
char *create_path(char *initial_path, char *entry_name);

/**
 * Frees allocated memory for file paths
 * 
 * @param files - array of pointers to free
 * @param size - size of array
 */
void freePaths(Path **files, size_t size);
