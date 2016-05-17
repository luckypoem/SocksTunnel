cmake_minimum_required(VERSION 3.0)
project(SocksTunnelLocal)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(LINKLIB ev event pthread crypto)

set(SOURCE_FILES
        src/CommonLib/Atomic.h
        src/CommonLib/Json.cpp
        src/CommonLib/Json.h
        src/CommonLib/Logger.h
        src/CommonLib/Logger.cpp
        src/CommonLib/Mutex.h
        src/CommonLib/Mutex.cpp
        src/CommonLib/NoCopyable.h
        src/CommonLib/SharedPtr.h
        src/CommonLib/Singleton.h
        src/CommonLib/StringUtils.cpp
        src/CommonLib/StringUtils.h
        src/CommonLib/Timestamp.cpp
        src/CommonLib/Timestamp.h
        src/CommonLib/Exception/Exception.h
        src/CommonLib/Exception/FileNotFoundException.h
        src/CommonLib/Exception/InitFailedException.h
        src/CommonLib/Exception/KeyNotFoundException.h
        src/CommonLib/Exception/MultiSetException.h
        src/CommonLib/Exception/OutOfRangeException.h
        src/CommonLib/Exception/ParseException.h
        src/CommonLib/Exception/StringBaseException.h
        src/CommonLib/Exception/UnKnowKeyException.h
        src/SocksTunnelLocal.cpp src/SocksTunnelLocal.h
        src/CommonLib/Reader.cpp
        src/CommonLib/Reader.h
        src/Common/MsgResolver.cpp
        src/Common/MsgResolver.h
        src/SocksTunnelServer.h
        src/SocksTunnelServer.cpp
        src/Common/ServerCommon.cpp
        src/Common/ServerCommon.h
        src/Crypto/Crypto.h
        src/Crypto/Rc4.cpp
        src/Crypto/Rc4.h
        src/Crypto/CryptoHelper.cpp
        src/Crypto/CryptoHelper.h
        src/Utils/BufferReader.cpp
        src/Utils/BufferReader.h
        src/Utils/SettingUtils.cpp
        src/Utils/SettingUtils.h
        src/Utils/BufferWriter.cpp
        src/Utils/BufferWriter.h
        src/Utils/RandomUtils.cpp
        src/Utils/RandomUtils.h
        src/Crypto/Xor.cpp
        src/Crypto/Xor.h
)
add_executable(SocksTunnelServer ${SOURCE_FILES} src/server.cpp)
target_link_libraries(SocksTunnelServer ${LINKLIB})

add_executable(SocksTunnelLocal ${SOURCE_FILES} src/client.cpp)
target_link_libraries(SocksTunnelLocal ${LINKLIB})
