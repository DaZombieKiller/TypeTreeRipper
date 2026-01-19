file(GLOB SOURCE_FILES CONFIGURE_DEPENDS "*.cpp" "*.hpp")

if (ANDROID) # needs to be first for cross-compilation purposes
    # EXCLUDE_FROM_ALL so that we don't build libdobby.so
    add_subdirectory("android/dobby" EXCLUDE_FROM_ALL)

    file(GLOB ANDROID_SOURCE_FILES "android/*.cpp" "android/*.hpp")

    add_library(TypeTreeRipper SHARED ${SOURCE_FILES} ${ANDROID_SOURCE_FILES} ${ANDROID_XHOOK_SOURCE_FILES})
    target_include_directories(TypeTreeRipper PRIVATE "." "android")
    target_link_libraries(TypeTreeRipper PRIVATE log dobby_static)
elseif (WIN32)
    find_path(DETOURS_INCLUDE_DIRS "detours/detours.h")
    find_library(DETOURS_LIBRARY detours REQUIRED)
    find_package(wil CONFIG REQUIRED)

    file(GLOB_RECURSE WINDOWS_SOURCE_FILES "windows/*.cpp" "windows/*.hpp")

    add_library(version SHARED ${SOURCE_FILES} ${WINDOWS_SOURCE_FILES})
    target_include_directories(version PRIVATE ${DETOURS_INCLUDE_DIRS} "." "windows")
    target_link_libraries(version PRIVATE ${DETOURS_LIBRARY} WIL::WIL)
elseif(LINUX)
    file(GLOB_RECURSE LINUX_SOURCE_FILES "linux/*.cpp" "linux/*.hpp")

    add_library(TypeTreeRipper SHARED ${SOURCE_FILES} ${LINUX_SOURCE_FILES})
    target_include_directories(TypeTreeRipper PRIVATE "." "linux")
endif()
