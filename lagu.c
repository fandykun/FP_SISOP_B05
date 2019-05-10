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

void menu_utama()
{
    printf("1. Mainkan lagu\n");
    printf("2. Daftar lagu saja\n");
    printf("3. Exit\n");
    printf("================================================\n");
}

void menu_lagu()
{
    printf("==================DAFTAR-LAGU===================\n");
    list_lagu();
    printf("================================================\n");
    printf("Tekan apapun untuk kembali\n");
    printf("================================================\n");
}

void menu_play()
{
    printf("==================DAFTAR-LAGU===================\n");
    list_lagu();
    printf("================================================\n");
    printf("1. Play\n");
    printf("2. Pause or resume\n");
    printf("3. Stop\n");
    printf("4. Previous song\n");
    printf("5. Next song\n");
    printf("6. Kembali ke menu utama\n");
    printf("================================================\n");
}

void *baca_perintah_keyboard(void *args)
{
    command = 0;
    while (command != 32)
    {
        command = getch();
        if(status_menu == MENU_UTAMA)
        {
            switch (command)
            {
            case '1':
                status_menu = MENU_PLAY;
                break;
            case '2':
                status_menu = MENU_LIST_LAGU;
                break;
            case '3':
                command = 32;
                break;
            default:
                break;
            }
        }
        else if(status_menu == MENU_LIST_LAGU)
        {
            switch (command)
            {
            case 'a':
                // status_menu = MENU_UTAMA;
            default:
                status_menu = MENU_UTAMA;
                break;
            }
        }
        else if(status_menu == MENU_PLAY)
        {
            switch (command)
            {
            case '1':
                if(status_lagu != PLAY)
                {
                    status_lagu = PLAY;
                    pthread_create(&tid_musik, NULL, &mainkan_musik, (void *)daftar_lagu[3]);
                }
                break;
            case '2':
                if(status_lagu == PLAY)
                    status_lagu = PAUSE;
                else if(status_lagu == PAUSE)
                    status_lagu = PLAY;
                break;
            case '3':
                status_lagu = STOP;
                break;
            case '6':
                status_lagu = STOP;
                status_menu = MENU_UTAMA;
                break;
            default:
                break;
            }
        }
    }
}

void *menu_music_player(void *args)
{
    static int retsys;
    int dummy;
    while(command != 32)
    {
        dummy = system("echo \"==================MUSIC PLAYER==================\" | lolcat ");
        dummy = system("echo \"================================================\" | lolcat ");
        switch (status_menu)
        {
        case MENU_UTAMA:
            menu_utama();
            break;
        case MENU_PLAY:
            menu_play();
            break;
        case MENU_LIST_LAGU:
            menu_lagu();
            break;
        default:
            break;
        }
        sleep(1);
        retsys = system("clear");
    }
}

void *mainkan_musik(void *args)
{
    char judul_lagu[MAXLEN];
    char path_lagu[MAXLEN];
    strcpy(judul_lagu, (void *) args);
    sprintf(path_lagu, "%s/%s", path, judul_lagu);

    play_lagu(path_lagu);
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

void play_lagu(char* nama_file)
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

    static int playing;
    do {
        playing = mpg123_read(music_handler, buffer, buffer_size, &done);
        switch (status_lagu)
        {
        case PAUSE:
            while(status_lagu != PLAY)
                ;
            break;
        case STOP:
            playing = !MPG123_OK;            
        default:
            break;
        }
        ao_play(device, buffer, done);
    } while (playing  == MPG123_OK);
}

void list_lagu()
{
    static int index;
    static int len_lagu;
    char lagu[MAXLEN];
    char *ext = ".mp3";
    
    index = 0;
    char abjad = 'A';
    struct dirent *file;
    DIR *directory = opendir(path);
    if(directory == NULL)
    {
        printf("SALAH DIRECTORY bgst");
        return;
    }
    while( (file = readdir(directory)) != NULL)
    {
        len_lagu = strlen(file->d_name);
        sprintf(lagu, "%s", file->d_name);
        if(strcmp(lagu + (len_lagu - 4), ext) == 0)
        {
            printf("%c. %s\n", abjad + index, file->d_name);
            sprintf(daftar_lagu[index++], "%s", lagu);
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