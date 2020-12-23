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

project(latino-socket)

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
  
  add_library(latino-socket SHARED src/latino-socket.c)
  target_link_libraries(latino-socket latino)

  #
  # Mensajes
  message(STATUS "ARQUITECTURA .................. ${TARGET_ARCH}")
  message(STATUS "DIRECTORIO .................... ${LATINO_PATH}")
  message(STATUS "INCLUDE ....................... ${LATINO_PATH}/include")
  message(STATUS "LIB ........................... ${LATINO_PATH}/lib")
else()
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D_GNU_SOURCE -Wall -std=c11 -rdynamic -ldl -fPIC")
  set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS} -D_GNU_SOURCE -g -O0 -Wall -std=c11 -rdynamic -ldl -fPIC")
endif()

#
# >>> macOS
if(APPLE)
  # link_directories(/usr/local/lib)
  # include_directories("/usr/local/include")
  # add_library(latino-mysql SHARED src/latino-mysql.c)
  # target_link_libraries(latino-mysql liblatino_static mysqlclient readline)
  # INSTALL(TARGETS latino-mysql DESTINATION /usr/local/lib)
endif()

#
# >>> LINUX
if(UNIX)
  # include_directories(include)
  # add_library(latino-mysql SHARED src/latino-mysql.c)
  # target_link_libraries(latino-mysql mysqlclient latino)
  # INSTALL(TARGETS latino-mysql DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
endif()