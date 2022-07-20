#include "find_and_path.h"

#include <limits.h>

#define UINT_MAX (INT_MAX * 2U + 1U)

void convertToDec(size_t *octal);
void freeOpenedDir(DIR *dir, struct stat *sb, char *entry_path);
void toLowerCase(char *path);

int process(OPT *opt)
{
    size_t index = 0;
    Path **files_to_write = NULL;
    Depth *depth = malloc(sizeof(Depth));
    get_depth(opt, depth);
    if (!recursion_directory(opt, opt->path, depth, &files_to_write, &index)) {
        free(depth);
        return 1;
    }
    
    if (!files_to_write) {
        free(depth);
        return 0;
    }
    // sorts array of paths and prints it
    printOut(files_to_write, index, opt);
    freePaths(files_to_write, index);
    free(depth);
    return 0;
}
/*
int main(int argc, char *argv[])
{
    OPT *opt = malloc(sizeof(OPT));
    opt->t = malloc(sizeof(Param));
    opt->t->argument = malloc(1);
    opt->t->argument[0] = '1';
    opt->path = malloc(strlen(argv[2]) + 1);
    strcpy(opt->path, argv[2]);
    process(opt);
    free(opt->path);
    free(opt);
}*/

bool recursion_directory(OPT *opt, char *path, Depth *depth, Path ***files_to_write, size_t *index)
{
    DIR *dir = NULL;
    if ((dir = opendir(path)) == NULL) {
        perror(path);
        // if opendir failed to read the initial directory -> return false
        // else returns true and continues searching for future directories
        // depth->depth describes opened directories,
        // therefore depth 1 is initial directory
        const bool failed_to_open_initial_path = depth->depth != 1;
        depth->depth--;
        return failed_to_open_initial_path;
    } else if (depth->t < depth->depth) {    // depth max exceeded
        closedir(dir);
        depth->depth--;
        return true;
    }
    struct dirent *dirEntry = NULL;
    while ((dirEntry = readdir(dir)) != NULL) {    // read contents
        char *entry_path;    // constructed path of file
        // creating path failed
        entry_path = create_path(path, dirEntry->d_name);
        struct stat *sb = calloc(sizeof(struct stat), 1);
        stat(entry_path, sb);
        if (!entry_path) {    // allocation failed
            freeOpenedDir(dir, sb, entry_path);
            freePaths(*files_to_write, *index);
            return false;
        
        } else if (!sb) {    // creating stat structure failed
            fprintf(stderr, "Cannot read information about: %s\n", entry_path);
            continue;
        }
        
        // if directory is not itself (. / ..) -> read it's content
        bool canSearch = strcmp("..", dirEntry->d_name) != 0 &&
                         strcmp(".", dirEntry->d_name)  != 0;
        // trying to read hidden dir when not allowed                 
        if (*dirEntry->d_name == '.' && !opt->hidden) {
            canSearch = false;
        }
        if (S_ISDIR(sb->st_mode) && canSearch) {
            depth->depth++;
            if (!recursion_directory(opt, entry_path, depth, files_to_write, index)) {
                freeOpenedDir(dir, sb, entry_path);
                freePaths(*files_to_write, *index);
                return false;
            }

        // valid depth and file is regular -> save file path
        } else if (depth->depth >= depth->f && S_ISREG(sb->st_mode)) {
            if (!save_path(files_to_write, entry_path, dirEntry, index, opt)) {
                freeOpenedDir(dir, sb, entry_path);
                return false;
            }
        }
        freeOpenedDir(NULL, sb, entry_path);    // free current stat and path
    }
    depth->depth--;
    closedir(dir);
    return true;
}

void printOut(Path **paths, size_t size, OPT *opt)
{
    char s_type = 'x';
    char delimiter = '\n';
    if (opt->sort)
        s_type = *opt->sort->argument;
    if (opt->null_bytes)
        delimiter = '\0';
    sortPath(paths, s_type, size);
    for (size_t i = 0; i < size; i++) {
        printf("%s%c", paths[i]->path, delimiter);
    }
}

void freeOpenedDir(DIR *dir, struct stat *sb, char *entry_path)
{
    if (dir) {
        closedir(dir);
    }
    if (sb) {
        free(sb);
    }
    if (entry_path) {
        free(entry_path);
    }
}
// ==========                   ==========
//            SUPPORT FUNCTIONS
// ==========                   ==========

void get_depth(OPT *opt, Depth *depth)
{
    if (opt->f) {
        depth->f = strtol(opt->f->argument, NULL, 0);
    } else {
        depth->f = 1;
    }
    if (opt->t) {
        depth->t = strtol(opt->t->argument, NULL, 0);
    } else {
        depth->t = UINT_MAX;
    }
    depth->depth = 1;
}


void set_options(bool *user, bool *string, bool *mask, OPT *opt, char *path, char *file_name)
{
    struct stat *sb = malloc(sizeof(struct stat));
    stat(path, sb);
    struct passwd *usr = getpwuid(sb->st_uid);
    if (opt->user) {
        *user = strcmp(opt->user->argument, usr->pw_name) == 0;
    }
    if (opt->mask) {
        size_t input_permission = strtol(opt->mask->argument, NULL, 0);
        convertToDec(&input_permission);
        size_t file_permission = (sb->st_mode & S_IRWXU) +
                                 (sb->st_mode & S_IRWXG) +
                                 (sb->st_mode & S_IRWXO);
        *mask = file_permission == input_permission;
    }
    if (opt->name) {
        *string = strstr(file_name, opt->name->argument) != NULL;
    }
    free(sb);
}

Path **files_realloc(Path **files_to_write, size_t index)
{
    Path **tmp = realloc(files_to_write, index * sizeof(Path*) + sizeof(Path*));
    if (!tmp) {
        fprintf(stderr, "Allocating memory for paths failed\n");
        freePaths(files_to_write, index);
        return NULL;
    }
    return tmp;
}

/**
 * Converts octal number to decimal
 * @param octal - number to convert - overrides to dec
 */
void convertToDec(size_t *octal)
{
    int dec = 0, base = 1;
    while(*octal) {
        dec += (*octal % 10) * base;
        *octal /= 10;
        base *= 8;
    }
    *octal = dec;
}
