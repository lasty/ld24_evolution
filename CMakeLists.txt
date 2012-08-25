
cmake_minimum_required(VERSION 2.8)
set(CMAKE_MODULE_PATH $(CMAKE_MODULE_PATH) "${CMAKE_CURRENT_LIST_DIR}/cmake")

set(CMAKE_CXX_FLAGS "-Wall -std=gnu++11 -static-libgcc -static-libstdc++" )

set(SOURCES main.cpp app.cpp)
set(DATA_DIRS data)


include_directories( "/usr/local/include" )

add_executable(evolution ${SOURCES})

find_package(SDL2)
include_directories( ${SDL_INCLUDE_DIR} )

find_package(GLEW)
include_directories(GLEW_INCLUDE_DIR)

find_package(OPENGL)
include_directories(${OPENGL_INCLUDE_DIR})


target_link_libraries(evolution ${SDL_LIBRARY} ${GLEW_LIBRARY} ${OPENGL_LIBRARY})


install(TARGETS evolution DESTINATION bin)
install(DIRECTORY ${DATA_DIRS} DESTINATION data)


if(MSYS OR MINGW)
	#gather and install all required DLLs to binary directory
	find_file(SDL_DLL SDL2.dll)
		
	set (DLL_ALL ${SDL_DLL})
	
	install(FILES ${DLL_ALL} DESTINATION bin)
endif(MSYS OR MINGW)
	