cmake_minimum_required(VERSION 3.1)
cmake_policy(SET CMP0054 NEW)

project(FileSender)

include(CheckFunctionExists)
include(CheckIncludeFiles)
include(CheckLibraryExists)
include(CheckStructHasMember)

macro(CMAKE_SEARCH_LIBS v_func v_lib func)
  check_function_exists(${func} ${v_func})
  if(NOT ${v_func})
    foreach(lib ${ARGN})
      check_library_exists(${lib} ${func} "" HAVE_${func}_IN_${lib})
      if(HAVE_${func}_IN_${lib})
        set(${v_func} TRUE)
        set(${v_lib} "${lib}" CACHE INTERNAL "Library providing ${func}")
        break()
      endif()
    endforeach()
  endif()
endmacro()


check_include_files(sys/sendfile.h HAVE_SYS_SENDFILE_H)
if(HAVE_SYS_SENDFILE_H)
  add_compile_definitions(HAVE_SYS_SENDFILE_H)
endif()

cmake_search_libs(HAVE_SENDFILE SENDFILE_LIB sendfile sendfile)
cmake_search_libs(HAVE_SOCKET SOCKET_LIB socket socket)

if(HAVE_SENDFILE)
  add_compile_definitions(HAVE_SENDFILE)
endif()

check_struct_has_member("struct sf_hdtr" hdr_cnt sys/socket.h HAVE_STRUCT_SF_HDTR_HDR_CNT)
if(HAVE_STRUCT_SF_HDTR_HDR_CNT)
  add_compile_definitions(HAVE_STRUCT_SF_HDTR_HDR_CNT)
endif()

add_executable(server server.c utils.c utils.h)
target_link_libraries(server ${SOCKET_LIB})

add_executable(client client.c utils.c utils.h)
target_link_libraries(client ${SOCKET_LIB} ${SENDFILE_LIB})

