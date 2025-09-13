# RFD-ENCRYPT: Güvenli RF İletişim Kütüphanesi

Bu proje, Pico (RP2040) gibi gömülü sistemler için tasarlanmış, paket tabanlı ve güvenli bir RF (Radyo Frekansı) iletişim protokolü kütüphanesidir. Kütüphane, `rfd` ve `rsa` olmak üzere iki ana modülden oluşur.

## RFD Kütüphanesi Mimarisi

`rfd` kütüphanesi, UART arayüzü üzerinden paket tabanlı bir iletişim sağlar. Temel amacı, farklı cihazların (node) bir ağ üzerinde birbirleriyle güvenli bir şekilde haberleşmesini sağlamaktır.

### Temel Bileşenler

- **`RFD` Sınıfı**: Kütüphanenin ana yönetim sınıfıdır. Her bir RFD cihazını temsil eder.
  - **Node (Düğüm) Yönetimi**: Ağdaki diğer cihazların genel anahtarlarını (`public key`) ve kimliklerini (`ID`) kaydeder.
  - **Veri İşleme**: Gelen UART verilerini işleyerek tam ve geçerli paketleri ayrıştırır.
  - **Paket Gönderimi**: `HELLO`, `PING`, `MESSAGE` gibi farklı türde paketlerin gönderilmesini sağlar.

- **`Packet` ve Türetilmiş Sınıflar**: Protokolün temelini oluşturan veri yapılarıdır.
  - **Soyut `Packet` Sınıfı**: Tüm paket türleri için ortak bir arayüz tanımlar (`serialize`, `deserialize`, `handle`).
  - **Paket Başlığı (`PacketHeader`)**: Her paketin başında bulunan ve paketin bütünlüğünü, türünü ve boyutunu belirten standart bir yapıdır. `0xAA` ve `0x55` sihirli byte'ları ile başlar.
  - **Spesifik Paket Sınıfları**:
    - `HelloPacket`: Bir cihazın ağa katıldığını bildirmek ve genel anahtarını paylaşmak için kullanılır.
    - `SignedMessagePacket`: Belirli bir hedefe şifrelenmiş ve imzalanmış mesaj göndermek için kullanılır.
    - `PingPacket`: Bir cihazın ağda aktif olup olmadığını kontrol etmek için kullanılır.
    - `AckPacket`: Alınan bir pakete onay (acknowledgement) göndermek için kullanılır.

### İletişim Akışı

1.  **Başlatma**: Her `RFD` nesnesi, kendine özgü bir `ID` ile başlatılır ve belirtilen UART pinleri üzerinden iletişime hazır hale gelir.
2.  **Ağa Katılma**: Bir cihaz, ağdaki diğer cihazlar tarafından tanınmak için periyodik olarak `HelloPacket` gönderir.
3.  **Paket Alma**: `RFD` sınıfı, UART'tan gelen byte akışını sürekli olarak dinler. Gelen byte'ları bir durum makinesi (state machine) kullanarak işler:
    - Sihirli byte'ları (`0xAA 55`) arar.
    - Paket başlığını okur.
    - Başlıkta belirtilen uzunluk kadar paket içeriğini (payload) okur.
    - Tam bir paket oluştuğunda, bu paketi işlenmek üzere ilgili `handle` fonksiyonuna yönlendirir.
4.  **Paket İşleme**: Gelen her paket, türüne göre (`HelloPacket`, `PingPacket` vb.) kendi `handle` metodu içinde işlenir. Örneğin, bir `HelloPacket` alındığında, gönderenin kimliği ve genel anahtarı ağ listesine kaydedilir.

## Proje Yapısı

