#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>
#include <errno.h>

void get_directory_usage(const char *path, int human_readable);
long get_file_usage(const char *path, int human_readable);
void print_usage(const char *path, long size, int human_readable);

int main(int argc, char *argv[]) {
    int opt;
    int show_all = 0;
    int human_readable = 0;

    while ((opt = getopt(argc, argv, "ah")) != -1) {
        switch (opt) {
            case 'a':
                show_all = 1;
                break;
            case 'h':
                human_readable = 1;
                break;
            default:
                fprintf(stderr, "Usage: %s [-ah] <path>\n", argv[0]);
                exit(1);
        }
    }

    //exception
    if (optind == argc) 
    {
        fprintf(stderr, "Usage: %s [-ah] <path>\n", argv[0]);
        exit(1);
    }

    
    if (show_all) 
    {
        get_file_usage(argv[optind], human_readable);
    }
    get_file_usage(argv[optind], human_readable);
   
    
    return 0;
}

void get_directory_usage(const char *path, int human_readable) {
    struct stat sb;

    //exception
    if (lstat(path, &sb) == -1) 
    {
        perror("lstat");
        exit(1);
    }

    if (S_ISDIR(sb.st_mode)) {
        print_usage(path, sb.st_blocks * 512, human_readable);

        DIR *dir = opendir(path);
        if (dir == NULL) {
            perror("opendir");
            exit(1);
        }

        struct dirent *entry;

        while ((entry = readdir(dir)) != NULL) {
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
                continue;
            }

            char fullpath[PATH_MAX];
            snprintf(fullpath, sizeof(fullpath), "%s/%s", path, entry->d_name);
            
        }

        closedir(dir);
    }
}

long get_file_usage(const char *path, int human_readable) {
    struct stat sb;

    //exception
    if (lstat(path, &sb) == -1) 
    {
        perror("lstat");
        exit(1);
    }

    if (S_ISREG(sb.st_mode)) 
    {
        print_usage(path, sb.st_blocks * 512, human_readable);
        return sb.st_blocks*512;
    } 
    
    else if (S_ISDIR(sb.st_mode)) 
    {
    	long total=sb.st_blocks*512;
        
        DIR *dir = opendir(path);
        
        //exception
        if (dir == NULL) 
        {
            perror("opendir");
            exit(1);
        }

        struct dirent *entry;

        while ((entry = readdir(dir)) != NULL) {
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
                continue;
            
            char fullpath[PATH_MAX];
            snprintf(fullpath, sizeof(fullpath), "%s/%s", path, entry->d_name);
            total+=get_file_usage(fullpath, human_readable);
            
            struct stat temp;
            lstat(fullpath, &temp);
            if(S_ISDIR(temp.st_mode))
            	print_usage(fullpath, total, human_readable);
        }

        closedir(dir);
        return total;
    }
    
}

void print_usage(const char *path, long size, int human_readable) {
    struct stat ttemp;
    lstat(path, &ttemp);
    
    if (human_readable) 
    {
        const char *units[] = {"", "K", "M", "G", "T"};
        int unit = 0;
        double sz = (double)size;
        while (sz >= 1024 && unit < 4) {
            sz /= 1024;
            unit++;
        }
        printf("%.2f%s\t%s\n", sz, units[unit], path);
    } else {
        printf("%ld\t%s\n", size/1000, path);
    }
}
