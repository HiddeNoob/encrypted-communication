# RFD kütüphanesi için CMakeLists.txt
cmake_minimum_required(VERSION 3.10)

# RFD kaynak dosyaları
set(RFD_SOURCES
    core/src/RFD.cpp
    core/src/Node.cpp
    packets/src/Packet.cpp
    packets/src/PacketHandler.cpp
    packets/src/HelloPacket.cpp
    packets/src/PingPacket.cpp
    packets/src/SignedMessagePacket.cpp
    packets/src/AckPacket.cpp
    packets/src/KeyPacket.cpp
)

# RFD başlık dosyaları
set(RFD_HEADERS
    headers/rfd_config.h
    core/headers/RFD.h
    core/headers/Node.h
    packets/headers/Packet.h
    packets/headers/PacketHandler.h
    packets/headers/HelloPacket.h
    packets/headers/PingPacket.h
    packets/headers/SignedMessagePacket.h
    packets/headers/AckPacket.h
    packets/headers/KeyPacket.h
)

# RFD kütüphanesini oluştur
add_library(rfd_lib STATIC ${RFD_SOURCES} ${RFD_HEADERS})

# Include dizinini ayarla
target_include_directories(rfd_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/headers
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

# Communication kütüphanesine bağımlılık
target_link_libraries(rfd_lib comm_lib)
target_include_directories(rfd_lib PRIVATE ../comm/headers)

target_include_directories(rfd_lib INTERFACE src)

# RFD test dosyası (gelecekte eklenebilir)
# add_executable(rfd_test test.cpp)
# target_link_libraries(rfd_test rfd_lib)
# target_include_directories(rfd_test PRIVATE src)