
ADD_EXECUTABLE(Test test.cpp)

TARGET_LINK_LIBRARIES(Test Patcher)

file(COPY tests DESTINATION ${CMAKE_CURRENT_BINARY_DIR})