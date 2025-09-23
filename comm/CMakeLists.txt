# Communication kütüphanesi için CMakeLists.txt
cmake_minimum_required(VERSION 3.10)

# Communication kaynak dosyaları
set(COMM_SOURCES
    src/UART.cpp
)

# Communication başlık dosyaları
set(COMM_HEADERS
    headers/IComm.h
    headers/UART.h
)

# Communication kütüphanesini oluştur
add_library(comm_lib STATIC ${COMM_SOURCES} ${COMM_HEADERS})

# Include dizinini ayarla
target_include_directories(comm_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/headers
)

# Pico SDK entegrasyonu (sadece gerekirse)
if(PICO_SDK_PATH)
    target_link_libraries(comm_lib pico_stdlib hardware_uart)
endif()

# RFD test dosyası (gelecekte eklenebilir)
# add_executable(rfd_test test.cpp)
# target_link_libraries(rfd_test rfd_lib)
# target_include_directories(rfd_test PRIVATE src)