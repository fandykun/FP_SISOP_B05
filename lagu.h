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
#define MENU_LIST_LAGU 12
#define PLAY 1
#define PAUSE 2
#define STOP 3
#define NEXT 4
#define PREV 5

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

int status_menu;
int status_sub_menu;
int status_lagu;
int index_lagu;
char command;
char daftar_lagu[MAXLIST][MAXLEN];

//---------Deklarasi fungsi-------------
void inisialisasi_music_player();
void baca_lagu(char* nama_file);
void play_lagu();
void close_music_player();
void list_lagu();

void *baca_perintah_keyboard(void *args);
void *menu_music_player(void *args);
void *mainkan_musik(void *args);

char getch();

#endif