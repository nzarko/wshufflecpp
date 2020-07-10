cmake_minimum_required(VERSION 2.8.12)

PROJECT(wshuffle)
SET(CMAKE_CXX_STANDARD 14)

if(MSVC)
    SET(CMAKE_TOOLCHAIN_FILE "E:/src/c_c++/vcpkg/scripts/buildsystems/windows.cmake")
    SET(VCPKG_TARGET_TRIPLET x64-windows)
    set(BOOST_ROOT "E:/src/c_c++/vcpkg/installed/x64-windows")
    ADD_DEFINITIONS( "/D _WIN32_WINNT=0x0A00 /D _CRT_SECURE_NO_WARNINGS" )
    set(OS_SRC wineditor.cpp)
    set(OS_HDR wineditor.h)
    ADD_DEFINITIONS("/DWSHSRC=\"${PROJECT_SOURCE_DIR}\"")
elseif(UNIX)
    set(OS_SRC posixeditor.cpp)
    set(OS_HDR posixeditor.h)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -pthread")
    SET(SRCDIRPATH ${PROJECT_SOURCE_DIR})
    ADD_DEFINITIONS(-DWSHSRC=\"${PROJECT_SOURCE_DIR}\")
endif()

find_package(Boost COMPONENTS system filesystem program_options)

if(POLICY CMP0076)
  cmake_policy(SET CMP0076 NEW)
endif()

if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS}) 
    
    message(status "* Boost_FOUND : ${BOOST_DIR}")
    message(status "** Boost Include: ${Boost_INCLUDE_DIR}")
	message(status "** Boost Libraries: ${Boost_LIBRARY_DIRS}")
	message(status "** Boost Libraries: ${Boost_LIBRARIES}")
    add_executable(wshuffle "")
    target_sources(wshuffle
        PRIVATE
            cliparser.cpp
            Editor.cpp
            ishufflerbase.cpp
            shufflerbase.cpp
            utils.cpp
            ${OS_SRC}
            wshuffle.cpp         
        PUBLIC 
            utils.hpp
            Editor.h
            ishufflerbase.h
            shufflerbase.h
            ${OS_HDR}
            cliparser.h
        )   
    TARGET_LINK_LIBRARIES(wshuffle ${OpenCV_LIBS} ${Boost_LIBRARIES})
endif()