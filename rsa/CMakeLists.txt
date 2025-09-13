# RSA kütüphanesi için CMakeLists.txt
cmake_minimum_required(VERSION 3.10)

# RSA kaynak dosyaları
set(RSA_SOURCES
    src/RSA.cpp
    src/Key.cpp
)

# RSA başlık dosyaları
set(RSA_HEADERS
    headers/RSA.h
    headers/Key.h
    headers/config.h
)

# RSA kütüphanesini oluştur
add_library(rsa_lib STATIC ${RSA_SOURCES} ${RSA_HEADERS})

# Include dizinini ayarla
target_include_directories(rsa_lib PUBLIC 
    headers
    src
)

# Pico SDK entegrasyonu
pico_sdk_init()
target_link_libraries(rsa_lib pico_stdlib)

# Math kütüphanesine bağımlılık
target_link_libraries(rsa_lib math_lib)
target_include_directories(rsa_lib PRIVATE ../math/headers)

