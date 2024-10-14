# Process-Pipes-In-C-
There will be a logger – responsible for logging all activity. There will be an encryption program – responsible for encrypting and decrypting strings. There will be a driver program that will interact with the user to use the encryption program. The entire system will be ran by running the driver program, which communicates through pipes.

In Linux Enviroment:
g++ main.cpp -o driver
g++ logger.cpp -o logger
g++ encryption.cpp -o encryption

./driver out.txt
