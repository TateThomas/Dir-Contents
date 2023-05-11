/*
 * Project 8: Read directory and file information
 * CS 3060
 * Author: Tate Thomas
 * Date: 4/22/23
 *
 * Contains the function inspect_dir for printing a directory and retreiving
 * the total space taken up by that directory, and a main function for using
 * command line input to specify which directory to inspect.
 */


#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>


/*
 * Function:	inspect_dir
 * ------------------------
 * Description:
 *
 *	Inspects the contents of a given directory and prints the results.
 *
 *	When a subdirectory is encountered, it is treated as another directory
 *	inspection, indenting itself 2 spaces relative to the parent directory.
 *
 *	When a file or symbolic link is encountered, it prints the size and name
 *	of the file. Each file in a directory is lined up to each other with ':',
 *	which is indented 8 spaces to the right of the parent directory. The size
 *	is printed on the left of ':', justified to the right, and the name is
 *	printed to the right of ':', justified to the left.
 *
 * Parameters:
 *
 *	str: The path to the directory being inspected, relative to the
 *	     current directory.
 *
 * Returns:
 *
 *	The total amount of space the given directory takes up, given as
 *	unsigned long long int.
 * ------------------------
 */

unsigned long long int inspect_dir(const char* str) {

	char* dir; 	// path to directory
	int spaces = 0; 	// offset for print to show directory depth

	// find the amount of spaces added to the end; create a copy to dir without the spaces
	while (str[strlen(str) - 1 - spaces] == ' ') spaces++;
	asprintf(&dir, "%.*s", (int)(strlen(str) - spaces), str);

	DIR *dirp; 	// pointer to working directory
	if ((dirp = opendir(dir)) == NULL) {
		printf("%*s open %s", spaces + 2, "Couldn't", dir);
		return 0;
	}

	struct dirent *dp; 	// pointer to directory entry data
        struct stat buf; 	// file/directory statistics
	unsigned long long int total_size = 0;

	printf("%*s %s\n", spaces + 3, "dir", dir);

	// read through each directory entry until it has reached the end
	do {
		if ((dp = readdir(dirp)) != NULL) {

			// ignore hidden files/directories
			if (dp->d_name[0] == '.') {
				continue;
			}

			// create a new string representing the path to the current directory entry
			char* path;
			asprintf(&path, "%s/%s", dir, dp->d_name);

			// retreive statistics on the entry, check for error
			if (lstat(path, &buf) == -1) {
				printf("%*s error occured while retreiving stats for %s\n", spaces + 2, "An", path);
                                free(path);
				continue;
			}

			// determine the type of the entry
			if (S_ISDIR(buf.st_mode) == 1) {
				// create new string for the path with spaces added for indentation later
				char* strp;
				asprintf(&strp, "%s%*s", path, spaces + 2, "  ");

				total_size += inspect_dir(strp);
				free(strp);
			}
			else if ((S_ISREG(buf.st_mode) == 1) || (S_ISLNK(buf.st_mode) == 1)) {
				// print the size and name of file, lining up each file with :
				printf("%*ld:%s\n", spaces + 8, buf.st_size, dp->d_name);
				total_size += buf.st_size;
			}
			else {
				printf("%*s file type (%s)\n", spaces + 7, "Unknown", path);
			}
			free(path);
		}
	} while (dp != NULL);

	closedir(dirp);
	free(dir);

	return total_size;
}


/*
 * Function:	main
 * -----------------
 * Description:
 *
 *	Coordinates which directory to inpsect. Prints the total file
 *	space used.
 *
 *	If no arguments are given, it is assumed that the current
 *	directory is to be inspected.
 *
 *	If 1 argument is given, it will use that path to inspect
 *	that directory.
 *
 * Arguments:
 *
 *	argv[1]: The path to the desired directory to be inspected. No
 *		 more than 1 argument should be given.
 *
 * Returns:
 *
 * 	Code 0: no errors.
 *	Code 1: Too many inputs.
 */

int main(int argc, char** argv) {

	unsigned long long int space_used;

	if (argc == 1) {
		space_used = inspect_dir(".");
	}
	else if (argc == 2) {
		space_used = inspect_dir(argv[1]);
	}
	else {
		printf("Error: Only 1 directory input is accepted.\n");
		return 1;
	}

	printf("\nTotal file space used:%lld\n", space_used);

	return 0;
}
