/**************************************************************
* Class:  CSC-415-03 Spring 2023
* Name: Essa Husary
* Student ID: 917014896
* GitHub UserID: EssaHusary
* Project: Assignment 6 – Device Driver
*
* File: husary_essa_HW6_main.c
*
* Description: This is the main program that tests my caesar cipher device 
*              driver (where main() is located).
*
**************************************************************/



#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <string.h>
#include <stdbool.h>
#include <errno.h>



// To limit the number of characters in the user's message to 1 MB
#define MAX_MESSAGE_LENGTH 1048576
// To limit the number of digits for a user's command
#define MAX_COMMAND_LENGTH 6
// To limit the number of digits for a user's key
#define MAX_KEY_LENGTH 6
// The first supported command, encryption
#define SUPPORTED_COMMAND_1 3
// The second supported command, decryption
#define SUPPORTED_COMMAND_2 4
// The third supported command, setting the key to a new value
#define SUPPORTED_COMMAND_3 5
// The command to exit the program
#define EXIT_COMMAND 0
// The default key value (so that setting the key is optional)
#define DEFAULT_KEY_VALUE 4





/* A function to handle user input. This function will do one of two things: either
   1) truncate the user's message/buffer and clear the input buffer if the user's
   message is too long, or 2) remove the newline character, '\n', from the end of
   the input so that output makes sense. NOTE: This function was taken from MY assignment
   3, Simple Shell, submission, so if you notice any similarities, now you know why */
void handleInput(char buffer[]){

    /* To check whether the newline character is present in the user's buffer. If 
       not, which this 'if' statement checks, use the truncated version of the input
       and clear the input buffer */
    if (strchr(buffer, '\n') == NULL){
        
        printf("\nYour input is too long. Truncating...\n\n");
        int c;
        while(true){

            c = getchar();
            if (c == '\n' || c == EOF){
                
                break;
            }
            
        }
    
    // Otherwise, remove the newline character to have valid input
    } else {

        for (int i = 0; i < strlen(buffer); i++){

            if (buffer[i] == '\n'){
                buffer[i] = '\0';
            }

        }

    }
    

}







