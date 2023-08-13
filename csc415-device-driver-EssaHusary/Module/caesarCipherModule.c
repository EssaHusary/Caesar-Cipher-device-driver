/**************************************************************
* Class:  CSC-415-03 Spring 2023
* Name: Essa Husary
* Student ID: 917014896
* GitHub UserID: EssaHusary
* Project: Assignment 6 – Device Driver
*
* File: caesarCipherModule.c
*
* Description: In this file, I am writing a device driver that performs 
*              a caesar cipher on a message provided by the user. Depending
*              on the command provided in ioctl, we will either encrypt
*              or decrypt the provided message, or, set the key to a value
*              other than the default value. 
*
**************************************************************/




#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/vmalloc.h>
#include <linux/string.h>




#include <linux/sched.h>
// My major version number
#define MAJOR_VERSION_NO 333
// My minor version number
#define MINOR_VERSION_NO 3
// The name of my device driver
#define NAME_OF_DEVICE "CaesarCipher"



// To store how many letters are in the English alphabet for the key
#define LETTERS_IN_ENGLISH_ALPHABET 26
// To store the ASCII value of 'A'
#define ASCII_CAPITAL_A 65
// To store the ASCII value of 'Z'
#define ASCII_CAPITAL_Z 90
// To store the ASCII value of 'a'
#define ASCII_LOWERCASE_A 97
// To store the ASCII value of 'z'
#define ASCII_LOWERCASE_Z 122
/* To store the difference between a lowercase ASCII letter and its uppercase 
   equivalent */
#define ASCII_CAPITAL_LOW_DIFFERENCE 32
// To limit the length of the user's message to 1MB
#define MAX_MESSAGE_LENGTH 1048576
// The first supported command, encryption
#define SUPPORTED_COMMAND_1 3
// The second supported command, decryption
#define SUPPORTED_COMMAND_2 4
// The third supported command, setting the key to a new value
#define SUPPORTED_COMMAND_3 5
// The default key value (so that setting the key is optional)
#define DEFAULT_KEY_VALUE 4




int majorNo, minorNo;
char *kernel_buffer;

struct cdev new_cdev;


MODULE_AUTHOR("Essa Husary");
MODULE_DESCRIPTION("A Caesar cipher program");
MODULE_LICENSE("GPL");





/* A data structure to store the message the user wants either encrypted or decrypted,
   "providedMessage", the returned message, "returnedMessage", which has either been
   encrypted or decrypted, the key for encryption/decryption, and the length of the
   providedMessage */
struct cipherDataStructure {
    
    // To store the user's message
    char providedMessage[MAX_MESSAGE_LENGTH];
    // To store the message that has now been either encrypted or decrypted 
    char returnedMessage[MAX_MESSAGE_LENGTH];
    // To store the key to use for either the encryption or decryption
    int key;
    /* To store the length of the provided message. I have this in my data 
       structure so that when we read back the encrypted/decrypted message,
       we only have the returned message and not the other data contained in the
       buffer. This will make more sense when you look at myRead() */
    int lengthOfProvidedMessage;
} cds;



// A custom function to capitalize English letters
void capitalize(char givenMessage[]){

    // A counter for the below 'for' loop
    int i;

    // To loop through the message and find letters to capitalize
    for (i = 0; i < strlen(givenMessage); i++){
        
        /* To check if a letter is in between lowercase a-z. If so, capitalize it
           by subtracting 32 to reach its ASCII uppercase equivalent */
        if ((givenMessage[i] >= ASCII_LOWERCASE_A && givenMessage[i] <= ASCII_LOWERCASE_Z)){
            givenMessage[i] = givenMessage[i] - ASCII_CAPITAL_LOW_DIFFERENCE;
        } 
        // Otherwise, just move on
        else {
            continue;
        }
    }

}



