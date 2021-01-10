/*********************************************************************************
* The MIT License (MIT)                                                          *
*                                                                                *
* Copyright (c) Latino - Lenguaje de Programacion                                *
*                                                                                *
* Permission is hereby granted, free of charge, to any person obtaining          *
* a copy of this software and associated documentation files (the "Software"),   *
* to deal in the Software without restriction, including without limitation      *
* the rights to use, copy, modify, merge, publish, distribute, sublicense,       *
* and/or sell copies of the Software, and to permit persons to whom the Software *
* is furnished to do so, subject to the following conditions:                    *
*                                                                                *
* The above copyright notice and this permission notice shall be included in     *
* all copies or substantial portions of the Software.                            *
*                                                                                *
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS        *
* OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,    *
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL        *
* THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER     *
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,  *
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN      *
* THE SOFTWARE.                                                                  *
**********************************************************************************/

#ifndef _LATINO_SOCKET_H_
#define _LATINO_SOCKET_H_

/** MS-Windows*/
// https://docs.microsoft.com/en-us/windows/win32/winsock/complete-server-code

// Definicion para el sistema operativo MS-Windows
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#define LATINO_LIB

#if (defined __WIN32__) || (defined _WIN32)
// INICIO _WIN32
#include <WinSock2.h>
#include <WS2tcpip.h>

#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "Mswsock.lib")
#pragma comment(lib, "AdvApi32.lib")

#define LATINO_BUILD_AS_DLL

// FIN _WIN32
#else
/** Unix*/
//INICIO _UNIX-BASE_OS
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#define SOCKET                          int
#define INVALID_SOCKET          (SOCKET)(~0)
#define SOCKET_ERROR                    (-1)
#define SD_SEND                         SHUT_RDWR
#define ZeroMemory(Destination,Length)  memset((Destination),0,(Length))
#endif  // FIN _UNIX-BASE_OS

#include "latino.h"

//** Definiciones*/
#define DEFAULT_PORT                    "3000"
#define DEFAULT_BUFLEN                  512

/** Structura*/
typedef struct LatSocket {
    SOCKET              socket;
    struct addrinfo     *result;
} LatSocket;

/** Funciones*/
LATINO_API  lat_objeto   *lat_sock_crear(lat_mv *mv, char *str_host, char *c_puerto);
LATINO_API  void         lat_sock_bind(lat_mv *mv, LatSocket *latSocket);
LATINO_API  lat_objeto   *lat_sock_conectar(lat_mv *mv, LatSocket *latSocket);
LATINO_API  void         lat_sock_cerrar(lat_mv *mv, LatSocket *lat_sock);

#endif //_LATINO_SOCKET_H_