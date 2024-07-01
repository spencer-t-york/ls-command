#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include <time.h>
#include <pwd.h>
#include <grp.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

struct files { // linked list to point to dirent struct for sorting
    struct dirent *direntp;
    struct files *next;
};

// SORTING FUNCTION
struct files *sort(struct files *head) {
    // if 0 or 1 nodes
    if (head == NULL || head->next == NULL) {
        return head; // Return if the list is empty or has only one node
    }

    struct files *prev = NULL;
    struct files *current = head;
    struct files *last = NULL;
    bool swapped;
    int firstCharacterIndex = 0;

    do {
        swapped = false;
        current = head;
        prev = NULL;

        while (current->next != last) {
            // check if first letter of directory/file1 > first letter of directory/file2 (accounts for '.')
            if (strcasecmp(current->direntp->d_name + ((current->direntp->d_name[0] == '.') ? 1 : 0),
                       current->next->direntp->d_name + ((current->next->direntp->d_name[0] == '.') ? 1 : 0)) > 0)
            {
                struct files *temp = current->next; // second node
                current->next = temp->next;         // set first node to point to third
                temp->next = current;               // set second to point to first

                if (prev == NULL) {
                    head = temp;         // set the first node to head
                } else {
                    prev->next = temp;   // have the previous node point to the swapped node
                }

                prev = temp;             // move prev to next node
                swapped = true;

            } else {
                prev = current;          // move prev to next node
                current = current->next; // move current to the next node
            }
        }

        last = current; // Update last to the last swapped node
    } while (swapped);

    return head;
}

// FILE PERMISSIONS FUNCTION
void mode_to_str(mode_t mode, char *str) {
    str[0] = (S_ISDIR(mode)) ? 'd' : '-';   // directory indicator
    str[1] = (mode & S_IRUSR) ? 'r' : '-';  // read  for owner
    str[2] = (mode & S_IWUSR) ? 'w' : '-';  // write for owner
    str[3] = (mode & S_IXUSR) ? 'x' : '-';  // exec  for owner
    str[4] = (mode & S_IRGRP) ? 'r' : '-';  // read  for group
    str[5] = (mode & S_IWGRP) ? 'w' : '-';  // write for group
    str[6] = (mode & S_IXGRP) ? 'x' : '-';  // exec  for group
    str[7] = (mode & S_IROTH) ? 'r' : '-';  // read  for others
    str[8] = (mode & S_IWOTH) ? 'w' : '-';  // write for others
    str[9] = (mode & S_IXOTH) ? 'x' : '-';  // exec  for others
    str[10] = '\0';                         // null terminator
}

// CONVERT NUM TO MONTH NAME FUNCTION
const char* numberToMonth(int monthNumber) {
    const char* months[] = {
        "Jan", "Feb", "Mar", "Apr", "May", "Jun",
        "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
    };

    // Return the corresponding month abbreviation
    return months[monthNumber - 1];
}

// PRINT FUNCTION
void printFiles(struct files *head) {
    struct files *current = head;
    while (current != NULL){
        printf("%-25s\n", current->direntp->d_name);
        current = current->next;
    }
}

// PRINT FOR L FLAG FUNCTION
void printFilesL(struct files *head, const char *dirname) {
    struct stat stats;
    char fullpath[PATH_MAX];
    char mode_str[11];
    struct files *current = head;

   int blockCount = 0; // Total blocks used by files and directories

    // Calculate total blocks first
    while (current != NULL){
        snprintf(fullpath, PATH_MAX, "%s/%s", dirname, current->direntp->d_name);

        if (stat(fullpath, &stats) == -1) {
            perror("stat");
            return;
        }

        blockCount += (int)stats.st_blocks;

        current = current->next;
    }

    printf("total %d\n", (blockCount/2)); // print total block count

    // reset point to head
    current = head;


    while (current != NULL){
        // builds formatted file path to specified file
        snprintf(fullpath, PATH_MAX, "%s/%s", dirname, current->direntp->d_name);

        // if file does not exist or cannot be accessed
        if (stat(fullpath, &stats) == -1) {
            perror("stat");
            return;
        }

        mode_to_str(stats.st_mode, mode_str); // for permissions section

        printf("%s %ld %s %s %ld %s %2d  %d:%d %s\n",
            mode_str,                                 // permissions
            stats.st_nlink,                           // number of hard links
            getpwuid(stats.st_uid)->pw_name,          // owner name
            getgrgid(stats.st_gid)->gr_name,	      // group name
            stats.st_size,			      // file size in bytes
            numberToMonth(			      // convert num to text
                gmtime(&stats.st_atime)->tm_mon + 1), // month
            gmtime(&stats.st_atime)->tm_mday,         // day
            gmtime(&stats.st_atime)->tm_hour,         // hour
            gmtime(&stats.st_atime)->tm_min,          // minutes
            current->direntp->d_name                  // file name
        );

        current = current->next; // advance to next node
    }
}

// LS FUNCTION
void ls(char dirname[], int a, int l) {
    DIR *dir_ptr;

    struct dirent *direntp;    // temporary variable to hold each directory entry
    struct files *head = NULL;
    struct files *current = NULL;
    struct files *tail = NULL; // always points to the tail

    if ((dir_ptr=opendir(dirname)) == NULL) {
        fprintf(stderr, "cannot open %s\n", dirname);
	return;
    }

    while ((direntp = readdir(dir_ptr)) != NULL) {
        if (a == 0 && direntp->d_name[0] == '.') { // if 'a' flag is set then skip over
            continue;                              // files beginning with '.'
        }

        struct files *new_node = (struct files *)malloc(sizeof(struct files));
        new_node->direntp = direntp;
        new_node->next = NULL; // ensure the node after new_node is NULL

        // Add new node to end of linked list
        if (head == NULL) {
            // If head is NULL, set head to new_node
            head = new_node;
            tail = new_node;
        } else {
            // append new_node to the end of the linked list
            tail->next = new_node;
            tail = new_node;
        }
    }

    head = sort(head); // Sort

    if (l == 0) {      // Print
        printFiles(head);
    } else {          // Print Long Listing Format
        printFilesL(head, dirname);
    }

    closedir(dir_ptr);
}

int main(int argc, char *argv[]) {
    if (argc < 2 || argc > 3) {
        printf("usage: ls directory_name\n");
    }
    if (argc == 3) {
        if (strcmp(argv[1], "-a") == 0) {        // ./a.out -a /directory
            ls(argv[2], 1, 0);
        } else if (strcmp(argv[2], "-a") == 0) { // ./a.out /directory -a
            ls(argv[1], 1, 0);
        } else if (strcmp(argv[1], "-l") == 0) { // ./a.out -l /directory
            ls(argv[2], 0, 1);
        } else if (strcmp(argv[2], "-l") == 0) { // ./a.out /directory -l
            ls(argv[1], 0, 1);
        }
    } else {                                     // ./a.out /directory
        ls(argv[1], 0, 0);
    }

    return 0;
}
