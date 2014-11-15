# CMake File for Sprite Test
# Written by Ethan "flibitijibibo" Lee

CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
PROJECT(SpriteTest)

# Build Options
OPTION(ST_DEBUG "Build SpriteTest with debugging symbols" ON)

# Compiler Flags
IF(ST_DEBUG)
	SET(CMAKE_C_FLAGS "-g -Wall")
ELSE()
	SET(CMAKE_C_FLAGS "-O2 -Wall")
ENDIF()

# Defines
ADD_DEFINITIONS(-DMOJOSHADER_NO_VERSION_INCLUDE)

# Include Directories
INCLUDE_DIRECTORIES(../meptool/mojoshader)

# Source Lists
SET(ST_SRC spritetest.c)
SET(MOJOSHADER_SRC
	../meptool/mojoshader/mojoshader.c
	../meptool/mojoshader/mojoshader_common.c
	../meptool/mojoshader/mojoshader_opengl.c
	../meptool/mojoshader/mojoshader_effects.c
)

ADD_EXECUTABLE(spritetest ${ST_SRC})
ADD_LIBRARY(mojoshader-static STATIC ${MOJOSHADER_SRC})
TARGET_LINK_LIBRARIES(spritetest mojoshader-static m SDL2 GL)
