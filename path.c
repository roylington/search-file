#include "find_and_path.h"

void toLowerCase(char *path);

bool comparePaths(Path *first, Path *second, char s_type)
{
    // type 'x' means implicit sort
    if (s_type == 'x') {
        int result = strcmp(first->file, second->file);
        if (result == 0) {
            return comparePaths(first, second, 'f');
        }
        return result < 0;
    }
    if (s_type == 's') {
        struct stat *frst = malloc(sizeof(struct stat));
        struct stat *snd  = malloc(sizeof(struct stat));
        stat(first->path, frst);    // already know I can read these files (checked while inputting)
        stat(second->path, snd);
        size_t x = frst->st_size;
        size_t y = snd->st_size;
        free(frst);
        free(snd);
        // same file size -> compare by path
        if (x == y) {
            return comparePaths(first, second, 'x');
        }
        return x > y;
    }
    // option 'f' (input already checked to be valid while parsing)
    return strcmp(first->path, second->path) < 0;
}

void sortPath(Path **paths, char s_type, size_t size)
{
    for (size_t i = 0; i < size; i++) {
        for (size_t j = 1; j < size; j++) {
            // true if first arg is bigger than second arg
            if (comparePaths(paths[j], paths[j-1], s_type)) {
                // swap pointers
                Path *tmp  = paths[j];
                paths[j]   = paths[j-1];
                paths[j-1] = tmp;
            }
        }
    }
}

bool save_path(Path ***files_to_write, char *path, struct dirent *file, size_t *index, OPT *opt)
{
    bool not_hidden = '.' != *file->d_name;     // current file visibility
    bool correct_user = true, correct_string = true, correct_mask = true;
    set_options(&correct_user, &correct_string, &correct_mask, opt, path, file->d_name);

    // save if hidden files are allowed or file is ordinary (not hidden)
    // user / string / mask are set to true
    // changed to false by set_options (only if non matching file found)
    if ((opt->hidden || not_hidden) && correct_user && correct_mask && correct_string) {
        // reallocate path array memory
        *files_to_write = files_realloc(*files_to_write, *index);
        if (!*files_to_write)
            return false;
        // allocate memory for path
        (*files_to_write)[*index] = calloc(sizeof(Path), 1);
        (*files_to_write)[*index]->file = calloc(strlen(file->d_name) + 2, 1);
        toLowerCase(file->d_name);
        strcpy((*files_to_write)[*index]->file, file->d_name);
        (*files_to_write)[*index]->path = calloc(strlen(path) + 2, 1);
        strcpy((*files_to_write)[*index]->path, path);
        (*index)++;
    }
    return true;
}

char *create_path(char *initial_path, char *entry_name)
{
    char *path;
    int size = strlen(initial_path);
    if ((path = malloc(size + strlen(entry_name) + 2)) == NULL) {
        fprintf(stderr, "Unsuccessful alloc\n");
        return NULL;
    }
    strcpy(path, initial_path);
    if (initial_path[size-1] != '/')
        strcat(path, "/");
    strcat(path, entry_name);    // create path
    return path;
}

void freePaths(Path **files, size_t size)
{
    if (!files)
        return;
    for (size_t i = 0; i < size; i ++) {
        if (files[i]) {
            if (files[i]->file)
                free(files[i]->file);
            if (files[i]->path)
                free(files[i]->path);
            free(files[i]);
        }
    }
    free(files);
}

void toLowerCase(char *path)
{
    for (size_t i = 0; i < strlen(path); i++) {
        path[i] = tolower(path[i]);
    }
}