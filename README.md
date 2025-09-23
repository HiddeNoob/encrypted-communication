# RFD-ENCRYPT: Eğitim Amaçlı RF Mesh İletişim Sistemi

Bu proje, Raspberry Pi Pico (RP2040) tabanlı mesh ağ iletişim sistemidir. RSA şifreleme konseptlerini öğrenmek için tasarlanmış ve paket tabanlı protokol ile node'lar arası mesajlaşma imkanı sunar.

## ⚠️ ÖNEMLİ GÜVENLİK UYARISI

**Bu proje yalnızca eğitim ve öğrenme amaçlı geliştirilmiştir!**

- RSA şifreleme 16-32 bit gibi çok küçük anahtar boyutları kullanır
- Rastgele sayı üretimi deterministik ve kriptografik güvenli değildir
- Gerçek güvenlik uygulamalarında minimum 2048 bit anahtar kullanılmalıdır
- Bu implementasyon gerçek dünya güvenlik tehditlerine karşı güvenli değildir
- Üretim ortamında kesinlikle kullanılmamalıdır
- Kriptografi konseptlerini öğrenmek için tasarlanmıştır

**Gerçek kriptografik güvenlik için kanıtlanmış kütüphaneler kullanın!**


## 🌐 Sistem Genel Bakış

Sistem, birbirleriyle iletişim kurabilen otonom node'lardan oluşur. Her node kendi RSA anahtar çiftini oluşturur ve ağdaki diğer node'larla şifreli mesaj alışverişi yapar. UART tabanlı RF modüller ile fiziksel iletişim sağlanır.

### Test Yapısı (Multi-Core)
Mevcut implementasyonda test etmek için:
- **Core 0**: Node1 (UART0, GPIO 0-1) - Kullanıcı arayüzü ile kontrol edilir
- **Core 1**: Node2 (UART1, GPIO 4-5) - Arka planda otomatik çalışır
- Her iki core sürekli olarak kendi UART'larından gelen paketleri işler
- Node1 menü sisteminden Hello gönderebilir, Node2 otomatik yanıt verir

## 📡 Haberleşme Akışı

### 1. Sistem Başlatma
- Her node benzersiz 64-bit ID ile başlatılır
- RSA anahtar çifti (public/private) oluşturulur
- UART pinleri yapılandırılır
- PacketHandler ile veri işleme başlatılır

### 2. Ağ Keşfi (Network Discovery)
- Node, Hello paketi yayınlar (broadcast)
- Hello paketi şunları içerir:
  - Node ID
  - Public Key
  - RSA Modulus
- Diğer node'lar Hello paketini alır ve gönderen node'u ağ listesine kaydeder

### 3. Node Kaydı ve Anahtar Değişimi
- Alınan Hello paketine KeyPacket ile yanıt verilir
- Karşılıklı public key değişimi tamamlanır
- Her node diğer node'ların public key'lerini saklar

### 4. Ping ve Durum Kontrolü
- Node'lar birbirlerine ping paketi gönderebilir
- Ping alan node AckPacket ile yanıt verir
- Bu sayede ağdaki aktif node'lar tespit edilir

