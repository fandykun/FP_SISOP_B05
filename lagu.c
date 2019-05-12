#include "lagu.h"

const char *path = "music"; //Path mengarah ke hasil fuse

int main(int argc, char *argv[])
{
    status_menu = MENU_UTAMA;
    int handler = system("clear");
    inisialisasi_music_player();

    pthread_mutex_init(&lock, NULL);
    pthread_create(&tid_keyboard, NULL, &baca_perintah_keyboard, NULL);
    pthread_create(&tid_menu, NULL, &menu_music_player, NULL);

    pthread_join(tid_keyboard, NULL);
    pthread_join(tid_menu, NULL);

    close_music_player();
    pthread_mutex_destroy(&lock);
    return 0;
}

//=====================
//---- Menu utama -----
//=====================
void menu_utama()
{
    printf("1. Mainkan lagu (semua lagu)\n");
    printf("2. Playlist\n");
    printf("3. Exit\n");
    printf("================================================\n");
}

void menu_lagu()
{
    printf("====================PLAYLIST====================\n");
    printf("MILIK = \t%s", nama_playlist[playlist_idx]);
    printf("================================================\n");
    
    printf("================================================\n");
    printf("1. Tambahkan lagu.\n");
    printf("2. Hapus lagu\n");
    printf("4. Kembali ke menu utama\n");
}

void menu_play()
{
    char text[16];
    if(now_playing) strcpy(text, "(now playing)");
    else strcpy(text, " ");
    list_lagu();
    printf("==================DAFTAR-LAGU===================\n");
    print_lagu();
    printf("================================================\n");
        if(current_idx == -1)
        printf("Silahkan memilih lagu dengan menekan sesuai abjad.\n");
    else
        printf("Current song: %s %s\n", daftar_lagu[current_idx], text);
    printf("================================================\n");
    printf("1. Play\n");
    printf("2. Pause or resume\n");
    printf("3. Stop\n");
    printf("4. Previous song\n");
    printf("5. Next song\n");
    printf("6. Previous Page\n");
    printf("7. Next Page\n");
    printf("8. Kembali ke menu utama\n");
    printf("========================%c=======================\n", command);

    if(status_lagu == PREV || status_lagu == NEXT) 
        status_lagu = PLAY;
    if( !now_playing && status_lagu == PLAY) 
    {
        pthread_cancel(tid_musik);
        pthread_create(&tid_musik, NULL, &mainkan_musik, (void *)daftar_lagu[current_idx]);
        now_playing = 1;
    }
}

void menu_playlist()
{
    static char *handler;
    if(max_playlist_idx == 0) printf("Silahkan menambahkan playlist terlebih dahulu.\n");
    else {
        printf("=================DAFTAR PLAYLIST================\n");
        print_playlist();
    }
    printf("================================================\n");
    if(playlist_idx == -1 && max_playlist_idx > 0) 
        printf("Silahkan memilih playlist dengan menekan sesuai abjad.\n");
    else if(playlist_idx >= 0)
    {
        printf("Current playlist : %s", nama_playlist[playlist_idx]);
        printf("================================================\n");
    }
    printf("1. Open playlist");
    if(max_playlist_idx <= 0)
        printf("(tambahkan playlist terlebih dahulu)");
    else if(playlist_idx < 0) 
        printf("(pilih playlist terlebih dahulu)");
    printf("\n");
    printf("2. Tambah playlist\n");
    printf("3. Hapus playlist ");
    if(max_playlist_idx <= 0)
        printf("(tambahkan playlist terlebih dahulu)");
    else if(playlist_idx < 0) 
        printf("(pilih playlist terlebih dahulu)");
    printf("\n");
    printf("4. Kembali ke menu utama\n");
    if(status_playlist == ADD)
    {
        pthread_mutex_lock(&lock);
        char nama[64];
        if(mutex == 1)
        {
            printf("Masukkan nama playlist: ");
            handler = fgets(nama, sizeof(nama), stdin);
            sprintf(nama_playlist[max_playlist_idx++], "%s", nama);
            status_playlist = DONE;
            pthread_mutex_unlock(&lock);
            pthread_cond_signal(&condition);
            mutex = 0;
        } else {
            pthread_mutex_unlock(&lock);
        }
    }
    else if(status_playlist == REMOVE)
    {
        remove_playlist(playlist_idx);
        status_playlist = DONE;
    }
}

void print_lagu()
{
    char abjad = 'A';
    for(int i = 0;i < PAGE; i++)
    {
        if( i + (current_page*PAGE) <=  max_idx)
            printf("%c. %s\n", abjad++, daftar_lagu[ (current_page * PAGE) + i]);
    }
}

void print_playlist()
{
    char abjad = 'A';
    for(int i = 0;i < max_playlist_idx; i++)
    {
        printf("%c. %s", abjad++, nama_playlist[i]);
    }
        
}

void remove_playlist(int idx)
{
    for(int i = idx;i < max_playlist_idx; i++)
        strcpy(nama_playlist[i], nama_playlist[i + 1]);
    max_playlist_idx--;
    playlist_idx = -1;
}

