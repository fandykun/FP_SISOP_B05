#ifndef LAGU_H_
#define LAGU_H_

#include <stdio.h>
#include <pthread.h>
#include <termios.h>
#include <string.h>
#include <dirent.h>
#include <stdlib.h>
#include <unistd.h>
//----Library untuk play mp3----
#include <ao/ao.h>
#include <mpg123.h>
#define BITS 8
#define MAXLEN 256
#define MAXLIST 100
//-------Pre-define keperluan mp3------
#define MENU_UTAMA 10
#define MENU_PLAY 11
#define MENU_PLAYLIST 12
#define MENU_LIST_LAGU 13
#define PLAY 1
#define PAUSE 2
#define STOP 3
#define NEXT 4
#define PREV 5
#define PAGE 6
#define ADD 1
#define REMOVE 2

#define KNRM  "\x1B[0m"
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34m"
#define KMAG  "\x1B[35m"
#define KCYN  "\x1B[36m"
#define KWHT  "\x1B[37m"

//--------Deklarasi variabel----------
//===========Inisialisasi mpg123=========
mpg123_handle *music_handler;
unsigned char *buffer;
size_t buffer_size;
size_t done;

//===========Inisialisasi libao==========
int driver;
ao_device *device;
ao_sample_format format;
int channels, encoding;
long rate;

//=========Inisialisasi thread===========
pthread_t tid_musik;
pthread_t tid_menu;
pthread_t tid_keyboard;
pthread_mutex_t lock;

int status_menu;
int status_lagu;
int status_playlist;

int now_playing = 0;
int current_idx = -1;
int max_idx = 0;
int current_page = 0;
char command;

char daftar_lagu[MAXLIST][MAXLEN];

char nama_playlist[MAXLIST][MAXLEN];
int isi_playlist[MAXLIST][MAXLIST];

//---------Deklarasi fungsi-------------
void inisialisasi_music_player();
void close_music_player();
void list_lagu();
void print_lagu();

void menu_lagu();
void menu_play();
void menu_utama();

void menu_playlist();
void print_playlist();

void *baca_perintah_keyboard(void *args);
void *menu_music_player(void *args);
void *mainkan_musik(void *args);

char getch();

#endif