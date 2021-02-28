add_executable(${PROJECT_NAME} fts.cpp)

target_link_libraries(fts PRIVATE project_options project_warnings)