cmake_minimum_required(VERSION 3.16)
set(CMAKE_CXX_STANDARD 14)
set(ncnn_DIR "/home/neo/ncnn/build/install/lib/cmake/ncnn")
project(Merge)
find_package(OpenCV REQUIRED)
FIND_PACKAGE( OpenMP REQUIRED)
find_package(ncnn REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})
include_directories("/home/neo/ncnn/build/install/include/ncnn")
include_directories("/home/neo/1.2.176.1/x86_64/include")

if(OPENMP_FOUND)
    message("OPENMP FOUND")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif()

file(GLOB SOURCES
        AngleSolver/*
        ArmorDetector/ArmorDetector.cpp
        ArmorDetector/ArmorDetector.h
        ArmorTracker/Armor_tracker.cpp
        ArmorDetector/ArmorDetector.
        Buffer/*.hpp
        Protocol/protocol.cpp
        Protocol/protocol.h
        Protocol/CRC/*
        Serial/serial.cpp
        Serial/serial.h
        Serial/serialcodec.cpp
        Serial/serialcodec.h
        TaskSwitcher/*
        ThreadManager/*
        CamCapture/*
        ArmorTracker/*
        nanodet/*
        )

add_executable(Merge main.cpp ${SOURCE_FILES} ${SOURCES} Buffer/SolveEssential.h)
target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS} -lpthread -lm)
target_link_libraries(
        Merge
        ncnn
        ${OpenCV_LIBS}
)



