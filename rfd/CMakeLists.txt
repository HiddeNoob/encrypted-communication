# RFD kütüphanesi için CMakeLists.txt
cmake_minimum_required(VERSION 3.10)

# RFD kaynak dosyaları (şu an boş, gelecekte eklenecek)
set(RFD_SOURCES
    # Gelecekte eklenecek .cpp dosyaları
    # src/RFD.cpp
    # src/RFDUtils.cpp
)

# RFD başlık dosyaları (şu an boş, gelecekte eklenecek)
set(RFD_HEADERS
    # Gelecekte eklenecek .h dosyaları
    # src/RFD.h
    # src/RFDUtils.h
)

# Şimdilik boş bir kütüphane oluştur
# Gelecekte RFD kaynak dosyaları eklendiğinde bu satırı güncelle:
# add_library(rfd_lib STATIC ${RFD_SOURCES} ${RFD_HEADERS})

# Geçici olarak boş bir interface kütüphanesi oluştur
add_library(rfd_lib INTERFACE)

# Include dizinini ayarla (gelecekte kullanım için)
target_include_directories(rfd_lib INTERFACE src)

# RFD test dosyası (gelecekte eklenebilir)
# add_executable(rfd_test test.cpp)
# target_link_libraries(rfd_test rfd_lib)
# target_include_directories(rfd_test PRIVATE src)