void *baca_perintah_keyboard(void *args)
{
    command = 0;
    while (command != 32)
    {
        pthread_mutex_lock(&lock);
        while(mutex == 1) pthread_cond_wait(&condition, &lock);
        pthread_mutex_unlock(&lock);
        command = getch();
        if(status_menu == MENU_UTAMA)
        {
            switch (command)
            {
            case '1': 
                status_menu = MENU_PLAY;
                break;
            case '2':
                status_menu = MENU_PLAYLIST;
                break;
            case '3':
                command = 32;
                break;
            default:
                break;
            }
        }
        else if(status_menu == MENU_PLAYLIST)
        {
            switch (command)
            {
            case '1': //Open playlist
                if(playlist_idx >= 0)
                    status_menu = MENU_LIST_LAGU;
                break;
            case '2': //Tambah playlist
                status_playlist = ADD;
                mutex = 1;
                break;
            case '3': //Hapus playlist
                if(playlist_idx >= 0)
                    status_playlist = REMOVE;
                break;
            case '4': //kembali ke menu utama
                status_menu = MENU_UTAMA;
            default:
                if('A' <= command && command < 'A' + max_playlist_idx)
                    playlist_idx = command - 'A';
                break;
            }
        }
        else if(status_menu == MENU_LIST_LAGU)
        {
            switch (command)
            {
            case '4':
                status_menu = MENU_UTAMA;
                break;
            default:
                break;
            }
        }
        else if(status_menu == MENU_PLAY)
        {
            switch (command)
            {
            case '1':
                if( (status_lagu != PLAY || status_lagu == PREV || status_lagu == NEXT) 
                    && current_idx >= 0)
                {
                    status_lagu = PLAY;
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
                now_playing = 0;
                break;
            case '4':
                if(current_idx > 0) {
                    current_idx--;
                    status_lagu = PREV;
                    now_playing = 0;
                }
                break;
            case '5':
                if(current_idx < sizeof(daftar_lagu)/MAXLEN)
                {
                    current_idx = (current_idx + 1) % (max_idx + 1);
                    status_lagu = NEXT;
                    now_playing = 0;
                } 
                break;
            case '6':
                if( (current_page*PAGE) >= PAGE )
                    current_page--;
                break;
            case '7':
                if( (current_page*PAGE + (max_idx%PAGE)) < max_idx )
                    current_page++;
                break;
            case '8':
                status_lagu = STOP;
                status_menu = MENU_UTAMA;
                break;
            default:
            {
                int abjad_maks;
                if( ((current_page+1)*PAGE) >= max_idx) 
                    abjad_maks = max_idx % PAGE;
                else abjad_maks = PAGE;

                if('A' <= command && command <= ('A' + abjad_maks ) 
                    && status_lagu != PLAY)
                    current_idx = (current_page*PAGE) + (command - 'A');
                break;
            }
                
            }
        }
    }
}

void *menu_music_player(void *args)
{
    static int handler;
    while(command != 32)
    {
        printf("==================MUSIC PLAYER==================\n");
        printf("================================================\n");
        switch (status_menu)
        {
        case MENU_UTAMA:
            current_idx = -1;
            playlist_idx = -1;
            menu_utama();
            break;
        case MENU_PLAY:
            menu_play();
            break;
        case MENU_LIST_LAGU:
            menu_lagu();
            break;
        case MENU_PLAYLIST:
            menu_playlist();
        default:
            break;
        }
        sleep(1);
        handler = system("clear");
    }
}

void *mainkan_musik(void *args)
{
    char judul_lagu[MAXLEN];
    char path_lagu[MAXLEN];
    strcpy(judul_lagu, (void *) args);
    sprintf(path_lagu, "%s/%s", path, judul_lagu);

    //Membuka file dan mendapatkan format lagunya
    mpg123_open(music_handler, path_lagu);
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
        case PLAY:
            ao_play(device, buffer, done);
            break;
        case PAUSE:
            while(status_lagu != PLAY) fflush(stdout);
            break;
        case STOP:
            playing = !MPG123_OK;
            break;
        case PREV:
            playing = !MPG123_OK;
            break;
        case NEXT:
            playing = !MPG123_OK;
            break;
        default:
            break;
        }
        
    } while (playing  == MPG123_OK);
    if(status_lagu == PLAY) {
        if(current_idx < sizeof(daftar_lagu)/MAXLEN)
        {
            current_idx = (current_idx + 1) % max_idx;
            status_lagu = NEXT;
            now_playing = 0;
        } 
    }
}

void list_lagu()
{
    static int index;
    static int len_lagu;
    char lagu[MAXLEN];
    char *ext = ".mp3";
    
    index = 0;
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
            sprintf(daftar_lagu[index++], "%s", lagu);
        }
    }
    max_idx = index - 1;
    closedir(directory);
}

//=====================
//--Inisialisasi mp3---
//=====================
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

//=====================
//------Close mp3------
//=====================
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