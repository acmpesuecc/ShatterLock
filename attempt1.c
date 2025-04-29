//Needs:
//Encryption and decryption functions. text goes in, text goes out. 
// function to make and operate on text document or dat document
//signup, read, edit and delete functions
//function to split up encrypted text with correct prefixes and make packets.
// till shashi gets here, a function to distribute packets among that folder
//similarly to piece packets back

#include <stdio.h>
#include <string.h>
#include <math.h>

void inputstring(char *what, char *input_to_this_string){
    printf("Enter %s:",what);
    scanf("%s",input_to_this_string);
}

int makekey(char *usn, char *pwd, int *keystream_out){
    for(int i=0;i<((strlen(pwd)<strlen(usn))?strlen(pwd):strlen(usn));i++){
        keystream_out[i]=((int)usn[i]*(int)pwd[i])%26;
    }
    return ((strlen(pwd)<strlen(usn))?strlen(pwd):strlen(usn));
}

void makepackets(char *ciphertext, char packets_out[][26]){
    int j=0;
    char temp[26];
    for(int i=0;i<strlen(ciphertext);i++){
        temp[i%25]=ciphertext[i];
        if(i%25==24){
            temp[25]='\0';
            strcpy(packets_out[j++],temp);
        }
    }
    if (strlen(ciphertext) % 25 != 0){
        temp[strlen(temp)]='\0';
        strcpy(packets_out[j],temp);
    }
}

void encrypt(char *plaintext, char *ciphertext_out, int *keystream, int len){ //this is now a ceaser cipher btw.
    for(int i=0;i<strlen(plaintext);i++){
        ciphertext_out[i]=(char)(((((int)plaintext[i])+keystream[i%len])+26-97)%26)+97;
    }
    ciphertext_out[strlen(plaintext)]='\0'; //null_terminating
}

void decrypt(char *ciphertext, char *plaintext_out, int *keystream, int len){ 
    for(int i=0;i<strlen(ciphertext);i++){
        plaintext_out[i]=(char)(((((int)ciphertext[i])-keystream[i%len])+26-97)%26)+97;
    }
    plaintext_out[strlen(ciphertext)]='\0'; //null_terminating
}

int main(){
    char plaintext[100];
    char ciphertext[100];
    char username[100];
    char password[100];
    int keystream[100];
    inputstring("Plaintext",plaintext);
    inputstring("Username",username);
    inputstring("Password",password);

    int len_of_key=makekey(username,password,keystream);

    encrypt(plaintext,ciphertext,keystream,len_of_key);

    char packets[(int)ceil(strlen(ciphertext)/25.0)][26];
    makepackets(ciphertext,packets);

    
    printf("%s\n",ciphertext);
    decrypt(ciphertext,plaintext,keystream,len_of_key);
    printf("%s\n",plaintext);

    
    for(int i=0;i<sizeof(packets)/sizeof(packets[0]);i++){printf("%s",packets[i]);}
    printf("\n%s\n",ciphertext);

    return 0;
}