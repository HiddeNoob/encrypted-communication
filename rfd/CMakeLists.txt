# RFD kütüphanesi için CMakeLists.txt
cmake_minimum_required(VERSION 3.10)

# RFD kaynak dosyaları
set(RFD_SOURCES
    core/RFD.cpp
    core/Node.cpp
    packets/Packet.cpp
    packets/HelloPacket.cpp
    packets/PingPacket.cpp
    packets/SignedMessagePacket.cpp
    packets/AckPacket.cpp
)

# RFD başlık dosyaları
set(RFD_HEADERS
    core/headers/RFD.h
    core/headers/Node.h
    packets/headers/Packet.h
    packets/headers/HelloPacket.h
    packets/headers/PingPacket.h
    packets/headers/SignedMessagePacket.h
    packets/headers/AckPacket.h
)

# RFD kütüphanesini oluştur
add_library(rfd_lib STATIC ${RFD_SOURCES} ${RFD_HEADERS})

# Include dizinini ayarla
target_include_directories(rfd_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/core
    ${CMAKE_CURRENT_SOURCE_DIR}/core/headers
    ${CMAKE_CURRENT_SOURCE_DIR}/packets
    ${CMAKE_CURRENT_SOURCE_DIR}/packets/headers
)

# Pico SDK entegrasyonu
pico_sdk_init()
target_link_libraries(rfd_lib pico_stdlib hardware_uart)

# RSA kütüphanesine bağımlılık
target_link_libraries(rfd_lib rsa_lib)
target_include_directories(rfd_lib PRIVATE ../rsa/headers)

# Math kütüphanesine bağımlılık  
target_link_libraries(rfd_lib math_lib)
target_include_directories(rfd_lib PRIVATE ../math/headers)

target_include_directories(rfd_lib INTERFACE src)

# RFD test dosyası (gelecekte eklenebilir)
# add_executable(rfd_test test.cpp)
# target_link_libraries(rfd_test rfd_lib)
# target_include_directories(rfd_test PRIVATE src)