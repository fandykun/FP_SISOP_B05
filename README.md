# Final Project Sistem Operasi 2019

## FUSE
#### Persyaratan

- Build-Essential / gcc
- Libfuse
```shell
$ sudo apt-get update
$ sudo apt-get install build-essential
$ sudo apt-get install libfuse* -y
```
### [Source code](https://github.com/fandykun/FP_SISOP_B05/blob/master/mp3_fuse.c)
#### NB: Program sudah saya buat agar mudah dibaca, penjelasan dibawah berupa penjelasan singkat.

#### Penjelasan Fuse
- `.init`
  - Digunakan untuk menginisialisasi, dalam hal ini dibuat sebuah **thread** untuk melakukan query untuk mendapatkan semua file yang berekstensi `.mp3` dan di **copy** satu direktori *temporary* untuk dikumpulkan 
  - Dilakukan sebuah fungsi rekursi untuk file didalam folder dan subfolder dengan *basecase* ketika nama *path* tersebut sudah bukan termasuk direktori lagi berdasarkan kondisi *statnya*.
- `.getattr`, `.readdir`, `.read`
  - Dalam hal ini, fungsi yang dilakukan didalamnya sama halnya dengan program contoh fuse seperti sebelumnya, bedanya ia melakukan `mount` dari hasil direktori yang dilakukan di `.init` tadi.
- `.destroy`
  - Karena `mount` tersebut didapatkan dari direktori yang mengcopy semua file yang berekstensi `.mp3` di init tadi, maka direktori *temporary* tersebut perlu dihapus.

## MP3 Player using Thread
#### Persyaratan
- Build-Essential / gcc
- libmpg123
- Libao
```shell
$ sudo apt-get update
$ sudo apt-get install build-essential
$ sudo apt-get install libmpg123-dev
$ sudo apt-get install libao-dev
```

### [Source code](https://github.com/fandykun/FP_SISOP_B05/blob/master/lagu.c)
#### Penjelasan MP3 Player
- Digunakan dua buah thread utama:
  - Thread 1 untuk menghandle keyboard / inputan (keypress).
  - Thread 2 untuk menu mp3player nya.
    - Ketika dalam kondisi **play**, dibuat thread baru untuk menghandle lagu tersebut, dan thread akan berhenti ketika lagu selesai atau setiap lagu saat itu tersebut berhenti / berganti.
- List semua lagu didapatkan dari path yang mengarah ke hasil **fuse**.
- Digunakan mutex untuk me-lock thread 1 ketika menambahkan playlist (butuh inputan nama playlist).
- Untuk setiap playlist yang menyimpan daftar lagu, menggunakan konsep *one-to-many* dimana setiap playlist menyimpan id dari lagu2 tersebut.
- Untuk memutar sebuah lagu:
  - memanfaatkan library mpg123 untuk mendapatkan format filenya (mp3) dan mendapatkan lagunya.
  - Memanfaatkan library libao untuk memutar lagu dengan mengeplay setiap buffer yang diterima dari hasil membaca file dengan mpg123.