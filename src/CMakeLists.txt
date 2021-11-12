add_library(${PROJECT_NAME} fts.hpp)

target_link_libraries(fts PRIVATE project_options project_warnings)