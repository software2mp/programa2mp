
SET(CMAKE_SYSTEM_NAME Windows)

IF (NOT COMPILER_PREFIX)
        SET(COMPILER_PREFIX mingw32)
ENDIF(NOT COMPILER_PREFIX)

# which compilers to use for C and C++
SET(CMAKE_C_COMPILER ${COMPILER_PREFIX}-gcc)
SET(CMAKE_CXX_COMPILER ${COMPILER_PREFIX}-g++)
SET(CMAKE_RESOURCE_COMPILER ${COMPILER_PREFIX}-windres)

# here is the target environment located
IF (NOT USR_LOCAL)
        SET(USR_LOCAL "~/opt/local-mingw32msvc/")
ENDIF(NOT USR_LOCAL)

IF (NOT MINGW_ROOT_PATH)
        SET(MINGW_ROOT_PATH "/usr/mingw32/usr/")
ENDIF (NOT MINGW_ROOT_PATH)

SET(CMAKE_FIND_ROOT_PATH  ${MINGW_ROOT_PATH} ${USR_LOCAL} ~/opt/local)

# adjust the default behaviour of the FIND_XXX() commands:
# search headers and libraries in the target environment, search 
# programs in the host environment
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
