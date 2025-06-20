
import struct
import sys
import numpy as np
from PIL import Image
from scapy.all import sendp, sniff, Ether, Raw, get_if_hwaddr, show_interfaces

INTERFACE_NAME = "enp7s0"  

FPGA_MAC = "00:18:3e:a7:01:02"  
CUSTOM_ETHER_TYPE = 0xebeb       
PAYLOAD_SIZE = 4                 



def crear_imagen_desde_array(datos_pixeles, ancho, alto, nombre_archivo="imagen_creada.png"):
   
    longitud_esperada = ancho * alto * 3  
    print(" Inicio de compilación de imagen\n")

    if len(datos_pixeles) != longitud_esperada:
        print(f"[ERROR] La cantidad de datos de píxeles ({len(datos_pixeles)}) no coincide "
              f"con las dimensiones esperadas ({ancho}x{alto}x3 = {longitud_esperada}).")
        return

    try:
   
        array_np = np.array(datos_pixeles, dtype=np.uint8)
        array_redimensionado = array_np.reshape((alto, ancho, 3))
        imagen = Image.fromarray(array_redimensionado, 'RGB')
        imagen.save(nombre_archivo)
        print(f"¡Imagen guardada exitosamente como '{nombre_archivo}'!")
        

    except Exception as e:
        print(f"Ocurrió un error al crear la imagen: {e}")





def main():
    print(f"Usando interfaz: '{INTERFACE_NAME}'")
    img =[]
    try:
        pc_mac = get_if_hwaddr(INTERFACE_NAME)
        print(f"MAC del PC detectada: {pc_mac}")
    except Exception:
        print(f"[ERROR] No se pudo encontrar la interfaz '{INTERFACE_NAME}'.")
        print("Interfaces disponibles:")
        show_interfaces()
        sys.exit(1)

    sniff_filter = f"ether proto 0x{CUSTOM_ETHER_TYPE:04x} and ether src {FPGA_MAC}"
 
    value_to_send = 1
	
    payload_data = struct.pack('>I', value_to_send)
	
    padding = b'\x00' * (46 - len(payload_data))
	
    response_packet = Ether(src=pc_mac, dst=FPGA_MAC, type=CUSTOM_ETHER_TYPE) / Raw(load=payload_data + padding)
	
    while True:
        for i in range (100*100*3):
            print(i)

            if i>0:
                sendp(response_packet, iface=INTERFACE_NAME, verbose=False)
            packets = sniff(iface=INTERFACE_NAME, filter=sniff_filter, count=1)
            packet = packets[0]

            if packet.haslayer(Raw):
                payload = bytes(packet[Raw].load)
	
                if len(payload) >= PAYLOAD_SIZE:
                    
                    received_value = struct.unpack('>I', payload[:PAYLOAD_SIZE])[0]
                    img.append(received_value)
                else:
                    continue

        crear_imagen_desde_array(img, 100, 100, "imagen_prueba.png")
					
					
                
            

if __name__ == "__main__":
    main()