// A function to encrypt a user's message
int encrypt(char givenMessage[], char returnedMessage[], int key){


    // A counter for 'for' loop seen below
    int i;

    // A variable that will eventually store an encrypted letter
    int encryptedLetter;

    // A variable that will eventually store an ASCII number/char from the givenMessage array
    int ASCIInumber;

    /* A variable to store the difference (in ASCII value) between the grabbed ASCII
       value, "ASCIInumber", and the ASCII value of capital A */
    int differenceFromTheLetterA = 0;


    /* A variable to store the difference between the currentKey and the above 
       difference. The purpose is so that if we're going below the ASCII 
       capital A, we can "teleport", so to speak, to ASCII capital
       Z using this "newDifference", and continue descending to a new encrypted
       letter below capital Z */
    int newDifference = 0;
    
    // To store the key to use for the Caesar Cipher
    int currentKey = key;

    // To capitalize the given/provided message
    capitalize(givenMessage);


    // To iterate through the user's message and encrypt each char of the C string
    for (i = 0; i < strlen(givenMessage); i++) {

        // To store the ASCII number of the current character
        ASCIInumber = givenMessage[i];

        /* To check if we've encountered anything other than a (capital) letter
           If so, continue */
        if ((ASCIInumber < ASCII_CAPITAL_A || ASCIInumber > ASCII_CAPITAL_Z)){
            
            returnedMessage[i] = givenMessage[i];
            continue;
        } 
        
        
        // To store the soon-to-be encrypted letter
        encryptedLetter = ASCIInumber - currentKey;

        // To check if we need to teleport to ASCII value 90, or capital Z 
        if (encryptedLetter < ASCII_CAPITAL_A){
            
            differenceFromTheLetterA = ASCIInumber - ASCII_CAPITAL_A;
            newDifference = currentKey - differenceFromTheLetterA;
            encryptedLetter = ASCII_CAPITAL_Z - newDifference + 1;
        }

        // To store the newly encrypted letter into "returnedMessage"
        returnedMessage[i] = encryptedLetter;

    }

    // To return success
    return 0;

}



// A function to decrypt a user's message
int decrypt(char givenMessage[], char returnedMessage[], int key){


    // A counter for the 'for' loop seen below
    int i;

    // A variable that will eventually store a decrypted letter
    int decryptedLetter;

    // A variable that will eventually store an ASCII number/char from the givenMessage array
    int ASCIInumber;

    /* A variable to store the difference (in ASCII value) between the ASCII
       value of capital Z and the grabbed ASCII value, "ASCIInumber" */
    int differenceFromTheLetterZ = 0;

    
    /* A variable to store the difference between the currentKey and the above 
       difference. The purpose is so that if we're going above the ASCII 
       capital Z, we can "teleport", so to speak, to ASCII capital
       A using this "newDifference", and continue ascending to a new decrypted
       letter above capital A */
    int newDifference = 0;
    
    // To store the key to use for the Caesar Cipher
    int currentKey = key;

    // To capitalize the given/provided message
    capitalize(givenMessage);


    // To iterate through the user's message and decrypt each char of the C string
    for (i = 0; i < strlen(givenMessage); i++) {

        // To store the ASCII number of the current character
        ASCIInumber = givenMessage[i];

        /* To check if we've encountered anything other than a (capital) letter
           If so, continue */
        if ((ASCIInumber < ASCII_CAPITAL_A || ASCIInumber > ASCII_CAPITAL_Z)){
            
            returnedMessage[i] = givenMessage[i];
            continue;
        }


        // To store the soon-to-be decrypted letter
        decryptedLetter = ASCIInumber + currentKey;

        // To check if we need to teleport to ASCII value 65, or capital A
        if (decryptedLetter > ASCII_CAPITAL_Z){
            
            differenceFromTheLetterZ = ASCII_CAPITAL_Z - ASCIInumber;
            newDifference = currentKey - differenceFromTheLetterZ;
            decryptedLetter = ASCII_CAPITAL_A + newDifference - 1;
        }

        // To store the newly decrypted letter into "returnedMessage"
        returnedMessage[i] = decryptedLetter;

    }

    // To return success
    return 0;

}



