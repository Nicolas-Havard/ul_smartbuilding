 ###########################################################################################################################################################
 # SMART BUILDING PROJECT : EXPERIMENTAL ROOM SERVER
 # 
 # The principle of this code is to set a socket on the module and to receive data sent by different modules like temperature, humidity, pressure and gas 
 # for example, by WiFi, and send them thanks to the building's LoRa network to store data on databases and use them in applications
 # 
 # 
 # HAVARD Nicolas, student at Polytech Lille, France
 ###########################################################################################################################################################
 
 
import socket
from network import LoRa
import time
import binascii

lora0_sigfox1 = 1

wifi_port = 15555
lora = LoRa(mode=LoRa.LORAWAN)#, region=LoRa.EU868)          # Asia = LoRa.AS923 | Australia = LoRa.AU915 | Europe = LoRa.EU868 | United States = LoRa.US915

# set the 3 default channels to the same frequency
lora.add_channel(0, frequency=868100000, dr_min=0, dr_max=5)
lora.add_channel(1, frequency=868100000, dr_min=0, dr_max=5)
lora.add_channel(2, frequency=868100000, dr_min=0, dr_max=5)

# create an OTAA authentication parameters
dev_eui = binascii.unhexlify('70B3D5499716C41D')
app_eui = binascii.unhexlify('70B3D57ED000F2D5')
app_key = binascii.unhexlify('3AF039FB4F046849F8E22F512C75A33E')


def decode(encode_string, separation_symb_1, separation_symb_2):
    '''
    #param_1 : str encode_string, 
    #param_2 : str separation_symb_1, 
    #param_3 : str separation_symb_2,
    #
    #return an array of character strings
    #
    #
    #This function allow to decode data the FiPy module will receive by its clients.
    #The principle is to fill an array of character strings with data which composed a string send by client and separated by symbols.
    #
    #EXAMPLE :
    #a module which send this string : "ID : 101 | time : 2018/06/28 | temperature : 25" will be decomposed, first, thanks to the symbol " | ", then with " : ".
    #Finally, function will return the array : ["ID", "101", "time", "2018/06/28", "temperature", "25"] which is easier to use.
    '''

    decoded_string = []

    # First split
    split_string_1 = encode_string.split(" | ")                 # ["str : str | ... | str : str"] >> ["str : str", ..., "str : str"]

    # Second split            
    n = len(split_string_1)
    for i in range(n):
        split_string_2 = split_string_1[i].split(" : ")         # ["str : str", ..., "str : str"] >> ["str", "str", ..., "str", "str"]
        m = len(split_string_2)
        for j in range(m):
            decoded_string += [split_string_2[j]]
    return decoded_string



######################### CODE ###########################



print("Socket creation ...")

if (lora0_sigfox1 == 0):
    # join a network using OTAA (Over the Air Activation)
    lora.join(activation=LoRa.OTAA, auth=(dev_eui, app_eui, app_key), timeout=0)

    # wait until the module has joined the network
    #while not lora.has_joined():
        #time.sleep(2)

    # create a raw LoRa socket
    lora_socket = socket.socket(socket.AF_LORA, socket.SOCK_RAW)
    lora_socket.setsockopt(socket.SOL_LORA, socket.SO_DR, 5)
    lora_socket.setblocking(False)


    # create a raw WiFi socket
    wifi_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)         
    wifi_socket.bind(('', wifi_port))


if (lora0_sigfox1 == 1):



print("Listenning...")
wifi_socket.listen(10)                 

# Client detected and accepted
while True:
    client, addr = wifi_socket.accept()
    print("Client detected. IP add : ", client, "\n")
    print(addr)
    print()

    # Data reception
    while True:
        print("Waiting for data transmission ...")
        message = client.recv(1024).decode('UTF-8')
        #message = str(message)
        #del message[0]
        
        if len(message) == 0:
           break

        # Data analysis
        else:
            print("Message received from ", addr[0], " :\n", message, "\n\n")
            decoded_message = decode(message, " | ", " : ")
            
            print("Message structuring...\n")
            print("IP address : ", client, "\n")
            n = len(decoded_message)
            for i in range(n//2):
                print(decoded_message[2*i], " : ", decoded_message[2*i+1])          
            
            lora_socket.setblocking(True)
            if (decoded_message[0] == "ID"):
                print("Data sending...\n")
                temp=float(decoded_message[3])
                print(temp)
                temp_int = int(temp)
                print(temp_int)
                temp_bytes=temp_int.to_bytes(1), byteorder='big')
                print(temp_bytes)
                lora_socket.send(bytes[temp_bytes])
            lora_socket.setblocking(False)
            
            # get any data received (if any...)
            #data = lora_socket.recv(64)
            #print(data)
            
            print("Data sent\n")
    print("Closing connection")
    client.close()