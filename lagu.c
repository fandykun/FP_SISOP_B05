#include "lagu.h"

const char *path = "music";

int main(int argc, char *argv[])
{
    status_menu = MENU_UTAMA;

    inisialisasi_music_player();

    pthread_create(&tid_keyboard, NULL, &baca_perintah_keyboard, NULL);
    pthread_create(&tid_menu, NULL, &menu_music_player, NULL);

    pthread_join(tid_keyboard, NULL);
    pthread_join(tid_menu, NULL);

    close_music_player();

    return 0;
}

void *baca_perintah_keyboard(void *args)
{
    command = 0;
    while (command != 32)
    {
        command = getch();

    }
}

void *menu_music_player(void *args)
{
    
}

void *mainkan_musik(void *args)
{
    char nama_lagu[MAXLEN];
    strcpy(nama_lagu, (void *) args);

    baca_lagu(nama_lagu);
    play_lagu();
}

void inisialisasi_music_player()
{
    static int err;
    ao_initialize();
    driver = ao_default_driver_id();
    mpg123_init();
    music_handler = mpg123_new(NULL, &err);
    buffer_size = mpg123_outblock(music_handler);
    buffer = (unsigned char*) malloc(buffer_size * sizeof(unsigned char));
}

void baca_lagu(char* nama_file)
{
    //Membuka file dan mendapatkan format lagunya
    mpg123_open(music_handler, nama_file);
    mpg123_getformat(music_handler, &rate, &channels, &encoding);

    //mengatur format musik agar dieksekusi oleh libao
    format.bits = mpg123_encsize(encoding) * BITS;
    format.rate = rate;
    format.channels = channels;
    format.byte_format = AO_FMT_NATIVE;
    format.matrix = 0;
    device = ao_open_live(driver, &format, NULL);
}

void play_lagu()
{
    static int playing;
    playing = mpg123_read(music_handler, buffer, buffer_size, &done);
    while (playing  == MPG123_OK)
    {
        switch (status_lagu)
        {
        case PLAY:
            ao_play(device, buffer, done);
            break;
        case PAUSE:
            while(status_lagu == PAUSE)
                ;
            break;
        case STOP:
            playing = !MPG123_OK;            
        default:
            break;
        }
    }
}

void list_lagu()
{
    static int index;
    static int len_lagu;
    static char lagu[MAXLEN];
    char *ext = ".mp3";
    
    index = 0;
    struct dirent *file;
    DIR *directory = opendir(path);
    if(directory == NULL)
    {
        printf("SALAH DIRECTORY");
        return;
    }
    while( (file = readdir(directory)) != NULL)
    {
        len_lagu = strlen(file->d_name);
        snprintf(lagu, MAXLEN, "%s", file->d_name);
        if(strcmp(lagu + (len_lagu - 4), ext) == 0)
        {
            sprintf(daftar_lagu[index++], "%s", lagu);   
            printf("%d. %s\n", index, file->d_name);
        }
    }
    closedir(directory);
}

void close_music_player()
{
    free(buffer);
    ao_close(device);
    mpg123_close(music_handler);
    mpg123_delete(music_handler);
    mpg123_exit();
    ao_shutdown();
}

//---Referensi implementasi getch pada C
//https://stackoverflow.com/questions/421860/capture-characters-from-standard-input-without-waiting-for-enter-to-be-pressed/912796#912796
//----------------------------------------
char getch()
{
    char buf = 0;
    struct termios old = {0};
    if (tcgetattr(0, &old) < 0)
        perror("tcsetattr()");
    old.c_lflag &= ~ICANON;
    old.c_lflag &= ~ECHO;
    old.c_cc[VMIN] = 1;
    old.c_cc[VTIME] = 0;
    if (tcsetattr(0, TCSANOW, &old) < 0)
        perror("tcsetattr ICANON");
    if (read(0, &buf, 1) < 0)
        perror("read()");
    old.c_lflag |= ICANON;
    old.c_lflag |= ECHO;
    if (tcsetattr(0, TCSADRAIN, &old) < 0)
        perror("tcsetattr ~ICANON");
    return (buf);
}