from network import Sigfox
import ubinascii

# initalise Sigfox for RCZ1 (You may need a different RCZ Region)
sigfox = Sigfox(mode=Sigfox.SIGFOX, rcz=Sigfox.RCZ1)

# print Sigfox Device ID
print(ubinascii.hexlify(sigfox.id()))

# print Sigfox PAC number
print(ubinascii.hexlify(sigfox.pac()))