add_library(${PROJECT_NAME} fts.hpp fts.cpp)

target_link_libraries(fts PRIVATE fts_project_options fts_project_warnings)
