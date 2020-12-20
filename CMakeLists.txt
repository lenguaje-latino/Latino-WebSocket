cmake_minimum_required(VERSION 3.13)

project(latino-socketc)

if (WIN32)
  #
  # Arquitectura del build
  set(TARGET_ARCH_REGEX                       "^.*-march[= ]([^ ]+).*$")
  string(REGEX MATCH                          "${TARGET_ARCH_REGEX}" TARGET_ARCH_MATCH ${CMAKE_C_FLAGS} ${CMAKE_CXX_FLAGS})

  if(TARGET_ARCH_MATCH)
      string(REGEX REPLACE                    "${TARGET_ARCH_REGEX}" "\\1" TARGET_ARCH ${CMAKE_C_FLAGS} ${CMAKE_CXX_FLAGS})
  else()
      set(TARGET_ARCH                         ${CMAKE_HOST_SYSTEM_PROCESSOR})
  endif()

# cmake_INSTALL_PREFIX
  # set (MySQL_PATH "C:/Program Files/MySQL/MySQL Server 8.0" CACHE FILEPATH "Ruta de instalacion de MySQL")
  message(STATUS "ARQUITECTURA .................. ${TARGET_ARCH}")

  set(CMAKE_INSTALL_PREFIX "C:/Program Files")
  message(STATUS "DIRECTORIO .................... ${CMAKE_INSTALL_PREFIX}")
  set (LATINO_PATH "${CMAKE_INSTALL_PREFIX}/Latino" CACHE FILEPATH "Ruta de instalacion de latino")
  include_directories(${LATINO_PATH}/include)
  link_directories(${LATINO_PATH}/lib)
  add_library(latino-socketc SHARED src/latino-socketc.c)
  target_link_libraries(latino-socketc latino)
else()
  SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D_GNU_SOURCE -Wall -std=c11 -rdynamic -ldl -fPIC")
  SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS} -D_GNU_SOURCE -g -O0 -Wall -std=c11 -rdynamic -ldl -fPIC")
endif()

if(APPLE)
  LINK_DIRECTORIES(/usr/local/lib)
  include_directories("/usr/local/include")
#   add_library(latino-mysql SHARED src/latino-mysql.c)
#   target_link_libraries(latino-mysql liblatino_static mysqlclient readline)
#   INSTALL(TARGETS latino-mysql DESTINATION /usr/local/lib)
endif()

if(UNIX)
  include_directories(include)
#   add_library(latino-mysql SHARED src/latino-mysql.c)
#   target_link_libraries(latino-mysql mysqlclient latino)
#   INSTALL(TARGETS latino-mysql DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
endif()