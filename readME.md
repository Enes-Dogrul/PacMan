Pac-Man
Bu proje, klasik Pac-Man oyununun C++ ve SFML kütüphanesi ile yapılmış bir klonudur.
Programlama II dersi için geliştirdim.
Oyuncu labirentte noktaları toplar, hayaletler onu kovalar. Güç meyvesi yenince
hayaletler bir süre kaçar ve bu sırada yenebilir.
Kullandığım Teknolojiler

C++ (C++17)
SFML 3.0.1 (grafik, pencere, ses)

Özellikler

WASD ile Pac-Man hareketi
4 hayalet ve farklı davranışları (Blinky, Pinky, Inky, Clyde)
Hayaletler en kısa yolu BFS algoritması ile buluyor
Güç meyvesi yenince hayaletler kaçış moduna geçiyor
Tüm noktalar bitince yeni seviye başlıyor
Can, puan ve yüksek skor gösterimi (yüksek skor dosyada saklanıyor)
Tünel (bir kenardan çıkıp diğer kenardan girme)

Klasör Yapısı
PACMAN/
  main.cpp
  textures/pacman/   -> görseller (.png)
  resources/         -> sesler (.wav)
  src/               -> arial.ttf ve highscore.txt
Derleme ve Çalıştırma
Dosya yolları proje klasörüne göre olduğu için oyunu PACMAN klasörünün
içinden derleyip çalıştırmak gerekiyor:
cd PACMAN
clang++ -std=c++17 main.cpp -o pacman -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio
./pacman
(Linux'ta clang++ yerine g++ de kullanılabilir.)
Not: Oyunu PACMAN klasörünün dışından çalıştırırsan dosyaları bulamaz.
IDE kullanıyorsan çalışma dizinini (working directory) PACMAN olarak ayarla.
Kontroller

W: yukarı
A: sol
S: aşağı
D: sağ
ESC: çıkış

Puanlama

Nokta: 1 puan
Güç meyvesi: 50 puan
Kaçan hayaleti yeme: 200, 400, 800, 1600 puan

Oyuncu 3 canla başlar. Canlar bitince oyun biter ve skor kaydedilir.