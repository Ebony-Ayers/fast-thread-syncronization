add_library(${PROJECT_NAME} fts.hpp fts.cpp)

target_link_libraries(fts PRIVATE project_options project_warnings)
