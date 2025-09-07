# RFD 868x ile Şifreli Haberleşme

Bu proje, RFD (Radio Frequency Data) modülleri ile RSA şifreleme kullanarak güvenli kablosuz haberleşme sağlamak için geliştirilmiştir.

## Proje Özellikleri

- **RSA Şifreleme**
    - **Prime Generator**: Sieve of Eratosthenes algoritması ile asal sayı üretimi
    - **Fast Math**: Modüler aritmetik işlemler için optimize edilmiş fonksiyonlar
- **RFD Entegrasyonu**: 868MHz band üzerinde kablosuz veri iletimi (geliştirilme aşamasında)

## Proje Yapısı

```
rfd-encrypted/
├── src/                 # Ana uygulama kodları
├── rsa/                 # RSA şifreleme kütüphanesi
│   ├── src/
│   │   ├── RSA.h/cpp           # RSA ana sınıfı
│   │   ├── PrimeGenerator.h/cpp # Asal sayı üretici
│   │   └── FastMath.h/cpp      # Matematik fonksiyonları
│   └── test.cpp        # RSA testleri
├── rfd/                # RFD haberleşme kütüphanesi (planlanan)
└── build/              # Derleme çıktıları
```

## Kullanım

### Derleme
```bash
mkdir build && cd build
cmake ..
make
```

### Çalıştırma
```bash
./main
```

## RSA Şifreleme Özellikleri

- **Anahtar Üretimi**: Dinamik asal sayı üretimi
- **Şifreleme/Deşifreleme**: Byte-level şifreleme desteği
- **64-bit Aritmetik**: `__uint64_t` türü ile büyük sayı işlemleri

# Kullandığım Kaynaklar
- [Modüler Aritmetik](https://www.derspresso.com.tr/matematik/moduler-aritmetik)
- [Adım Adım RSA](https://www.cryptool.org/en/cto/rsa-step-by-step/)
- [RSA Wikipedia](https://en.wikipedia.org/wiki/RSA_cryptosystem#Operation)