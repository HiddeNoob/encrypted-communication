# RSA kütüphanesi için CMakeLists.txt
cmake_minimum_required(VERSION 3.10)

# RSA kaynak dosyaları
set(RSA_SOURCES
    src/RSA.cpp
    src/FastMath.cpp
    src/PrimeGenerator.cpp
)

# RSA başlık dosyaları
set(RSA_HEADERS
    src/RSA.h
    src/FastMath.h
    src/PrimeGenerator.h
)

# RSA kütüphanesini oluştur
add_library(rsa_lib STATIC ${RSA_SOURCES} ${RSA_HEADERS})

# Include dizinini ayarla
target_include_directories(rsa_lib PUBLIC src)

# Test çalıştırılabilir dosyası (isteğe bağlı)
add_executable(rsa_test rsa-test.cpp)
target_link_libraries(rsa_test rsa_lib)
target_include_directories(rsa_test PRIVATE src)