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

#### Penjelasan MP3 Player
