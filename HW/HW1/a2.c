#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>
#include <errno.h>
#include <limits.h>
#include "math.h"


int show_all = 0;
int human_readable = 0;
long inode_arr[99999];
int inode_arr_ptr=0;

double get_file_usage(const char *path);
void print_usage(const char *path, double size);

int main(int argc, char *argv[]) {
    int opt;
    int dir_count=0;
    char* dir_paths[argc];

    while ((opt = getopt(argc, argv, "ah")) != -1) {
        switch (opt) {
            case 'a':
                show_all = 1;
                break;
            case 'h':
                human_readable = 1;
                break;
        }
    }
    
    while(optind<argc)
    	dir_paths[dir_count++]=argv[optind++];
    	
    if(dir_count==0)
    {
        dir_paths[dir_count++]=".";
    }
    
    for(int i=0;i<dir_count;i++)
    {
    	get_file_usage(dir_paths[i]);
    }
    
    return 0;
}

double get_file_usage(const char *path) {
    struct stat sb;
    
    if (lstat(path, &sb) == -1) {
        perror("lstat");
        exit(1);
    }
    

    if (S_ISREG(sb.st_mode)) 
    {
    	if(show_all)
            print_usage(path, sb.st_blocks * 512);
            
        return sb.st_blocks * 512;
    } 
    else if (S_ISDIR(sb.st_mode)) 
    {
        double total = 4096;
        DIR *dir = opendir(path);

        if (dir == NULL) {
            perror("opendir");
            exit(1);
        }

        struct dirent *entry;

        while ((entry = readdir(dir)) != NULL) 
        {
            int flag=0;
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) 
            {
                continue;
            }
            else
            {
		        for(int k=0;k<inode_arr_ptr;k++)
		        {
		            if(inode_arr[k]==entry->d_ino)
		        	{
						flag=1;
						break;
				    }
		        	     
		        }
				if(flag==0)
				{
				inode_arr[++inode_arr_ptr]=entry->d_ino;
		        char fullpath[PATH_MAX];
		        snprintf(fullpath, sizeof(fullpath), "%s/%s", path, entry->d_name);
		        total += get_file_usage(fullpath);
		        }
            }
        }

        closedir(dir);

        print_usage(path, total);

        return total;
    }

}

void print_usage(const char *path, double size) {
    if (human_readable) 
    {
        const char *units[] = {"", "K", "M", "G", "T", "P", "E", "Z", "Y"};
        int unit = 0;
       
        while (size >= 1024 && unit < 8) 
        {
            size /= 1024;
            unit++;
        }
        
        if(size<10){
        	//double tmp=ceil(size)/10;
            printf("%.1f%s\t%s\n", size, units[unit], path);
            }
        else{
        	//double tmp=ceil(size)/10;
            printf("%.f%s\t%s\n", size, units[unit], path);
            }
    } 
    else 
    {
        printf("%g\t%s\n", size/1024, path);
    }
}
