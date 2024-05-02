/* Filename:	logAndReg.c
   Author:	Sam & Jake
   Date:	4/18/24
   Description:	A registration and login system with built in encryption and decryption that can be used for many applications.
   		In this application, onced loged in a user can translate a word to spanish.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <curl/curl.h>

#define MAX_STRING 100
#define ARGS_NUM 2

struct memoryStruct
{
    char* memory;
    size_t size;
};

static size_t writeMemoryCallback(void* contents, size_t size, size_t nmemb, void* userp);
void encrypt(char* user, char* pass);
void decrypt(char* user, char* pass);
void registration(char* user, char* pass, char *line);
void login(char* user, char* pass, char *line);
void trim(char *str, char *dest);
void logout(char* user, char* pass);


int main(void) {

    /*holds the command the user has inputed before it has been trimmed*/
    char *command = malloc(MAX_STRING * sizeof(char));

    /*Holds the username that will be inputed by the user*/
    char *username = malloc(MAX_STRING * sizeof(char));

    /*Holds the password that will be inputed by the user*/
    char *password = malloc(MAX_STRING * sizeof(char));

    /*will hold the command after it is ran through the trim function*/
    char *commandTrim = malloc(MAX_STRING * sizeof(char));

    /*holds the lines that will be read from the userInfo*/
    char *line = malloc(MAX_STRING * sizeof(char));
    
    /*set username pointer contents to NUL*/
    *username = '\0';

    /* print a welcome message */
    printf("Welcome! Type \"help\" for more information.\n");

    while(1) {

        /*checks and Lets the user know if they are logged in or not*/
        if(*username == '\0') {
            printf("Not Logged in:\n");
        } else {
            printf("Logged in: %s\n", username);
        }

        printf("$ ");

        /* Fill the commandTrim with null chars. */
        memset(commandTrim, '\0', sizeof(char)*MAX_STRING);

		/*use fgets to get stdin from the user, also check to make sure it is not null*/
	    if(fgets(command, MAX_STRING, stdin)==NULL) {
           	printf("Error processing line.\n");
			break;
        } else if(*command == '\n') {
            continue;
        } else {
            /* run the user inputed command through the trim function*/
            trim(command, commandTrim);
        }

        /*use strcmp to check if the entered command is help*/
        if(strcmp(commandTrim, "help")==0) {
            printf("This is the help menu. List of valid commands are:\n help - display this menu.\n quit - exit the program.\n register - register an account with us.\n login - login to your preexiting account.\n logout- logout of your account.\n\n");
        }

		/*use strcmp to check if the entered command is quit*/
        else if(strcmp(commandTrim, "quit")==0) {
            break;
        }

        /*use strcmp to check if the entered command is register*/
        else if(strcmp(commandTrim, "register")==0) {
            registration(username, password, line);
        }

        /*use strcmp to check if the entered command is login*/
        else if(strcmp(commandTrim, "login")==0) {
	        login(username, password, line);
	 	  
            if (strcmp(username, "\0") != 0) {
            /* if the login was successfull, prompt the user for a word to translate */
            char* inputWord = malloc(100 * sizeof(char));
            printf("\nEnter a word to translate to spanish:\n");	
            fgets(inputWord, MAX_STRING, stdin);
            
            /* get the length of the input and trim it */    
            int length = strlen(inputWord);
            char* trimInputWord = malloc(length * sizeof(char));
            trim(inputWord, trimInputWord);
            
            /* prepare the queryString for the API call */	
            char* queryString = malloc((length + 50) * sizeof(char));
            sprintf(queryString, "from=auto&to=es&text=%s", trimInputWord);
        
            /* credit goes to youtube user PortfolioCourses and rapidapi.com for this section - end is marked by **** */
            CURL* curl;
            CURLcode result;

            struct memoryStruct chunk;
            chunk.memory = malloc(1);
            chunk.size = 0;
                
            curl = curl_easy_init();
            if (curl == NULL) {
                printf("Error. Request failed.\n");
                return 1;
            }

            curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "POST");
            curl_easy_setopt(curl, CURLOPT_URL, "https://google-translate113.p.rapidapi.com/api/v1/translator/text");

            struct curl_slist *headers = NULL;
            headers = curl_slist_append(headers, "content-type: application/x-www-form-urlencoded");
            headers = curl_slist_append(headers, "X-RapidAPI-Key: a188e25f7bmshc454a613d1ead7cp119a5djsnd5f6a4780c37");
            headers = curl_slist_append(headers, "X-RapidAPI-Host: google-translate113.p.rapidapi.com");
            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeMemoryCallback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)&chunk);
            
            /* set curl to use the queryString we created earlier */
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, queryString);

            result = curl_easy_perform(curl);
            if (result != CURLE_OK) {
                printf("Error: %s\n", curl_easy_strerror(result));
                return 1;
            }

            /***************************************************************************************************************/
                
            /* create a char* array and int to hold the results and count */
            char** resultArr = malloc(200 * sizeof(char*));
            int count = 0;

            /* parse the result and tokenize it on double quotes to get the keys and values */
            int i = 0;
            char* token = strtok(chunk.memory, "\"");
            while (token != NULL) {
                resultArr[i] = malloc((strlen(token) + 1) * sizeof(char));
                count++;
                strcpy(resultArr[i], token);
                token = strtok(NULL, "\"");
                i++;
            }
            
            /* output the result */
            printf("%s translated to spanish is: %s\n", trimInputWord, resultArr[3]);
        
            /* free allocated memory */
            for (i = 0; i < count; i++) {
                    free(resultArr[i]);
            }
            free(inputWord);
            free(trimInputWord);
            free(queryString);
            free(chunk.memory);
            curl_easy_cleanup(curl);	
                break;
            }
        }

        /*use strcmp to check if the entered command is logout*/
        else if (strcmp(commandTrim, "logout")==0)
        {
           
	       	logout(username, password);
        
	}
    }

    /*free up all the malloced space on the heap*/
    free(line);
    free(username);
    free(password);
    free(commandTrim);
    free(command);

    return 0;
}

