import serial

ser = serial.Serial(
        # Serial Port to read the data from
        port='/dev/ttyAMA0',
 
        #Rate at which the information is shared to the communication channel
        baudrate = 9600,
   
        #Applying Parity Checking (none in this case)
        parity=serial.PARITY_NONE,
 
       # Pattern of Bits to be read
        stopbits=serial.STOPBITS_ONE,
     
        # Total number of bits to be read
        bytesize=serial.EIGHTBITS,
 
        # Number of serial commands to accept before timing out
        timeout=1
)
# Pause the program for 1 second to avoid overworking the serial port
while 1:
        x=ser.readline()
        print (x)
