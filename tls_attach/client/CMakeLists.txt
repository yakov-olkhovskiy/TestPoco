add_executable(client main.cpp)

target_link_libraries(client PUBLIC Poco::Net Poco::Net::SSL)