/* credit goes to youtube user PortfolioCourses for this function - it is used in order to store the API call as a string */
static size_t writeMemoryCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    size_t realsize = size * nmemb;
    struct memoryStruct *mem = (struct memoryStruct *) userp;

    char *ptr = realloc(mem->memory, mem->size + realsize + 1);
    if (!ptr) 
    {
	printf("Not enough memory! (realloc returned NULL)\n");
	return 0;
    }										        
    mem->memory = ptr;
    memcpy(&(mem->memory[mem->size]), contents, realsize);
    mem->size += realsize;
    mem->memory[mem->size] = 0;

    return realsize;
}

/*
registration- allows the user to make an account using a username and password and saves it in the userInfo file.
inputs: user -  is a pointer to an address that holds the username string
	    pass - is a pointer to an address that holds the password string
        line - holds a pointer to where the line that is read and written in from the userInfo file will be.
*/
void registration(char* user, char* pass, char *line) {

    /*holds the tokenized username and password read in from the file*/
    char *userAndPass[ARGS_NUM];
    
    /* variable used to stop while loop when set to 1*/
    int stop = 0;

    /*open userInfo file*/
    FILE *infoRead;
    infoRead=fopen("userInfo.txt", "r");
    if(infoRead==NULL) {
        printf("Error has occured opening userInfo file");
        exit(EXIT_FAILURE);
    }

    /* ask the user for a username and password*/
    /* while loop are to remove any extra characters in the buffer*/
    printf("Thank you! for choosing to register with us.\n");
    printf("Please enter a username\n");
    scanf("%s", user);
    int a; while ((a = getchar()) != EOF && a != '\n'); /* Credit to  Stackoverflow user Jonathan Leffler for insight into this peice of code*/
    trim(user, user);
    printf("Please enter a password\n");
    scanf("%s", pass);
    int b; while ((b = getchar()) != EOF && b != '\n'); /* Credit to  Stackoverflow user Jonathan Leffler for insight into this peice of code*/
    trim(pass, pass);

    /*encrypt the entered username and password*/
    encrypt(user,pass);

    while(stop == 0) {

        /*read userInfo file line by line*/
        if(fgets(line, MAX_STRING, infoRead)==NULL) {

            /* check if we are at the end of the file.*/
            if(feof(infoRead)) {
                stop = 1;
			} else {
                printf("Error has occured opening userInfo file");
                exit(EXIT_FAILURE);
            }
        } else {
            /*holds the tokenized strings from the userInfo file*/
            userAndPass[0] = strtok(line, " \t\n");
            userAndPass[1] = strtok(NULL, " \t\n");
			
            /*compare the username and password entered to the usernames and passwords in the userInfo file*/
            if((strcmp(user, userAndPass[0])==0 || strcmp(pass, userAndPass[1])==0)) {
                printf("The username or password you entered has already been taken\n");
                *user = '\0';
                *pass = '\0';
                fclose(infoRead);
                return;
            }
        }
    }
    fclose(infoRead);

    /*open userinfo file with append permissions*/
    FILE *infoWrite;
    infoWrite=fopen("userInfo.txt", "a");
    if(infoWrite==NULL) {
        printf("Error has occured opening userInfo file");
        exit(EXIT_FAILURE);
    }

    /*write username and password into the last line of the file*/
    fprintf(infoWrite, "%s %s \n", user, pass);

    printf("The account has been created\n");

    /*decode username and password, so the username appears in the program for the user to see*/
    decrypt(user, pass);

    fclose(infoWrite);
}

