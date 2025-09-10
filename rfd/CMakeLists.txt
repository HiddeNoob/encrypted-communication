# RFD kütüphanesi için CMakeLists.txt
cmake_minimum_required(VERSION 3.10)

# RFD kaynak dosyaları
set(RFD_SOURCES
    src/RFD.cpp
    src/Packet.cpp
)

# RFD başlık dosyaları
set(RFD_HEADERS
    src/RFD.h
    src/Packet.h
)

# RFD kütüphanesini oluştur
add_library(rfd_lib STATIC ${RFD_SOURCES} ${RFD_HEADERS})

# Include dizinini ayarla
target_include_directories(rfd_lib PUBLIC src)

# Pico SDK entegrasyonu
pico_sdk_init()
target_link_libraries(rfd_lib pico_stdlib hardware_uart)

# RSA kütüphanesine bağımlılık
target_link_libraries(rfd_lib rsa_lib)
target_include_directories(rfd_lib PRIVATE ../rsa/src)
target_include_directories(rfd_lib INTERFACE src)

# RFD test dosyası (gelecekte eklenebilir)
# add_executable(rfd_test test.cpp)
# target_link_libraries(rfd_test rfd_lib)
# target_include_directories(rfd_test PRIVATE src)