/* A write function to write into the "providedMessage" field of the above data structure
   the user's message */
static ssize_t myWrite(struct file *fs, const char __user *userBuffer, size_t size, loff_t *offset){

    struct cipherDataStructure *cds;
    
    // To store the data from private_data
    cds = (struct cipherDataStructure *) fs->private_data;

    // To check if proper input was provided by the user
    if (userBuffer == NULL){

        return (-1);

    }

    /* To store the length of the user's message */
    cds->lengthOfProvidedMessage = strlen(userBuffer);
    
    /* To copy the message from the user's buffer into the "providedMessage" field 
       of the cipherDataStructure */
    strcpy(cds->providedMessage, userBuffer);

    // To print a message to the kernel log
    printk(KERN_INFO "You wrote the message: %s\n", cds->providedMessage);
    
    return size;
}



/* A read function to read into the user's buffer the field "returnedMessage" of 
    the above data structure, that was filled in by either "enrypt" or 
    "decrypt" */
static ssize_t myRead(struct file *fs, char __user *userBuffer, size_t size, loff_t *offset){


    struct cipherDataStructure *cds;

    // To capture the data from private_data
    cds = (struct cipherDataStructure *) fs->private_data;


    /* To append the null terminator to the end of the returned message as we only
       want that returned message and not what's already contained in the buffer */
    cds->returnedMessage[cds->lengthOfProvidedMessage] = '\0';


    /* To copy the decrypted or encrypted message from the "returnedMessage" field 
       of the cipherDataStructure into the user's buffer */
    strcpy(userBuffer, cds->returnedMessage);


    // To print a message to the kernel log
    printk(KERN_INFO "You got back the message: %s\n", userBuffer);

    return 0;
}



// A function to open the device file to allow the device driver to do work
static int myOpen(struct inode *inode, struct file *fs){

    struct cipherDataStructure *cds;

    // To allocate space for the cipherDataStructure struct
    cds = vmalloc(sizeof(struct cipherDataStructure));

    // To error check whether the allocation succeeded or not
    if (cds == 0){
        printk(KERN_ERR "Error allocating space!\n");
        return -1;
    }

    /* To set the length of the provided message to 0 */
    cds->lengthOfProvidedMessage = 0;
    
    // To set the key to use for encryption/decryption to a default value of 4
    cds->key = DEFAULT_KEY_VALUE;

    // To have the private_data field point to the newly allocated cds space
    fs->private_data = cds;
    return 0;

}



// To close the device file
static int myClose(struct inode *inode, struct file *fs){

    struct cipherDataStructure *cds;

    // To have this new instance point to "private_data"
    cds = (struct cipherDataStructure *) fs->private_data;

    // To free the allocated space pointed to by "private_data"
    vfree(cds);

    // To return success
    return 0;

}