/*
login- allows the user to login to account using username and password stored in userInfo text file.
inputs: user -  is a pointer to an address that holds the username string
	    pass - is a pointer to an address that holds the password string
        line - holds a pointer to where the line that is read in from the userInfo file will be.
*/
void login(char* user, char* pass,char *line) {

    /*holds the tokenized username and password read in from the file*/
    char *userAndPass[ARGS_NUM];

    /*open userInfo file*/
    FILE *infoRead;
    infoRead=fopen("userInfo.txt", "r");
    if(infoRead==NULL) {
        printf("Error has occured opening userInfo file");
        exit(EXIT_FAILURE);
    }

    /* ask the user for a username and password*/
    /* while loop are to remove any extra characters in the buffer*/
    printf("Please login in.\n");
    printf("Please enter your username\n");
    scanf("%s", user);
    int c; while ((c = getchar()) != EOF && c != '\n'); /* Credit to  Stackoverflow user Jonathan Leffler for insight into this peice of code*/
    trim(user, user);
    printf("Please enter your password\n");
    scanf("%s", pass);
    int d; while ((d = getchar()) != EOF && d != '\n'); /* Credit to  Stackoverflow user Jonathan Leffler for insight into this peice of code*/
    trim(pass, pass);

    /*encrypt the entered username and password*/
    encrypt(user,pass);

    while(1) {
        /*read userInfo file line by line*/
        if(fgets(line, MAX_STRING, infoRead)==NULL) {

            /* check if we are at the end of the file.*/
            if(feof(infoRead)) {
                fclose(infoRead);
                *user = '\0';
                *pass = '\0';
                printf("There is no account with those credentials. Make sure you typed both your username and password correctly\n");
                return;
            } else {
                printf("Error has occured opening userInfo file");
                exit(EXIT_FAILURE);
            }
        } else {
            /*holds the tokenized strings from the userInfo file*/
            userAndPass[0] = strtok(line, " \t\n");
            userAndPass[1] = strtok(NULL, " \t\n");

            /*compare the username and password entered to the usernames and passwords in the userInfo file*/    
            if((strcmp(user, userAndPass[0])==0 && strcmp(pass, userAndPass[1])==0)) {

                printf("You have logged in succesfully!\n");

                /*decode username and password, so the username appears in the program for the user to see*/
                decrypt(user,pass);
                fclose(infoRead);
                return;
            }
        }
    }     
}

/*
logout- allows the user to logout of there current account if they are logged into one.
inputs: user -  is a pointer to an address that holds the username string
	    pass - is a pointer to an address that holds the password string
*/
void logout(char* user, char* pass) {

    if(*user == '\0') {
        printf("Logout Failed: User is not logged into an account.\n");
    } else {
        printf("Logout Success: You have logged out of your account.\n");
        *user = '\0';
        *pass = '\0';
    }
}

/*
encrypt- applies the correct char arthemitic to encode the username and password
inputs: user -  is a pointer to an address that holds the username string
	    pass - is a pointer to an address that holds the password string
*/
void encrypt(char* user, char* pass) {

    int i;
    int i2;

    /*for loop for encoding each char in the username string*/
    for (i=0; (i<MAX_STRING && user[i]!='\0');i++) {

        if(user[i]==' ') {
            continue;
        }
        user[i]=user[i]-3-i;
    }

    /*for loop for encoding each char in the password string*/
    for (i2=0; (i2<MAX_STRING && pass[i2]!='\0');i2++) {

        if(pass[i2]==' ') {
            continue;
        }

        pass[i2]=pass[i2]-3-i2;
    }
}

/*
decrypt- applies the correct char arthemitic to decode the username and password
inputs: user -  is a pointer to an address that holds the username string
	    pass - is a pointer to an address that holds the password string
*/
void decrypt(char* user, char* pass) {

    int i;
    int i2;

    /*for loop for decoding each char in the username string*/
    for (i=0; (i<MAX_STRING && user[i]!='\0');i++) {

        if(user[i]==' ') {
            continue;
        }

        user[i]=user[i]+3+i;
    }

    /*for loop for decoding each char in the password string*/
    for (i2=0; (i2<MAX_STRING && pass[i2]!='\0');i2++) {

        if(pass[i2]==' ') {
            continue;
        }

        pass[i2]=pass[i2]+3+i2;
    }
}

/*
trim- trims off the leading and trailing white spaces of the string provided
inputs: str -  holds the string that is going to be trimmed
	dest - will hold the modified string since str is immutable so it can not be changed
*/
void trim(char *str, char *dest) {
  
	int i = 0;

	while (isspace(str[i])) {
		i++;
	}

	str+=i;

	int i2 = strlen(str)-1;

	while (isspace(str[i2])) {
		i2--;
	}

	char holdstr[MAX_STRING];
	strcpy(holdstr, str);	
  
	strncpy(dest, holdstr, i2+1);
}
