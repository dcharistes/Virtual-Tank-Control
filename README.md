#Virtual Tank Control with Arduino Uno
This project involves programming an ATMEGA328P microcontroller on an Arduino Uno to control the operation of a virtual tank system. 
The modules used are an LCD for displaying messages, a keypad for user input, an EEPROM flash for storing data, and serial communication for debugging and monitoring. 
The User can input a desired volume of liquid, which is shown on the LCD and stored in the EEPROM. The system then controls the tank's inlet and outlet valves to achieve the specified volume. 
Once the target volume is reached, a stirrer and cooler are activated to maintain the required temperature. Serial communication is used throughout the process for real-time feedback. 
The program was tested through the virtual-simulation tank in PICSimLab by uploading the compiled hex file.