// A function to allow the user to perform either encryption or decryption
static long myIoCtl(struct file *fs, unsigned int command, unsigned long desiredValue){

    
    struct cipherDataStructure *cds;
    
    // A variable to store the return value of either "encrypt()" or "decrypt()"
    int ret = 0;

    // A variable to eventually get the key to use for either encryption or decryption
    int key;

    // A variable to store the user's command
    int userCommand = command;

    // A variable to store the value that will allow us to get the key (via modulo)
    int value = (int) desiredValue;

    // To check if the desired key is negative
    if (value < 0){
        printk(KERN_ERR "Cannot process request for the value %ld. Must be a ", desiredValue);
        printk(KERN_CONT "positive number!\n");
        return (-1);
    }

    // To get the key to use for either encryption or decryption 
    key = value % LETTERS_IN_ENGLISH_ALPHABET;

    // To have the instance point to the same address as private_data
    cds = (struct cipherDataStructure *) fs->private_data;



    //  This command encrypts the "providedMessage"
    if (userCommand == SUPPORTED_COMMAND_1){  

        
        // To encrypt the message provided by the user
        ret = encrypt(cds->providedMessage, cds->returnedMessage, cds->key);

        // To check if encryption was successful. If not, return error
        if (ret < 0){
            printk(KERN_ERR "Error! Invalid message input! Returned: %d\n", ret);
            return (-1);
        }

        printk(KERN_INFO "Entered the 'if' statement for command %d\n", userCommand);
        printk(KERN_INFO "The provided message is: %s\n", cds->providedMessage);
        printk(KERN_INFO "The encrypted message is: %s\n", cds->returnedMessage);



    //  This command decrypts the "providedMessage"
    } else if (userCommand == SUPPORTED_COMMAND_2){  
        
        
        // To decrypt the message provided by the user
        ret = decrypt(cds->providedMessage, cds->returnedMessage, cds->key);

        // To check if decryption was successful. If not, return error
        if (ret < 0){
            printk(KERN_ERR "Error! Invalid message input! Returned: %d\n", ret);
            return (-1);
        }


        printk(KERN_INFO "Entered the 'if' statement for command %d\n", userCommand);
        printk(KERN_INFO "The provided message is: %s\n", cds->providedMessage);
        printk(KERN_INFO "The decrypted message is: %s\n", cds->returnedMessage);



    //  This command sets the key to a new value
    } else if (userCommand == SUPPORTED_COMMAND_3){  


        printk(KERN_INFO "Entered the 'if' statement for command %d\n", userCommand);
        printk(KERN_INFO "The old key was: %d\n", cds->key);


        // To set the key to a new value for the user
        cds->key = key;


        printk(KERN_INFO "The new key is: %d\n", cds->key);
        


    } else {

        printk(KERN_ERR "Invalid command! Command must be either 3, 4, or 5!\n");
        return (-1);
    
    }
    
    

    // To return success
    return 0;

}



// To reassign the linux file ops to our device driver's file functions
struct file_operations fops = {

    .open = myOpen,
    .release = myClose,
    .write = myWrite,
    .read = myRead,
    .unlocked_ioctl = myIoCtl,
    .owner = THIS_MODULE,

};



// To begin initializing and registering the device driver to the kernel
int init_module(void){

    // To store the return value of cdev_add
    int returnVal = 0;
    // To store the return value of the chardev registration
    int registrationNum = 0;
    // To store the device driver number, the result of MKDEV
    dev_t devNumber;

    // To make the device driver number
    devNumber = MKDEV(MAJOR_VERSION_NO, MINOR_VERSION_NO);

    // To register the driver
    registrationNum = register_chrdev_region(devNumber, 1, NAME_OF_DEVICE);

    // To print a success message to the kernel log
    printk(KERN_INFO "Device registration successful: %d", registrationNum);
    // To initialize the driver with its file operations
    cdev_init(&new_cdev, &fops);
    new_cdev.owner = THIS_MODULE;

    // To add the new chardev into the system
    returnVal = cdev_add(&new_cdev, devNumber, 1);
    printk(KERN_INFO "Succeeded adding the new chardev! %d", returnVal);
    printk(KERN_INFO "Welcome to my Caesar Cipher device driver!");

    // To check if the addition failed
    if (returnVal < 0){
        printk(KERN_ERR "Failed to add the chardev!%d\n", returnVal);
    }

    // To return the status of cdev_add
    return returnVal;


}



// To unregister and remove the device driver
void cleanup_module(void){

    dev_t devNumber;
    devNumber = MKDEV(MAJOR_VERSION_NO, MINOR_VERSION_NO);

    // To unregister the device driver
    unregister_chrdev_region(devNumber, 1);

    // To delete the driver
    cdev_del(&new_cdev);

    printk(KERN_INFO "Unloading! Thanks for using my device driver! Hope you had fun!\n");

}