### 5. Şifreli Mesajlaşma
- Mesaj gönderen node, hedef node'un public key'ini kullanır
- Mesaj byte-by-byte RSA ile şifrelenir (hedef node'un public key ile)
- Mesajın hash'i gönderen node'un private key ile imzalanır
- SignedMessagePacket olarak gönderilir
- Alıcı node mesajı kendi private key ile deşifreler
- Alıcı node mesajın hash'ini hesaplar ve gönderen node'un public key ile imzayı doğrular
- Hash'ler eşleşirse mesaj okunur, aksi halde reddedilir

## 🏗️ Proje Mimarisi

```
rfd-encrypt/
├── src/                    # Ana uygulama
│   └── main.cpp           # Kullanıcı arayüzü ve sistem kontrolü
├── math/                  # Matematiksel işlemler
│   ├── headers/
│   │   ├── FastMath.h     # Modüler aritmetik, GCD, üs alma
│   │   └── PrimeGenerator.h # Asal sayı üretimi (Eratosthenes)
│   └── src/
├── rsa/                   # RSA kriptografi
│   ├── headers/
│   │   ├── RSA.h          # Ana RSA sınıfı
│   │   └── Key.h          # Public/Private key yönetimi
│   └── src/
├── comm/                  # İletişim katmanı
│   ├── headers/
│   │   ├── IComm.h        # İletişim arayüzü
│   │   └── UART.h         # UART implementasyonu
│   └── src/
└── rfd/                   # RF protokol katmanı
    ├── core/              # Ana sistem bileşenleri
    │   ├── headers/
    │   │   ├── RFD.h      # Ana protokol yönetimi
    │   │   ├── Node.h     # Yerel node yönetimi
    │   │   └── RemoteNode.h # Uzak node temsili
    │   └── src/
    └── packets/           # Paket sistemi
        ├── headers/
        │   ├── Packet.h        # Soyut paket sınıfı
        │   ├── PacketHeader.h  # Paket başlık yapısı
        │   ├── PacketHandler.h # Paket işleme motoru
        │   ├── HelloPacket.h   # Ağ keşif paketi
        │   ├── PingPacket.h    # Durum kontrol paketi
        │   ├── SignedMessagePacket.h # Şifreli mesaj paketi
        │   └── AckPacket.h     # Onay paketi
        └── src/
```

## 🔧 Kütüphane Detayları

### Math Kütüphanesi
**Görev**: RSA için gerekli matematiksel işlemler

**FastMath.h/cpp**:
- Modüler aritmetik işlemler
- Hızlı modüler üs alma (RSA şifreleme/deşifreleme için)
- Genişletilmiş Öklid algoritması (anahtar üretimi için)
- Euler totient fonksiyonu
- Hash algoritması (dijital imza için)

**PrimeGenerator.h/cpp**:
- Eratosthenes eleme algoritması ile asal sayı üretimi
- RSA anahtar üretimi için basit asal sayı seçimi (eğitim amaçlı)
- Asal sayı doğrulama

### RSA Kütüphanesi
**Görev**: Eğitim amaçlı kriptografi implementasyonu

**RSA.h/cpp**:
- RSA anahtar çifti üretimi (küçük anahtar boyutu)
- Anahtar boyutu yapılandırması
- Public/Private key yönetimi

**Key.h/cpp**:
- RSA anahtarları ile şifreleme/deşifreleme
- Dijital imzalama
- Byte-level veri işleme

### COMM Kütüphanesi
**Görev**: Donanım seviyesi iletişim soyutlaması

**IComm.h**:
- İletişim katmanı için soyut arayüz
- Farklı iletişim protokolleri için genişletilebilir

**UART.h/cpp**:
- Pico UART donanımı ile entegrasyon
- Yapılandırılabilir baudrate ve pin ataması

### RFD Kütüphanesi
**Görev**: RF protokol ve ağ yönetimi

#### Core Bileşenleri

**RFD.h/cpp**:
- Ana protokol yönetim sınıfı
- UART üzerinden veri işleme
- Paket gönderim fonksiyonları (Hello, Ping, Message)
- RSA servisi entegrasyonu

**Node.h/cpp**:
- Yerel node'un ağ yönetimi
- Uzak node'ların kaydı ve yönetimi
- Public key saklama ve erişim

**RemoteNode.h/cpp**:
- Ağdaki diğer node'ların yerel temsili
- Node ID ve public key saklama

#### Paket Sistemi

**PacketHeader.h**:
- Tüm paketlerin standart başlık yapısı
- Magic bytes (0xAA55) ile paket senkronizasyonu
- Paket tipi ve uzunluk bilgisi

**PacketHandler.h/cpp**:
- UART'dan gelen byte stream'ini paketlere çevirme
- State machine ile paket ayrıştırma
- Buffer yönetimi ve garbage collection
- Incomplete paket timeout yönetimi

**Packet.h/cpp**:
- Tüm paket türleri için soyut sınıf
- Serialize/deserialize işlemleri
- Factory pattern ile paket oluşturma

**Spesifik Paket Türleri**:
- **HelloPacket**: Ağ keşfi ve public key paylaşımı
- **PingPacket**: Node durum kontrolü
- **SignedMessagePacket**: Şifreli ve imzalı mesaj
- **AckPacket**: Paket alındı bildirimi
- **KeyPacket**: Anahtar değişimi

## 🔄 Veri İşleme Akışı

### Gelen Veri İşleme
1. **UART Buffer**: UART'dan gelen bytes PacketHandler buffer'ına alınır
2. **Magic Byte Detection**: 0xAA55 patern'i aranır
3. **Header Parsing**: Paket tipi ve uzunluk okunur
4. **Completeness Check**: Tüm paket geldi mi kontrol edilir
5. **Packet Extraction**: Complete paket buffer'dan çıkarılır
6. **Deserialization**: Binary data paket objesine çevrilir
7. **Handler Dispatch**: Paket tipine göre ilgili handler çağrılır

### Giden Veri İşleme
1. **Packet Creation**: İlgili paket objesi oluşturulur
2. **Serialization**: Paket binary data'ya çevrilir
3. **Header Addition**: Magic bytes ve header eklenir
4. **Chunked Transmission**: Veri parçalar halinde UART'a gönderilir
5. **Flow Control**: Her chunk arasında delay eklenir

## 🚀 Kurulum ve Kullanım

### Derleme
```bash
mkdir build && cd build
cmake ..
make
```

### Kullanıcı Arayüzü (Node1 - Core 0)
- **1**: Ağ taraması (Hello paketi gönder)
- **2**: Bağlı node'ları listele ve işlem yap
  - **1-9**: Node seç
  - **p**: Ping gönder
  - **m**: Mesaj gönder
- **0**: Çıkış

### Multi-Core Test Yapısı
- **Core 0 (Node1)**: Menü sistemi ile etkileşimli kontrol
- **Core 1 (Node2)**: Arka planda sürekli paket dinleme ve otomatik yanıt
- Her iki node kendi UART pinlerinden sürekli veri işler
- Test için Node1'den Hello gönderildiğinde Node2 otomatik KeyPacket ile yanıt verir


## 🔒 Kriptografi Özellikleri 


- **End-to-End Encryption**: RSA ile mesaj şifreleme (16-32 bit)
- **Digital Signatures**: Mesaj bütünlüğü doğrulama 
- **Public Key Infrastructure**: Basit anahtar dağıtımı 
- **Node Authentication**: Public key ile kimlik doğrulama 
- **Random Number Generation**: Deterministik rastgelelik (güvensiz)

**Gerçek üretim ortamları için minimum 2048-bit RSA anahtarları ve kriptografik güvenli rastgele sayılar kullanılmalıdır!**

## 📚 Kullanılan Kaynaklar
- [RSA Algoritması](https://www.cryptool.org/en/cto/rsa-step-by-step/)
- [Modüler Aritmetik](https://www.derspresso.com.tr/matematik/moduler-aritmetik)
- [RSA Cryptosystem](https://en.wikipedia.org/wiki/RSA_cryptosystem)