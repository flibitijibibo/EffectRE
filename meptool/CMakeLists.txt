# CMake File for MEPTool
# Written by Ethan "flibitijibibo" Lee

CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
PROJECT(MEPTool)

# Build Options
OPTION(MEP_DEBUG "Build MEPTool with debugging symbols" ON)

# Compiler Flags
IF(MEP_DEBUG)
	SET(CMAKE_C_FLAGS "-g -Wall")
ELSE()
	SET(CMAKE_C_FLAGS "-O2 -Wall")
ENDIF()

# Defines
ADD_DEFINITIONS(-DMOJOSHADER_NO_VERSION_INCLUDE)

# Source Lists
SET(MEP_SRC meptool.c)
SET(MOJOSHADER_SRC
	mojoshader/mojoshader.c
	mojoshader/mojoshader_common.c
	mojoshader/mojoshader_opengl.c
	mojoshader/mojoshader_effects.c
)

ADD_EXECUTABLE(meptool ${MEP_SRC})
ADD_LIBRARY(mojoshader-static STATIC ${MOJOSHADER_SRC})
TARGET_LINK_LIBRARIES(meptool mojoshader-static m)
