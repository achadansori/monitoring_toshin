# Sistem Monitoring IoT Toshin Prima Fine Blanking

Proyek ini adalah sistem monitoring IoT untuk Toshin Prima Fine Blanking. Sistem ini mencakup pemantauan suhu untuk chiller dan pemantauan status untuk mesin.

## Deskripsi Proyek

Sistem monitoring ini terdiri dari dua bagian utama:
1. Node sensor berbasis ESP32 yang mengumpulkan data suhu chiller dan status mesin.
2. Aplikasi web yang menampilkan data dalam bentuk dashboard dan tabel.

## Fitur Utama

- Monitoring suhu real-time untuk dua chiller
- Monitoring status on/off untuk dua mesin
- Dashboard interaktif dengan indikator visual
- Penyimpanan data historis
- Fitur filter data berdasarkan tanggal
- Fitur penghapusan data

## Node

- Node Chiller berada di Mesin Chiller untuk mengetahui suhu
- Node Monitor berada di Bengkel untuk mengetahui warning ketika suhu tidak aman
- Node Web berada di alamat 10.100.4.180/monitoring_toshin untuk mengetahui dashboard dan riwayat di database

## Komponen Hardware

- ESP32 development board
- Sensor suhu (misalnya DS18B20) untuk chiller
- Sensor atau switch untuk status mesin
- LCD I2C 20x4
- Buzzer
- Kabel jumper

## Teknologi yang Digunakan

- ESP32 (Arduino framework)
- PHP
- MySQL
- HTML/CSS
- JavaScript (jQuery)

## Panduan Pemasangan

### Persiapan File

- Download dengan klik code lalu download zip
- unzip folder

### Persiapan Hardware

1. Hubungkan sensor suhu ke pin GPIO ESP32 (misalnya pin 4 dan 5)
2. Hubungkan sensor atau switch status mesin ke pin GPIO ESP32 (misalnya pin 12 dan 13)
3. Hubungkan LCD I2C ke pin SDA dan SCL ESP32
4. Hubungkan buzzer ke pin GPIO ESP32 (misalnya pin 5)

### Persiapan Software ESP32

1. Install Arduino IDE
2. Tambahkan dukungan board ESP32 di Arduino IDE
3. Install library yang diperlukan:
   - WiFi.h
   - LiquidCrystal_I2C.h
   - OneWire.h (jika menggunakan sensor DS18B20)
   - DallasTemperature.h (jika menggunakan sensor DS18B20)

### Program ESP32 Node Chiller

1. Buka Arduino IDE
2. Buka file `node_chiller.ino`
3. Sesuaikan konfigurasi WiFi dan IP statis jika diperlukan
4. Upload program ke ESP32 di Chiller

### Program ESP32 Node Monitor

1. Buka Arduino IDE
2. Buka file `node_monitor.ino`
3. Sesuaikan konfigurasi WiFi dan IP statis jika diperlukan
4. Upload program ke ESP32 di Monitor

### Persiapan Web Server

1. Install XAMPP
2. Start Apache dan MySQL dari XAMPP Control Panel
3. Terhubung ke Wifi TPF-Office untuk wifi tanyakan ke pihak terkait

### Persiapan Database

1. Buka phpMyAdmin (http://10.100.4.180/phpmyadmin)
2. Buat database baru dengan nama `monitoring_toshin`
3. Import file `node_chiller.sql` dan `node_mesin.sql`

### Pemasangan File Web

1. Buka folder htdocs di direktori instalasi XAMPP
2. Buat folder baru dengan nama `monitoring_toshin`
3. Copy semua file PHP dan HTML ke folder `monitoring_toshin`

### Konfigurasi Koneksi Database

1. Buka file `connect.php`
2. Sesuaikan kredensial database jika diperlukan

## Penggunaan

1. Pastikan ESP32 sudah terhubung ke jaringan WiFi
2. Buka browser dan akses http://10.100.4.180/monitoring_toshin
3. Dashboard monitoring akan menampilkan data suhu chiller dan status mesin secara real-time
4. Gunakan menu di sidebar untuk beralih antara Dashboard, Tabel Chiller, dan Tabel Mesin
5. Di halaman tabel, gunakan fitur filter tanggal untuk melihat data historis
6. Gunakan tombol Delete untuk menghapus data yang dipilih atau semua data

## Troubleshooting

- Jika data tidak muncul di dashboard, periksa koneksi WiFi ESP32 dan pastikan URL pengiriman data sudah benar
- Jika terjadi error database, periksa kredensial database di file PHP
- Jika LCD tidak menampilkan data, periksa koneksi I2C dan alamat LCD

## Kontribusi

Proyek ini dikembangkan oleh Elka x Meka PENS 21 untuk Toshin Prima Fine Blanking. Kontribusi dan saran perbaikan sangat diterima.

## Contact Developer

https://achadansori.ee.student.pens.ac.id/

## Lisensi

[MIT License](https://opensource.org/licenses/MIT)