```
rfd-encrypted/
├── src/                 # Ana uygulama kodları
│   └── main.cpp        # Ana uygulama dosyası
├── math/               # Matematik işlemleri kütüphanesi
│   ├── headers/
│   │   ├── FastMath.h          # Hızlı matematik fonksiyonları
│   │   ├── PrimeGenerator.h    # Asal sayı üretici
│   │   └── config.h            # Temel veri tipleri tanımları
│   ├── src/
│   │   ├── FastMath.cpp        # Modüler aritmetik işlemleri
│   │   └── PrimeGenerator.cpp  # Eratosthenes eleme algoritması
│   └── CMakeLists.txt
├── rsa/                # RSA şifreleme kütüphanesi
│   ├── headers/
│   │   ├── RSA.h              # RSA ana sınıfı
│   │   ├── Key.h              # Anahtar sınıfı (public/private)
│   │   └── config.h           # RSA veri tipleri
│   ├── src/
│   │   ├── RSA.cpp            # RSA implementasyonu
│   │   └── Key.cpp            # Anahtar işlemleri
│   └── CMakeLists.txt
├── rfd/                # RFD iletişim protokolü kütüphanesi
│   ├── core/
│   │   ├── headers/
│   │   │   ├── RFD.h          # Ana RFD yönetim sınıfı
│   │   │   └── Node.h         # Ağ düğümü sınıfı
│   │   ├── RFD.cpp
│   │   └── Node.cpp
│   ├── packets/
│   │   ├── headers/
│   │   │   ├── Packet.h       # Soyut paket sınıfı
│   │   │   ├── HelloPacket.h  # Ağa katılma paketi
│   │   │   ├── PingPacket.h   # Durum kontrolü paketi
│   │   │   ├── AckPacket.h    # Onay paketi
│   │   │   └── SignedMessagePacket.h # Şifreli mesaj paketi
│   │   ├── Packet.cpp
│   │   ├── HelloPacket.cpp
│   │   ├── PingPacket.cpp
│   │   ├── AckPacket.cpp
│   │   └── SignedMessagePacket.cpp
│   └── CMakeLists.txt
├── build/              # Derleme çıktıları
├── test_x86.cpp       # PC üzerinde test dosyası
└── CMakeLists.txt     # Ana proje build dosyası
```

## Kullanım

### Derleme
```bash
mkdir build && cd build
cmake ..
ninja  # veya make
```

### PC Test Çalıştırma
```bash
# PC üzerinde test için
g++ test_x86.cpp rsa/src/RSA.cpp rsa/src/Key.cpp math/src/FastMath.cpp math/src/PrimeGenerator.cpp -I rsa/headers -I math/headers -g -o test
./test
```

### Pico Çalıştırma
```bash
# Pico için derleme
ninja
# Flash işlemi (picotool ile)
picotool load rfd-encrypt.uf2 -fx
```

## Kütüphane Yapısı

### Math Kütüphanesi
Matematik işlemleri için ayrılmış modüler bir kütüphane:
- **FastMath**: Modüler aritmetik, hızlı üs alma, GCD ve hash işlemleri
- **PrimeGenerator**: Eratosthenes eleme algoritması ile asal sayı üretimi
- **config.h**: Temel veri tiplerinin tanımlanması (`rsa_data`)

### RSA Kütüphanesi
RSA şifreleme algoritmasının implementasyonu:
- **RSA Sınıfı**: Anahtar üretimi ve şifreleme/deşifreleme işlemleri
- **Key Sınıfı**: Public/private anahtar yönetimi ve imzalama işlemleri
- Math kütüphanesine bağımlı

### RFD Kütüphanesi
RF iletişim protokolünün implementasyonu:
- **Core**: RFD ana yönetim sınıfı ve Node (düğüm) yönetimi
- **Packets**: Farklı paket türleri ve bunların işlenmesi
- RSA ve Math kütüphanelerine bağımlı

## RSA Şifreleme Özellikleri

- **Anahtar Üretimi**: Dinamik asal sayı üretimi
- **Şifreleme/Deşifreleme**: Byte-level şifreleme desteği
- **64-bit Aritmetik**: `__uint64_t` türü ile büyük sayı işlemleri

# Kullandığım Kaynaklar
- [Modüler Aritmetik](https://www.derspresso.com.tr/matematik/moduler-aritmetik)
- [Adım Adım RSA](https://www.cryptool.org/en/cto/rsa-step-by-step/)
- [RSA Wikipedia](https://en.wikipedia.org/wiki/RSA_cryptosystem#Operation)