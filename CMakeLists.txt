cmake_minimum_required(VERSION 3.10)
project(encrypted-rfd-communication)

# C++ standart ayarları
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Debug bilgileri için
set(CMAKE_BUILD_TYPE Debug)

# Alt dizinleri ekle
add_subdirectory(rsa)
add_subdirectory(rfd)

# Ana çalıştırılabilir dosya
add_executable(main src/run.cpp)

# Kütüphaneleri bağla
target_link_libraries(main rsa_lib rfd_lib)

# Include dizinlerini ekle
target_include_directories(main PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/rsa/src
    ${CMAKE_CURRENT_SOURCE_DIR}/rfd/src
)