find_path(DETOURS_INCLUDE_DIRS "detours/detours.h")
find_library(DETOURS_LIBRARY detours REQUIRED)
find_package(wil CONFIG REQUIRED)

file(GLOB_RECURSE SOURCE_FILES CONFIGURE_DEPENDS "*.cpp" "*.hpp")
add_library(version SHARED ${SOURCE_FILES})
target_include_directories(version PRIVATE ${DETOURS_INCLUDE_DIRS})
target_link_libraries(version PRIVATE ${DETOURS_LIBRARY} WIL::WIL)
