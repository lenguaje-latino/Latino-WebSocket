import socket

mi_socket = socket.socket()
mi_socket.bind( ('localhost', 3000) )
mi_socket.listen(5)

while True:
    conexion, addr = mi_socket.accept()
    print ("Nueva conexión establecida")
    print (addr)

    peticion = conexion.recv(1024)
    print (peticion)

    conexion.send('Desde el servidor'.encode('utf-8'))