#[[
/***********************************************************************************
 * Copyright (c) 2015-2020 Lenguaje-Latino, programación con sintaxis al Español.  *
 *                                                                                 *
 *                        CMake_Root de Latino-WebSocket                           *
 ***********************************************************************************/
]]


#
# >>> CMAKE configuraciones
cmake_minimum_required(VERSION 3.13)

set(INSTALL_LIB_UNIX                             ${CMAKE_INSTALL_PREFIX}/lib)
set(INSTALL_LIB_APPL                             ${CMAKE_INSTALL_PREFIX}/usr/local/lib)

project(latino-socket)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D_GNU_SOURCE -Wall -std=c11 -rdynamic -ldl -fPIC")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS} -D_GNU_SOURCE -g -O0 -Wall -std=c11 -rdynamic -ldl -fPIC")

#
# >>> MS_WINDOWS
if (WIN32)
  #
  # Arquitectura del build
  set(TARGET_ARCH_REGEX             "^.*-march[= ]([^ ]+).*$")
  string(REGEX MATCH                "${TARGET_ARCH_REGEX}" TARGET_ARCH_MATCH ${CMAKE_C_FLAGS} ${CMAKE_CXX_FLAGS})

  if(TARGET_ARCH_MATCH)
      string(REGEX REPLACE          "${TARGET_ARCH_REGEX}" "\\1" TARGET_ARCH ${CMAKE_C_FLAGS} ${CMAKE_CXX_FLAGS})
  else()
      set(TARGET_ARCH               ${CMAKE_HOST_SYSTEM_PROCESSOR})
  endif()

  #
  # Variable
  set(LATINO_PATH                   "C:/Program Files/Latino" CACHE FILEPATH "Ruta de instalacion de latino")

  #
  # Enlazar
  include_directories(${LATINO_PATH}/include)
  link_directories(${LATINO_PATH}/lib)

  #
  # Mensajes
  message(STATUS "ARQUITECTURA .................. ${TARGET_ARCH}")
  message(STATUS "DIRECTORIO .................... ${LATINO_PATH}")
  message(STATUS "INCLUDE ....................... ${LATINO_PATH}/include")
  message(STATUS "LIB ........................... ${LATINO_PATH}/lib")

endif()

#
# >>> macOS
if(APPLE)
link_directories(/usr/local/lib)
include_directories("/usr/local/include")
endif()

#
# >>> LINUX
if(UNIX)
include_directories(include)
endif()

add_library(latino-socket SHARED src/latino-socket.c src/lat_socket_compat.c)
target_link_libraries(latino-socket latino)

#
# >>> instalacion
if(UNIX)
install(TARGETS     latino-socket                          DESTINATION      ${INSTALL_LIB_UNIX})
elseif(APPLE)
install(TARGETS     latino-socket                          DESTINATION      ${INSTALL_LIB_APPL})
endif()