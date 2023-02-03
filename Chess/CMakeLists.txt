# CMakeList.txt : CMake project for VulkanGraphics, include source and define
# project specific logic here.
#

set(PROJECT_NAME Chess)
set(ALL_FILES 
main.cpp


Bishop.h
Bishop.cpp
Chess.h 
Chess.cpp 
King.cpp 
King.h 
Knight.cpp 
Knight.h 
Pair.cpp 
Pair.h
Pawn.cpp 
Pawn.h 
Piece.cpp 
Piece.h 
Player.cpp 
Player.h 
PlayerBlack.cpp 
PlayerBlack.h
PlayerWhite.cpp
PlayerWhite.h 
Queen.cpp
Queen.h
Rook.cpp
Rook.h
Table.cpp
Table.h
)

add_executable(Chess ${ALL_FILES})

target_link_libraries(Chess VulkanGraphics)
target_include_directories(Chess PUBLIC VulkanGraphics ${CMAKE_SOURCE_DIR}/Externals)


cmake_minimum_required (VERSION 3.8)
if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET Chess PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.