int main(int argc, char *argv[]){


    
    
    int fileDescriptor = 0;
    

    // A buffer to store the user's message they want either encrypted or decrypted
    char userBuffer[MAX_MESSAGE_LENGTH];
    // A buffer to store the returned message that has been either encrypted or decrypted
    char newBuffer[MAX_MESSAGE_LENGTH];
    // A buffer to store the user's desired command
    char userCommand[MAX_COMMAND_LENGTH];
    // A buffer to store the user's desired key if they want to set it
    char userKey[MAX_KEY_LENGTH];

    
    // To store the key of the caesar cipher
    unsigned long key = 0;
    // To store the inputted key just for error checking purposes
    int temporaryKey = 0;
    // To store the command that the user wants to enter
    int commandNo = 0;
    // To store the return value of write() for the sake of error checking
    int retValueOfWrite = 0;
    // To store the return value of ioctl() for the sake of error checking
    int retValueOfIOCTL = 0;
    // To store the return value of read() for the sake of error checking
    int retValueOfRead = 0;
    // To store the return value of fgets
    char *retValueOfFgets;
    

    
    



    // To open the device file and to store the associated file descriptor
    fileDescriptor = open("/dev/CaesarCipher", O_RDWR);
    printf("\nReturned file descriptor value: %d\n", fileDescriptor);

    // To check if the process of opening the device file was successful
    if (fileDescriptor < 0){

        printf("Could not open the device driver\n");
        perror("Error details pertaining to opening the driver: ");
        printf("\n");

        return (-1);
    
    } else {

        printf("Successfully opened the device file!\n\n");

    }







    while (true){

        
        // A pointer to use in strtol() (around 11 lines down)
        char *ptr;
        
        // To prompt the user for a command
        printf("\nPlease enter a command number, where %d is encryption, %d is ", 
               SUPPORTED_COMMAND_1, SUPPORTED_COMMAND_2); 
        printf("decryption, and %d is setting the key (the default value of the key ", 
               SUPPORTED_COMMAND_3);
        printf("is %d). Type either %d or a non-number to exit: ", 
               DEFAULT_KEY_VALUE, EXIT_COMMAND);
        
        // To get the desired command from the user
        fgets(userCommand, MAX_COMMAND_LENGTH, stdin);
        
        /* To check if there was an error getting input (taken from MY assignment
           3 submission) */
        if (retValueOfFgets == NULL) {
            
            if (ferror(stdin)){
                
                printf("\nAn error has occured while reading input." "Exiting...\n");
                clearerr(stdin);
                exit(errno);
            
            }            
        }
        
        // To handle the input. (see my function before main() for more info)
        handleInput(userCommand);
        
        // To convert the input into an int
        commandNo = strtol(userCommand, &ptr, 10);


        // To check if the user wants to exit the test program
        if (commandNo == 0){
            break;
        }





        // To check if the user wants to set the key to a new value
        if (commandNo == SUPPORTED_COMMAND_3){



            // To prompt the user to set a new key
            printf("\n\nPlease now set the key to your desired value: ");
            // To get the new key from the user
            retValueOfFgets = fgets(userKey, MAX_KEY_LENGTH, stdin);
            /* To check if there was an error getting input (taken from MY assignment
               3 submission) */
            if (retValueOfFgets == NULL) {
            
                if (ferror(stdin)){
                
                    printf("\nAn error has occured while reading input." "Exiting...\n");
                    clearerr(stdin);
                    exit(errno);

                }            
            }
            // To handle the input. (see my function before main() for more info)
            handleInput(userKey);
            // To convert the input into an int
            temporaryKey = strtol(userKey, &ptr, 10);
            /* To check if we have a negative number, continue to the next 
               iteraton (non-number chars will be treated as 0s)*/
            if (temporaryKey < 0){
                printf("Error! You've entered an invalid key! Ensure that "); 
                printf("your input is a positive number!\n\n");
                continue;
            }
            key = temporaryKey;
            printf("New key number: %ld\n", key);

            

            // To process/reset the key the user desires for their future messages
            retValueOfIOCTL = ioctl(fileDescriptor, commandNo, key);
            
            // To check if the io control succeeded
            if (retValueOfIOCTL < 0){

                printf("Error processing request!\n");

                perror("Error details: ");
                printf("\n\n");

                return (-1);

            } else {

                printf("IOCTL operation successful!\n\n");

            }



        // Otherwise, to check if the user wants to encrypt or decrypt a message
        } else if (commandNo == SUPPORTED_COMMAND_1 || 
                   commandNo == SUPPORTED_COMMAND_2){



            // To prompt the user for a message
            printf("\nPlease enter a message you want encrypted or decrypted: ");
            // To get the message from the user
            retValueOfFgets = fgets(userBuffer, MAX_MESSAGE_LENGTH, stdin);
            /* To check if there was an error getting input (taken from MY assignment
               3 submission) */
            if (retValueOfFgets == NULL) {
            
                if (ferror(stdin)){
                
                    printf("\nAn error has occured while reading input." "Exiting...\n");
                    clearerr(stdin);
                    exit(errno);
            
                }            
            } 

            // To handle the input. (see my function before main() for more info)
            handleInput(userBuffer); 


            
            // To write the user's message into their buffer
            retValueOfWrite = write(fileDescriptor, userBuffer, MAX_MESSAGE_LENGTH);
            
            // To check if the write succeeded
            if (retValueOfWrite < 0){

                perror("Error providing input. Here are the details: "); 
                printf("Closing...\n");

                return (-1);
            }



            /* To process either the encryption or decryption the user desires 
               for their message */
            retValueOfIOCTL = ioctl(fileDescriptor, commandNo, key);
            
            // To check if the io control succeeded
            if (retValueOfIOCTL < 0){

                printf("Error! You entered invalid input!\n"); 
                perror("Error details: ");
                printf("\n\n");

                continue;

            } else {

                printf("\nIOCTL operation successful!\n\n");

            }



            /* To read the user's now encrypted/decrypted message. Alternatively, we can
               think of this as "writing" the returned message into the new buffer. */
            retValueOfRead = read(fileDescriptor, newBuffer, MAX_MESSAGE_LENGTH);
            
            // To check if the read succeeded
            if (retValueOfRead < 0){

                perror("\nError reading input. Here are the details: "); 
                printf("Closing...\n\n");

                return (-1);
            }

            printf("Return value of read: %d\n", retValueOfRead);
            

            printf("Your returned message is the following: %s\n\n", newBuffer);
            


        }


    }





    // To close the file associated with the file descriptor
    close(fileDescriptor);


    return 0;
}