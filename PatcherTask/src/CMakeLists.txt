INCLUDE_DIRECTORIES(patcher)

ADD_SUBDIRECTORY(patcher)

ADD_EXECUTABLE(Main main.cpp)

TARGET_LINK_LIBRARIES(Main Patcher)

INCLUDE_DIRECTORIES(test)

ADD_SUBDIRECTORY(test)
