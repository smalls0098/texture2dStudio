cmake_minimum_required(VERSION 3.12.0 FATAL_ERROR)

set(MY_PROJECT_NAME texture2dstudio)
project(${MY_PROJECT_NAME} VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 14)


if(MSVC)
    set(MY_VERSIONINFO_RC "${CMAKE_CURRENT_SOURCE_DIR}/VersionInfo.rc")
    configure_file("${CMAKE_CURRENT_SOURCE_DIR}/VersionInfo.rc.in"
            "${MY_VERSIONINFO_RC}")
endif()

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/src SOURCE_CODES)


set(MODULE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/)
#etc2comp
set(ETC2COMP ${MODULE_DIR}/etc2comp/)
aux_source_directory(${ETC2COMP}/EtcLib/Etc SOURCE_CODES)
aux_source_directory(${ETC2COMP}/EtcLib/EtcCodec SOURCE_CODES)
#lodepng
aux_source_directory(${MODULE_DIR}/lodepng SOURCE_CODES)
#astc-encoder
aux_source_directory(${MODULE_DIR}/astc-encoder/Source SOURCE_CODES)
#texture2ddecoder
aux_source_directory(${MODULE_DIR}/texture2ddecoder SOURCE_CODES)

include_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        #etc2comp
        ${ETC2COMP}/EtcLib/Etc
        ${ETC2COMP}/EtcLib/EtcCodec
        #lodepng
        ${MODULE_DIR}/lodepng
        #astc-encoder
        ${MODULE_DIR}/astc-encoder/Source
        ${MODULE_DIR}/astc-encoder/Source
        #texture2ddecoder
        ${MODULE_DIR}/texture2ddecoder
#        ${MODULE_DIR}/texture2ddecoder/crunch
#        ${MODULE_DIR}/texture2ddecoder/fp16
#        ${MODULE_DIR}/texture2ddecoder/unitycrunch
)

# SHARED,动态库
# STATIC,静态库
# MODULE,在使用 dyld 的系统有效,如果不支持 dyld,则被当作 SHARED 对待。
# mac如果生成动态库有问题。dylib运行会出错，可能是需要证书之类的
# mac使用STATIC静态库运行正常
#add_library(
#        ${MY_PROJECT_NAME}
#        STATIC
#        ${CMAKE_CURRENT_SOURCE_DIR}/texture2d.cpp
#        ${CMAKE_CURRENT_SOURCE_DIR}/texture2d.h
#        ${SOURCE_CODES}
#
#        ${MY_VERSIONINFO_RC}
#)

add_executable(
        ${MY_PROJECT_NAME}
        ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/texture2d.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/texture2d.h
        ${SOURCE_CODES}
)
