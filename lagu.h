#ifndef LAGU_H_
#define LAGU_H_

#include <stdio.h>
#include <pthread.h>
#include <termios.h>
#include <string.h>
//----Library untuk play mp3----
#include <ao/ao.h>
#include <mpg123.h>
#define BITS 8
//-------Pre-define keperluan mp3------
#define LIST 0
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
pthread_t thread;

//---------Deklarasi fungsi-------------
void inisialisasi_lagu();
void baca_lagu(char* nama_file);
void play_lagu();
void tutup_lagu();

#endif