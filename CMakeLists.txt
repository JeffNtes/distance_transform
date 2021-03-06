cmake_minimum_required(VERSION 3.3)

project(distance_transform)



# option(WITH_RTTI "Build DopeVector using RTTI or not" OFF)
# option(WITH_EIGEN "Build DopeVector with Index<Dimension> as Eigen Matrix (if present) or not." OFF)
# option(ATTACH_SOURCES "When generating an IDE project, add DopeVector header files to project sources." OFF)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/extern/DopeVector ${CMAKE_CURRENT_BINARY_DIR}/lib_dope_vector)
include(${CMAKE_CURRENT_SOURCE_DIR}/extern/DopeVector/sources_properties.cmake)


set(toLink DopeVector)
if(CMAKE_COMPILER_IS_GNUCXX)
	find_package(Threads REQUIRED)
	list(APPEND toLink Threads::Threads)
endif()



set(hdr_dir ${CMAKE_CURRENT_SOURCE_DIR}/include)

set(tools_inline_files
	${hdr_dir}/distance_transform/tools/stb_image.h
	${hdr_dir}/distance_transform/tools/stb_image_write.h
	${hdr_dir}/distance_transform/tools/stb_image_resize.h
	${hdr_dir}/distance_transform/tools/tools.inl
)
set_source_files_properties(${tools_inline_files} PROPERTIES XCODE_EXPLICIT_FILE_TYPE "sourcecode.cpp.h")
source_group("distance_transform\\tools" FILES ${tools_inline_files})

set(hdr_inline_files
	${hdr_dir}/distance_transform/inlines/distance_transform.inl
)
set_source_files_properties(${hdr_inline_files} PROPERTIES XCODE_EXPLICIT_FILE_TYPE "sourcecode.cpp.h")
source_group("distance_transform\\inlines" FILES ${hdr_inline_files})

set(hdr_main_files
	${hdr_dir}/distance_transform/distance_transform.hpp
	${hdr_dir}/distance_transform/tools.hpp
)
source_group("distance_transform" FILES ${hdr_main_files})

set(all_hdr ${hdr_inline_files} ${tools_inline_files} ${hdr_main_files})



add_library(${PROJECT_NAME} INTERFACE)

target_include_directories(${PROJECT_NAME} INTERFACE ${hdr_dir})
target_link_libraries(${PROJECT_NAME} INTERFACE ${toLink})

if(ATTACH_SOURCES)
	target_sources(${PROJECT_NAME} INTERFACE ${all_hdr})
endif()
