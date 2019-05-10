#include "lagu.h"

int main(int argc, char *argv[])
{
    if(argc < 2)
        exit(0);

    inisialisasi_lagu();
    baca_lagu(argv[1]);
    play_lagu();
    tutup_lagu();

    return 0;
}

void inisialisasi_lagu()
{
    int err;
    ao_initialize();
    driver = ao_default_driver_id();
    mpg123_init();
    music_handler = mpg123_new(NULL, &err);
    buffer_size = mpg123_outblock(music_handler);
    buffer = (unsigned char*) malloc(buffer_size * sizeof(unsigned char));
}

void baca_lagu(char* nama_file)
{
    /* open the file and get the decoding format */
    mpg123_open(music_handler, nama_file);
    mpg123_getformat(music_handler, &rate, &channels, &encoding);

    /* set the output format and open the output device */
    format.bits = mpg123_encsize(encoding) * BITS;
    format.rate = rate;
    format.channels = channels;
    format.byte_format = AO_FMT_NATIVE;
    format.matrix = 0;
    device = ao_open_live(driver, &format, NULL);
}

void play_lagu()
{
    while (mpg123_read(music_handler, buffer, buffer_size, &done) == MPG123_OK)
    {
        ao_play(device, buffer, done);
    }
}

void tutup_lagu()
{
    free(buffer);
    ao_close(device);
    mpg123_close(music_handler);
    mpg123_delete(music_handler);
    mpg123_exit();
    ao_shutdown();
}