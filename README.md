# WebSocket para Latino
Esta es la libreria de WebSocket para el lenguaje de programacion [latino](https://github.com/lenguaje-latino/latino).

## Instalación

### Linux / Mac

#### Prerequisitos

Tener instalado:
[latino](https://github.com/lenguaje-latino/latino)
[cmake](https://cmake.org/download/)
[gcc](https://gcc.gnu.org/) o [clang](https://clang.llvm.org/)

Ejecutar lo siguiente en bash:

```
git clone https://github.com/lenguaje-latino/latino-websocket
cd latino-websocket
git submodule update --init --recursive
mkdir build
cd build
cmake ..
make
sudo make install
```

### Windows

#### Prerequisitos

Tener instalado:
[latino](https://github.com/lenguaje-latino/latino)
[cmake](https://cmake.org/download/)
[visual studio](https://visualstudio.microsoft.com/es/vs/community/)

Ejecutar lo siguiente en cmd:

```
git clone https://github.com/lenguaje-latino/latino-websocket
cd latino-websocket
git submodule update --init --recursive
md build
cd build
cmake -G "Visual Studio 16 2019" -DCMAKE_BUILD_TYPE=Release ..\
```

Abrir con visual studio 2019 y compilar la solucion latino-websocket.sln
Para instalar la libreria abrir visual studio con permisos de administrador
Generar el proyecto de INSTALL.vcxproj

### Uso de esta librería en código latino

Crear un archivo servidor.lat y levantarlo con $latino servidor.lat
```
incluir("socket")

mi_socket = socket.socket("localhost", 3000)
socket.enlazar(mi_socket)
socket.escuchar(mi_socket, 5)

mientras (verdadero)
    conexion = socket.aceptar(mi_socket)

    peticion = socket.recv(conexion, 1024)
    imprimir(peticion)

    socket.enviar(conexion, 'Hola desde el servidor')
fin
```

Crear un archivo cliente.lat y levantarlo con $latino cliente.lat

```
incluir("socket")

mi_socket = socket.socket("localhost", 3000)
conexion = socket.conectar(mi_socket)

socket.enviar(conexion, "Hola desde el cliente")
respuesta = socket.recv(conexion, 1024)

imprimir(respuesta)
socket.cerrar(conexion)
```

#### Cualquier aportación o sugerencia es bienvenida
