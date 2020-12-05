// #include <stdio.h>
// #include <stdlib.h>
// #include <unistd.h>
// #include <string.h>
#include "../include/latino.h"
#include <netdb.h>

#define LIB_WEBSOCKET_NAME "socket"

static void lat_conectar(lat_mv *mv) {
};

static const lat_CReg libwebsocket[] = {
    {"conectar", lat_conectar, 2},
    {NULL, NULL, 0}
};

LATINO_API void latC_abrir_liblatino_websocket(lat_mv *mv) {
    latC_abrir_liblatino(mv, LIB_WEBSOCKET_NAME, libwebsocket);
}

