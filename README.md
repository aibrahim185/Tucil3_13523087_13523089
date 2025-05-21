<h1 align="center">Tugas Kecil 3 IF2211 Strategi Algoritma</h1>
<h1 align="center">➡️ Penyelesaian Puzzle Rush Hour Menggunakan Algoritma Pathfinding ⬅️</h1>

![alt text](<img/image.png>)

## Daftar Isi
1. [Informasi Umum](#informasi-umum)
2. [Kontributor](#kontributor)
3. [Fitur Utama](#fitur-utama)
4. [Algoritma yang Diimplementasikan](#algoritma-yang-diimplementasikan)
5. [Prasyarat Program](#prasyarat-program)
6. [Cara Menjalankan Program](#cara-menjalankan-program)
7. [Struktur Proyek](#struktur-proyek)
8. [Status Proyek](#status-proyek)

## Informasi Umum
Proyek ini bertujuan untuk mengimplementasikan dan menganalisis berbagai algoritma pathfinding dalam menyelesaikan puzzle klasik Rush Hour. Rush Hour adalah permainan puzzle logika berbasis grid dengan pemain harus menggeser kendaraan di dalam area terbatas agar mobil utama (target) dapat keluar melalui pintu keluar yang telah ditentukan. Tantangannya adalah mencapai tujuan ini dengan jumlah langkah seminimal mungkin.

Program ini dikembangkan menggunakan Godot Engine dengan logika inti algoritma pencarian yang diimplementasikan dalam C++ sebagai GDExtension. Pengguna dapat memuat konfigurasi puzzle dari file teks dan memilih algoritma pencarian yang diinginkan untuk menemukan solusi. Program akan menampilkan hasil pencarian, termasuk jumlah langkah, waktu eksekusi, dan visualisasi langkah-langkah solusi.

## Kontributor
| NIM      | Nama                 |
| :------- | :------------------- |
| 13523087 | Grace Evelyn Simon   |
| 13523089 | Ahmad Ibrahim        |

## Fitur Utama
* **Visualisasi Permainan Rush Hour**: Papan permainan dan gerakan mobil divisualisasikan menggunakan Godot Engine.
* **Input Konfigurasi**: Membaca konfigurasi awal papan dan bidak dari file `.txt`.
* **Pemilihan Algoritma**: Pengguna dapat memilih algoritma pathfinding yang akan digunakan untuk menyelesaikan puzzle.
* **Output Hasil Pencarian**: Menampilkan jumlah langkah dalam solusi, waktu eksekusi, dan urutan konfigurasi papan per gerakan.
* **Animasi Solusi (Bonus)**: Menampilkan animasi gerakan mobil untuk mencapai solusi.

## Algoritma yang Diimplementasikan
Program ini mengimplementasikan beberapa algoritma pathfinding untuk menyelesaikan puzzle Rush Hour:
1.  **Uniform Cost Search (UCS)**: Menemukan solusi dengan biaya total terendah dari state awal. Dalam Rush Hour dengan biaya langkah seragam, ini akan menemukan solusi dengan jumlah gerakan paling sedikit.
2.  **Greedy Best-First Search (Greedy BFS)**: Menggunakan fungsi heuristik untuk memandu pencarian secara "rakus" ke state yang tampak paling dekat dengan tujuan. Tidak menjamin solusi optimal.
3.  **A\* (A-Star) Search**: Menggabungkan biaya aktual dari state awal (`g(n)`) dengan estimasi biaya heuristik ke tujuan (`h(n)`) untuk memandu pencarian.
4.  **Iterative Deepening Search (IDS)**: Menggabungkan keunggulan DFS dalam penggunaan memori dengan kelengkapan dan optimalitas BFS (untuk biaya langkah seragam) dengan melakukan serangkaian DLS dengan batas kedalaman yang meningkat.

## Prasyarat Program
Sebelum menjalankan program, pastikan perangkat lunak berikut telah terinstal:

* **Godot Engine**: Versi 4.4 atau lebih baru (disarankan versi stabil terbaru).
* **Kompilator C++**: Kompilator C++ yang kompatibel (misalnya, GCC/MinGW untuk Windows, Clang untuk macOS, GCC untuk Linux).
* **Python**: Diperlukan untuk menjalankan SCons.
* **SCons**: Sistem build yang digunakan untuk mengkompilasi GDExtension C++. Dapat diinstal menggunakan pip:
    ```bash
    pip install scons
    ```
    Atau melalui manajer paket sistem (misalnya, `sudo apt install scons` di Debian/Ubuntu).

## Cara Menjalankan Program
### 1. Kloning Repositori
```bash
git clone https://github.com/aibrahim185/Tucil3_13523087_13523089.git
```

### 2. Kompilasi GDExtension C++
Logika inti algoritma pencarian berada dalam GDExtension C++. Kompilasi bagian ini menggunakan SCons. Buka terminal atau command prompt di direktori root proyek (yang berisi file `SConstruct`).
Jalankan salah satu perintah berikut:

**Opsi 1**
```bash
scons
```

**Opsi 2**
```bash
python -m scons
```

### 3. Impor dan Jalankan Proyek di Godot Engine
* Buka aplikasi Godot Engine.
* Pada Manajer Proyek Godot, pilih tombol "Impor".
* Arahkan ke file `project.godot` yang berada di dalam direktori utama proyek Godot (`Tucil3_13523087_13523089/tukang-parkir-simulator/project.godot`). Klik "Buka".
* Klik "Impor & Edit".
* Setelah proyek terbuka di editor Godot, Anda dapat menjalankan game dengan menekan tombol "Play" di kanan atas (atau menggunakan shortcut F5).

### 4. Menggunakan Program
* Setelah program berjalan, akan disajikan dengan antarmuka pengguna (GUI).
* Fitur yang tersedia mencakup:
    * Tombol untuk **memuat file test case** (format `.txt`) yang berisi konfigurasi puzzle Rush Hour.
    * Pilihan untuk **memilih algoritma pencarian** yang ingin digunakan (UCS, Greedy BFS, A\*, IDS).
* Setelah memuat test case dan memilih algoritma (serta heuristik jika ada), klik tombol "Solve".
* Program akan memproses dan kemudian menampilkan hasil pencarian:
    * Visualisasi papan permainan.
    * Animasi langkah-langkah solusi.
    * Informasi seperti jumlah langkah dalam solusi, waktu eksekusi, dan jumlah node yang dieksplorasi.

## Struktur Proyek
```
.
├── LICENSE.md
├── README.md
├── SConstruct
├── __pycache__
│   └── methods.cpython-311.pyc
├── bin
│   ├── android
│   ├── linux
│   ├── macos
│   ├── web
│   └── windows
├── build_profile.json
├── godot-cpp
│   ├── CMakeLists.txt
│   ├── LICENSE.md
│   ├── Makefile
│   ├── README.md
│   ├── SConstruct
│   ├── __pycache__
│   ├── bin
│   ├── binding_generator.py
│   ├── build_profile.py
│   ├── cmake
│   ├── doc
│   ├── gdextension
│   ├── gen
│   ├── include
│   ├── misc
│   ├── pyproject.toml
│   ├── src
│   ├── test
│   └── tools
├── image-1.png
├── image.png
├── img
│   └── image.png
├── methods.py
├── src
│   ├── gen
│   ├── main_scene
│   ├── register_types.cpp
│   ├── register_types.h
│   ├── register_types.o
│   ├── search
│   └── utils
├── test
│   ├── 1.txt
│   ├── 2.txt
│   ├── 3.txt
│   ├── 4.txt
│   └── 5.txt
└── tukang-parkir-simulator
    ├── bin
    ├── export_presets.cfg
    ├── icon.svg
    ├── icon.svg.import
    ├── project.godot
    └── scenes
```

## Status Proyek
Proyek ini telah selesai dengan bonus yang diterapkan berupa algoritma tambahan dan GUI.