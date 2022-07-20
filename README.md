# Search file
School project that works similarly to bash 'find', has several options that restrict/extend search

I am unable to provide the project requirements as I do not have the right to share them.

This C programme is based on bash 'find' function. It recursively searches the current or specified directory for file(s) that match the criteria
set by options. Basic criteria like file name, owner, mask, depth of recursion and if the found files are sorted by their size or path.
If all criteria set by options are met and a file(s) is found it's path is returned.

Usage:

                    ./my_find -OPTIONS [DIRECTORY]\n
                    if no directory is supplied - searches working directory
                    -n NAME\t Filter by substring NAME
                    -s SORT\t Sort 's' by size / 'f' by path
                    -m MASK\t Filter by permission
                    -u USER\t Filter by user name
                    -f NUM \t Filter by minimal depth
                    -t NUM \t Filter by maximum depth
                    -a     \t Hidden directories
                    -0     \t Use null byte as line separator
                    -h     \t Show help
