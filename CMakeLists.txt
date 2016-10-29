CMAKE_MINIMUM_REQUIRED (VERSION 2.6)
PROJECT (FileSender)

INCLUDE (CheckIncludeFiles)
CHECK_INCLUDE_FILES (sys/sendfile.h HAVE_SYS_SENDFILE_H)

INCLUDE (CheckFunctionExists)
CHECK_FUNCTION_EXISTS (socket HAVE_SOCKET)
IF(NOT HAVE_SOCKET)
    CHECK_LIBRARY_EXISTS (socket socket "" HAVE_SOCKET)
ENDIF(NOT HAVE_SOCKET)

ADD_EXECUTABLE (server server.c utils.c utils.h)
ADD_EXECUTABLE (client client.c utils.c utils.h)
