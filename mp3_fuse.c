#define FUSE_USE_VERSION 28
#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdlib.h>
#include <pthread.h>

#define SUCCESS_STAT 0
#define MAXL 1024
static const char *dirpath = "/home/fandykun/hasilmp3";
const char *path_soal = "/home/fandykun";

int is_directory(const char *path);
int delete_files_and_directory(const char *path);
void get_mp3_from_directory(const char *path);
int is_mp3(const char *filename, char *ext);
void *init_fusethread(void *args);

char mountable[64];
char list_lagu[256][MAXL];
int idx_lagu = 0;
int same = 0;
pthread_t tid_fuse;

//=====================
//----Inisialisasi-----
//=====================
void *prefix_init(struct fuse_conn_info *conn)
{
    if (!is_directory(dirpath))
        mkdir(dirpath, 0750);

    pthread_create(&tid_fuse, NULL, &init_fusethread, NULL);
    return NULL;
}

//=====================
//-Inisialisasi thread-
//=====================
void *init_fusethread(void *args)
{
    get_mp3_from_directory(path_soal);
    return NULL;
}

//=====================
//---Untuk GetAttr-----
//=====================
static int prefix_getattr(const char *path, struct stat *stbuf)
{
    int res;
    char fpath[1000];
    sprintf(fpath, "%s%s", dirpath, path);
    res = lstat(fpath, stbuf);

    if (res == -1)
        return -errno;

    return 0;
}

//===========================
//----Untuk ReadDirectory----
//===========================
static int prefix_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
                          off_t offset, struct fuse_file_info *fi)
{
    char fpath[1000];
    if (strcmp(path, "/") == 0)
    {
        path = dirpath;
        sprintf(fpath, "%s", path);
    }
    else
        sprintf(fpath, "%s%s", dirpath, path);
    int res = 0;

    DIR *dp;
    struct dirent *de;

    (void)offset;
    (void)fi;

    dp = opendir(fpath);
    if (dp == NULL)
        return -errno;

    while ((de = readdir(dp)) != NULL)
    {
        struct stat st;
        memset(&st, 0, sizeof(st));
        st.st_ino = de->d_ino;
        st.st_mode = de->d_type << 12;
        res = (filler(buf, de->d_name, &st, 0));
        if (res != 0)
            break;
    }

    closedir(dp);
    return 0;
}

//===========================
//--------Untuk Read---------
//===========================
static int prefix_read(const char *path, char *buf, size_t size, off_t offset,
                       struct fuse_file_info *fi)
{
    char fpath[1000];
    if (strcmp(path, "/") == 0)
    {
        path = dirpath;
        sprintf(fpath, "%s", path);
    }
    else
        sprintf(fpath, "%s%s", dirpath, path);
    int res = 0;
    int fd = 0;

    (void)fi;
    fd = open(fpath, O_RDONLY);
    if (fd == -1)
        return -errno;

    res = pread(fd, buf, size, offset);
    if (res == -1)
        res = -errno;

    close(fd);
    return res;
}

//===========================
//--Destroy jika diunmount---
//===========================
void prefix_destroy(void *private_data)
{
    delete_files_and_directory(dirpath);
}

static struct fuse_operations prefix_oper = {
    .init = prefix_init,
    .getattr = prefix_getattr,
    .readdir = prefix_readdir,
    .read = prefix_read,
    .destroy = prefix_destroy,
};

int main(int argc, char *argv[])
{
    strcpy(mountable, argv[1]);
    umask(0);
    return fuse_main(argc, argv, &prefix_oper, NULL);
}

//===========================
//--flag untuk ngecek folder-
//===========================
int is_directory(const char *path)
{
    struct stat stat_buffer;
    if (stat(path, &stat_buffer) != 0)
        return 0;
    return S_ISDIR(stat_buffer.st_mode);
}

//===========================
//---Flag untuk ngecek mp3---
//===========================
int is_mp3(const char *filename, char *ext)
{
    int len_file = strlen(filename);
    if(strcmp(filename + (len_file - 4), ext) == 0)
        return 1;
    return 0;
}

//===========================
//---Hapus file dan folder---
//===========================
int delete_files_and_directory(const char *path)
{
    if(path == NULL) 
        return SUCCESS_STAT;
    DIR *folder = opendir(path);
    struct dirent *next_file;
    char filepath[1024];
    int ret_val;
    if(folder == NULL)
        return errno;
    
    while( (next_file = readdir(folder)) != NULL)
    {
        sprintf(filepath, "%s/%s", path, next_file->d_name);
        if ((strcmp(next_file->d_name, "..") == 0) ||
            (strcmp(next_file->d_name, "." ) == 0) )
        {
            continue;
        }
        // if(is_directory(filepath))
        // {

        // }
        ret_val = remove(filepath);
        if(ret_val != SUCCESS_STAT && ret_val != ENOENT)
        {
            closedir(folder);
            return ret_val;
        }
    }
    closedir(folder);
    rmdir(path);

    return SUCCESS_STAT;
}

//===========================
//------Filtering mp3--------
//===========================
void get_mp3_from_directory(const char *path)
{
    if(path == NULL || !is_directory(path))
        return;
    
    DIR *folder = opendir(path);
    struct dirent *content;
    char call[MAXL];
    char filepath[MAXL];
    char filename[MAXL];
    char src_path[MAXL], dst_path[MAXL];

    while( (content = readdir(folder)) != NULL)
    {
        sprintf(filepath, "%s/%s", path, content->d_name);
        if ((strcmp(content->d_name, "..") == 0) ||
            (strcmp(content->d_name, "." ) == 0) ||
            (strcmp(content->d_name, mountable) == 0) ||
            (content->d_name[0] == '.') )
        {
            continue;
        }
        if(is_directory(filepath))
        {
            get_mp3_from_directory(filepath);
        }
        else if(is_mp3(content->d_name, ".mp3"))
        {
            printf("current : path:%s name:%s\n", path, content->d_name);
            sprintf(filename, "%s", content->d_name);
            same = -1;
            for(int i = 0; i < idx_lagu; i++)
            {
                if(strcmp(list_lagu[i], content->d_name) == 0)
                {
                    same++;
                }
            }
            if(same > 0)
            {
                int len_file = strlen(content->d_name);
                char temp_name[MAXL];
                snprintf(temp_name, len_file - 3, "%s", content->d_name);
                sprintf(filename, "%s(%d).mp3", temp_name, same);
            }
            
            sprintf(list_lagu[idx_lagu++], "%s", content->d_name);

            sprintf(src_path, "%s/%s", path, content->d_name);
            sprintf(dst_path, "%s/%s", dirpath, filename);
            sprintf(call, "cp '%s' '%s'", src_path, dst_path);
            system(call);
        }
    }
    closedir(folder);
}