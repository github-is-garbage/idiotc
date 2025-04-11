cmake_minimum_required(VERSION 3.30)

project(idiotc VERSION 1.0.0)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED True)
set(CMAKE_GENERATOR_PLATFORM x64)
set(CMAKE_C_FLAGS_RELEASE "-Os -s")

file(GLOB_RECURSE SOURCES
	"src/**.c"
)

file(GLOB_RECURSE HEADERS
	"src/**.h"
)

set(RESOURCES src/resource.rc)

add_executable(${PROJECT_NAME} ${SOURCES} ${RESOURCES})

set_target_properties(
	${PROJECT_NAME} PROPERTIES

	OUTPUT_NAME "idiotc"
	PREFIX ""
)

target_compile_definitions(${PROJECT_NAME} PRIVATE
	WIN32_LEAN_AND_MEAN
)

target_link_libraries(${PROJECT_NAME} winmm)

install(
	TARGETS ${PROJECT_NAME}
	RUNTIME DESTINATION bin
)
