#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <pwd.h>
#include <time.h>
#include <grp.h>
#include <sys/stat.h>
#include <sys/types.h>

void print_dir(char *dirname, u_int depth)
{
    DIR *dir = opendir(dirname);
    struct dirent *d;
    printf("%s:\n", dirname);
    printf("total %d\n", depth);

    while ((d = readdir(dir)) != NULL)
    {
        struct stat s;
        lstat(d->d_name, &s);
        if (S_ISDIR(s.st_mode))
        {
            if (strcmp(d->d_name, ".") == 0 || strcmp(d->d_name, "..") == 0)
                continue;
        }

        char mode[11] = {
            S_ISDIR(s.st_mode) ? 'd' : '-',
            s.st_mode & 256 ? 'r' : '-',
            s.st_mode & 128 ? 'w' : '-',
            s.st_mode & 64 ? 'x' : '-',
            s.st_mode & 32 ? 'r' : '-',
            s.st_mode & 16 ? 'w' : '-',
            s.st_mode & 8 ? 'x' : '-',
            s.st_mode & 4 ? 'r' : '-',
            s.st_mode & 2 ? 'w' : '-',
            s.st_mode & 1 ? 'x' : '-',
            0};
        struct passwd *pwd = getpwuid(s.st_uid);
        struct group *grp = getgrgid(s.st_gid);
        char time_s[128];
        strftime(time_s, 128, "%Y-%m-%d %I:%M", localtime(&s.st_ctime));
        printf("%10s %3d %s %s %6d %s %s\n", mode, s.st_nlink, pwd->pw_name, grp->gr_name, s.st_size, time_s, d->d_name);
    }
    closedir(dir);
    printf("\n");

    dir = opendir(dirname);
    chdir(dirname);
    while ((d = readdir(dir)) != NULL)
    {
        struct stat s;
        lstat(d->d_name, &s);
        if (S_ISDIR(s.st_mode))
        {
            if (strcmp(d->d_name, ".") == 0 || strcmp(d->d_name, "..") == 0)
                continue;
            print_dir(d->d_name, depth + 4);
        }
    }
    chdir("..");
    closedir(dir);
    printf("\n");
}

int main()
{
    print_dir(".", 0);
}