import serial
import numpy as np
from PIL import Image

# Abrir la imagen
imagen = Image.open("clara100.jpg")

# Obtener los píxeles de la imagen
pixeles = imagen.load()






pixels = np.zeros((imagen.width, imagen.height, 3), dtype=np.uint8)

cont = 0

auxI = 0
auxJ = 0

f = open("teledeteccion.txt", "w")
f.write('{ ')
for i in range(imagen.height):
    
    for j in range(imagen.width):
        r, g, b = pixeles[j, i]

        f.write(str(r))
        f.write(',')
        f.write(str(g)) #g.to_bytes(1, 'little')      
        f.write(',')
        f.write(str(b))
        f.write(',')

    print(i)

f.write('};')
print(imagen.height)
print(imagen.width)
# Cerrar la conexión serial
f.close()

