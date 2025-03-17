# Caesar Cipher device driver
This is a Caesar Cipher device driver, created using C in Kernel space, in which a user can input a word and have that word be either encrypted or decrypted. They can also select a key that they want to use for the encryption/decryption. A Caesar cipher is simply an encrypted message wherein letters are replaced by letters an x number away. For example, “Ocean” would be “Qegcp” if x were 2 and we were ascending ASCII codes per letter. X can also be referred to as the “key”. This is the term we will be using from now on. 

I created the program in this way. I first initialized and registered the device driver to the Kernel. I then created the function called “myOpen” which opens the device file (created when installing the kernel object). This is an essential step as the device driver cannot do work unless the file is opened. Afterwards, I wrote the “myWrite” function to write the user input into a buffer which will he accessed for encryption and decryption. Next, I created the I/O control function to process the encryption or decryption for the user. It also allows the user to specify an encryption/decryption key. Along with this function, I created the “encrypt” and “decrypt”functions which perform the encryption or decryption for the user. The “capitalize” function is a helper function I wrote for the “encrypt” and “decrypt” functions to capitalize user input. It makes the encryption/decryption process easy and consistent (in my opinion, at least). Next, I created the myRead function to read the result of the encryption or decryption into the user’s buffer so they acquire their result. The “myClose” function was written to close the device file. Finally, the “cleanup_module” function was written to unregister and remove the device driver. 

The user will be prompted to request encryption, decryption, to change the key, or to exit.
Encryption with the default key value of 4 looks like this:

![image](https://github.com/user-attachments/assets/4e25fed4-0cc2-4a9b-b955-d8a8eb92a01e)



Decryption with the default key value of 4 looks like this:

![image](https://github.com/user-attachments/assets/fd7bf893-70d6-4e9d-9ce6-73102ce8e53a)



Changing the key, and performing encryption and decryption with a new key value of 3245 looks like this:

![image](https://github.com/user-attachments/assets/583dd963-49c9-4681-affb-c7c531c34dfd)



## Instructions for how to build, load, install, and even uninstall and unload my kernel module:
1) The first thing that must be done is to change the “BASENAME” in the Makefile of the Module folder. Open the Makefile, and in the 6th line which reads “BASENAME=”, type in “caesarCipherModule” as shown here:
![image](https://github.com/user-attachments/assets/1de49bfe-4655-4d38-a932-21a1c8114cfa)


2) Next, in the terminal, run the command “make”. Once this is done, the kernel object has successfully been created as evidenced by the compilation:

![image](https://github.com/user-attachments/assets/807fa97f-dc7c-4be5-a734-1f61e8d90112)

3) Next, we begin installing the newly created kernel object by running three commands,  
a. sudo insmod caesarCipherModule.ko  
b. sudo mknod /dev/CaesarCipher c 333 3  
c. sudo chmod 666 /dev/CaesarCipher 

in that order. The first command inserts a module into the kernel, the second command creates the device file, which is a special file, and the third command sets file permissions. If successful, you will get the following  messages by running “tail /var/log/kern.log” in the command line (the messages are located towards the end):

![image](https://github.com/user-attachments/assets/b9d90671-c013-42e8-beac-194a8266c01b)

Here is a closer look at what I mean:

![image](https://github.com/user-attachments/assets/2b3e25b1-6853-4f89-8642-7802857cc0da)

The kernel object has been successfully installed now and the special device file has also 
been created. To prove this, here’s the screenshot of the device file highlighted when 
running “ls /dev/C*” in the command line:

![image](https://github.com/user-attachments/assets/70d16bc2-2098-4786-ae03-7c31144a9354)



4)  The kernel module has been successfully built and installed, and the device file has successfully been created. But what if we wanted to delete the device file and remove the kernel object from the kernel? Well, we can begin by running “make clean”. Then once we run this, we can remove the device file by running “sudo rm /dev/CaesarCipher” in the command line as shown here: 

![image](https://github.com/user-attachments/assets/55e80be5-f093-464c-80cd-31efa6fe676b)

If we try to find this file, we get this message:

![image](https://github.com/user-attachments/assets/dddbb3c3-823e-4d4c-9c44-fc0fc495b175)

indicating that it has been removed. Now, we can remove the kernel object from the 
kernel by running “sudo rmmod caesarCipherModule.ko” in the command line as shown 
here:

![image](https://github.com/user-attachments/assets/bfd89109-170d-4455-8875-93553ba17393)


We get this message from the kernel log indicating that the kernel object has now been 
unregistered/removed from the kernel: 

![image](https://github.com/user-attachments/assets/3f392801-9010-410c-a7c7-e57f1c225719)

Here is a closer look:

![image](https://github.com/user-attachments/assets/bb16f09f-7404-451a-8e66-af9da25c14d3)







