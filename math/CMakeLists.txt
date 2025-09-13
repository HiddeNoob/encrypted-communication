# Math kütüphanesi için CMakeLists.txt
cmake_minimum_required(VERSION 3.10)

# Math kaynak dosyaları
set(MATH_SOURCES
    src/FastMath.cpp
    src/PrimeGenerator.cpp
)

# Math başlık dosyaları
set(MATH_HEADERS
    headers/FastMath.h
    headers/PrimeGenerator.h
)

# Math kütüphanesini oluştur
add_library(math_lib STATIC ${MATH_SOURCES} ${MATH_HEADERS})

# Include dizinini ayarla
target_include_directories(math_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/headers
)

# Pico SDK entegrasyonu
pico_sdk_init()
target_link_libraries(math_lib pico_stdlib)