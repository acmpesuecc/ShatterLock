//gonna do the exact same thing as with attempt1.c but with 2 additional security questions only asked at the start 
// to create 2 extra keys that will also be hidden. then use all 3 keystreams to encrypt and decrypt.
// also going to add more junk in the actual packets so that theres more packets. but also add more junk in junk packets so the ratio stays the same.
// i was thinking we could have alternate characters be junk. so that people cant use kasiski test to get the key and stuff.
// the only point of this is to increase the number of packets the attacker needs to fetch and also the number of packets in the system 
// so that its harder t brute force.
//i think we should have a ratio of real files to junk files and try to achieve that at every point.
//challenges i think will arise:
//      how to encrypt using all 3 keys in a strong way?
//      structure of getting key 2 and 3 from password and so on
//      smarter generation of junk (achieving ratios)
//look into qsort()
//TODO: make sure ppl cant get to know length of key 1, 2 or 3. make sure packets stored are always of the same length
//      regardless of length of key.
// TODO: encrypt the metadata and add a secodn layer of encryption to the whole thing. (layers and layered metadata encryption?)
// TODO: once the encryption is better, encrypt it in layers using key 1, 2 and keystream instead of 1 tempkeystream like here.
//in a future version, have it such that one user can have many contents

//TODO: IMP Obscure more. i.e when choosing directories, choose all directories atleast once.
//      so that when attacker sees whic folders were changed when and cant go off of that.
//      cause once attacker can see which folder was changed when, attacker can know which folders to see.
//      so make *some* change in all folders.

//      secondly, make sure theres more packets for the key. somehow use junk letters like z or something
//      cause once attacker knows how many packets are for key2,3, attacker knows how long it is. 
//      lenght in general should be obscured. figure out a way (maybe pad with useless letters and remove later.)
//      Q and Z occur V rarely.


// TODO:
// currently, encrypt is a vigenerre cipher. (a stream cipher)
// now i want to do:
// vigenerre cipher as we are doing now.
// add junk alternating like: ajajajajaja where a is actual character and j is junk character. (remove these while decrptying. is indipendet of the key.)
// then add the metadata.
// make sure it fits into a matrix adding extra "z" characters.
// then we do a hill cipher with key being a square matrix.
// then make the packets using that. (have the packets be a little bigger.)

// for each user, when signup, make anoher file containing list of subdirs taken. i.e
//    char subdirs[100][256];
//    int num_subdirs = get_subdirectories("storage", subdirs, 100); //gets (up to) 100 subdirs from storage.
// now we take this list of subdirs and (take just the characters in an encryptable way)
// then we encrypt and keep the packets just like with contents, key, etc. (encrypt as you encrypt content)
// then we get these back when reading and use it to get subdirs. that way someone creating a new folder in storage wont break the encryption for all the files. (cause currently, it relies on order)

#include <stdio.h> // for printf and scanf
#include <string.h> //for strlen, strcpy, memcpy
#include <math.h> //for ceil
#include <stdlib.h> //for rand functions, malloc, free and exit.
#include <time.h> //for time(0) when randomness needed.

#include <dirent.h> //-shashi //for scanning of subdirectories in linux(/posix) systems. (used in get_subdirectories)
#include <sys/stat.h> //-shashi //lets us access file/directory metadata like permissions and type. (used to differentiate bw directories and files in get_sibdirectories)

void inputstring(char *what, char *input_to_this_string){
    printf("Enter %s:",what);
    scanf("%s",input_to_this_string);
}

void slice(char *sliced_out, char *to_slice, int from, int to){
    int count=0; //we assume size of sliced_out alloted is greater than or equal to (size of the slice)+1
    for(int i=from;i<=to;i++){
        sliced_out[count++]=to_slice[i];
    }
    sliced_out[count]='\0';
}

void writetofile(char *filepath, char *contents) { 

    //just checking for error
    FILE *check = fopen(filepath, "r");
    if (check) {
        fclose(check);
        printf("FILE IS REPEATING ERROR: %s\n", filepath);
        return;
    }
    
    FILE *pF = fopen(filepath, "w"); 
    if (!pF) { //just checking for error
        printf("File open error for %s\n", filepath);
        exit(1);
    }

    //-shashi till here
    
    fprintf(pF, "%s", contents); //puts it in and closes.
    fclose(pF);
}

void readcontents(char *filepath, char *contents_out){ //-many changes by shashi
    
    FILE *pF = fopen(filepath, "r"); //just error handling
    if (!pF) {
        printf("File open error: %s\n", filepath);
        exit(1);
    }

    fgets(contents_out, 26, pF);  // 26-byte packet put in contents out and sent.
    fclose(pF);
}

void deletepackets(char packetpath[][513], int numpackets){
    for(int i=0;i<numpackets;i++){
        if (remove(packetpath[i]) == 0) {
            printf("File deleted successfully.\n");
        } else {
            printf("Error: Unable to delete the file: %s\n",packetpath[i]);
        }
    }
}

//this function fills subdirs array with subdirectories found in parent.
int get_subdirectories(const char *parent, char subdirs_out[][256], int max_subdirs) { //-shashi
    DIR *dir = opendir(parent); 
    if (!dir) {
        perror("opendir failed");
        return 1;
    }
    //above lines open the parent directory, throws error 1 if fail.

    struct dirent *entry; //used to iterte through each object in directory.
    int count = 0; //how many subdirs found
    char path[512]; //full path names
    struct stat statbuf; //is the entry a diretory? 

    while ((entry = readdir(dir)) != NULL && count < max_subdirs) { //reading each object in parent till max_subdirs found.
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0){continue;} //if directory is . or .., skip it. (those are current and parent dirs)

        snprintf(path, sizeof(path), "%s/%s", parent, entry->d_name); //constructs full path
        if (stat(path, &statbuf) == 0 && S_ISDIR(statbuf.st_mode)) { //if metadata and if its a directory, put into subdirs[]
            strncpy(subdirs_out[count], entry->d_name, 255);
            subdirs_out[count][255] = '\0';  // null terminate just in case
            count++; 
        }
    }

    closedir(dir); //closes parent
    return count; //returns number of (useful) subdirs put into subdirs[].
}

void getpaths(char packetpaths_out[][513],char packetnames[][100], int numpacks, int seed){ 
//-shashi from here
    char subdirs[100][256];
    int num_subdirs = get_subdirectories("storage", subdirs, 100); //gets (up to) 100 subdirs from storage.

    //TODO: check and make sure reproducibility.
    //PROBLEM THIS IS NOT REPRODUCIBLE. MAKE SURE IT DOESNT HAPPEN!!
    if (num_subdirs == 0) { //if no subdirectories found. failsafe. consider changing it to make dirs based on key instead of just using default.
        printf("ERROR: No subdirectories found. Creating 'default'...\n");
        mkdir("storage/default");
        strcpy(subdirs[0], "default");
        num_subdirs = 1;
    }

    srand(seed);
    for(int i=0;i<numpacks;i++){//we go through each packet and take the filename and path and combine to put in list of packetpaths
        int subdir_index = rand() % num_subdirs;  //chooses a number and takes subdir[that num]
        sprintf(packetpaths_out[i],"storage/%s/%s.txt", subdirs[subdir_index], packetnames[i]); //prints directory name into filepath   
    }
}

int getuniquetouserseed(int *keystream, int len_of_keystream){ //same thing as before (made by gemini, pls forgive me)
    if (keystream == NULL || len_of_keystream <= 0) {
        return 0;
    }
    long long int sum=0;
    for (int i = 0; i < len_of_keystream; i++) {
        // Add the keystream element. Modulo inside the loop helps prevent overflow
        // if keystream values or length are very large, keeping the sum within long long limits.
        // Using a large prime helps distribute the sum.
        sum = (sum + keystream[i]) % 99991; //99991 is a large prime number.
    }
    sum = (sum + len_of_keystream) % 99991;
    if (sum < 0) {
        sum += 99991; // Add the prime to make it positive
    }
    return ((int)sum);
}

int makekey(char *usn, char *pwd, int *keystream_out){ //here, key depends on the element in front of it.
    //i know this is innefficient, just roll with it for now.
    int len_of_key=(strlen(pwd)<strlen(usn))?strlen(pwd):strlen(usn);
    for(int i=0;i<len_of_key;i++){
        srand((int)usn[i]*(int)pwd[i]);
        keystream_out[i]=(rand())%26;
    }
    for(int i=0;i<(len_of_key-1);i++){
        srand(keystream_out[i]*keystream_out[i+1]); //i know its usually i-1 but i thought why not
        keystream_out[i]=(rand())%26;
    }
    srand(keystream_out[len_of_key-1]*keystream_out[0]);
    keystream_out[len_of_key-1]=rand()%26;
    return (len_of_key);
}

void vigenerre_encrypt(char *plaintext, char *ciphertext_out, int *keystream, int len){ //going with the same encryption as before
    for(int i=0;i<strlen(plaintext);i++){
        ciphertext_out[i]=(char)(((((int)plaintext[i])+keystream[i%len])+26-97)%26)+97;
    }
    ciphertext_out[strlen(plaintext)]='\0'; //null_terminating
}


void vigenerre_decrypt(char *ciphertext, char *plaintext_out, int *keystream, int len){ //same decryption algorithm as before.
    for(int i=0;i<strlen(ciphertext);i++){
        plaintext_out[i]=(char)(((((int)ciphertext[i])-keystream[i%len])+26-97)%26)+97;
    }
    plaintext_out[strlen(ciphertext)]='\0'; //null_terminating
}

// void multiply_matrices(int R1, int m1[R1][26], int key[26][26], char m_out[R1][26]){ //works when R1<26
//     char letters[]="abcdefghijklmnopqrstuvwxyz";
//     int temp[R1][26];
//     for (int i = 0; i < R1; i++) {
//         for (int j = 0; j < 26; j++) {
//             m_out[i][j] = 0;
//             temp[i][j]=0;
//             for (int k = 0; k < 26; k++) {
//                 temp[i][j] += (key[i][k] * m1[k][j])%26;
//             }
//             m_out[i][j]=letters[temp[i][j]%26];
//         }
//     }
// }

//TODO: THIS WILL ONLY WORK FOR NUMPACKS<26 !!
void hill_encrypt(int numpacks, char packets_in[numpacks][26],int hillkey[26][26], char packets_out[numpacks][26]){ //packets_in X hillkey
    //we split up packets_out into blocks of m[R1][26] where R1<26;
    //then perform hill encryption to the block and add it to packets_out.
    // int numpacks_left=numpacks;
    // int count=0;
    // while(numpacks_left>0){ //iterating through the packets 26 at a time
    //     if(numpacks_left>26){ //encrypt a block of 26 packets and add it to packets_out
    //         int packets[26][26];
    //         for(int i=0;i<26;i++){
    //             for(int j=0;j<26;j++){
    //                 packets[i][j]=(((int)packets_in[count][j])+26-97)%26; //taking the number of that character.
    //             }
    //             count++;
    //         }
    //         char temp[26][26];
    //         multiply_matrices(26,packets,hillkey,temp);
    //         int j=0;
    //         for(int i=numpacks-numpacks_left;i<26;i++){
    //             strcpy(packets_out[i],temp[j++]);
    //         }
    //     }
        //else{ //encrypt the numpacks_left into packets_out
            char letters[]="abcdefghijklmnopqrstuvwxyz";
            int packets[numpacks][26];
            for(int i=0;i<numpacks;i++){
                for(int j=0;j<26;j++){
                    packets[i][j]=(((int)packets_in[i][j])+26-97)%26; //taking the number of that character.
                }
            }

            int result[numpacks][26];
            for (int i = 0; i < numpacks; i++) {
                for (int j = 0; j < 26; j++) {
                    result[i][j] = 0;
                    packets_out[i][j]='\0';

                    for (int k = 0; k < 26; k++) {
                        result[i][j] += (packets[i][k] * hillkey[k][j])%26;
                    }
                    packets_out[i][j]=letters[result[i][j]%26];
                }
            }

        //}
    //}
}


void hill_decrypt(){} //TODO: DO THIS ASAP IMP

void insertjunkintostream(char *ciphertext_in, char *ciphertext_out){
    char letters[]="abcdefghijklmnopqrstuvwxyz";
    srand(time(0));
    int len=strlen(ciphertext_in);
    int count=0, j=0;
    for(int i=0;i<len;i++){
            ciphertext_out[count++]=letters[rand()%strlen(letters)]; //junk character
            ciphertext_out[count++]=ciphertext_in[j++]; //real character
    }
    ciphertext_out[count]='\0';
}
void removejunkfromstream(char *ciphertext_in, char *ciphertext_out){  //TODO: IMP might not work. check.
    // takes every other character only.
//please note that this has no cryptographic value. it is only to increase the number of packets.
//this is ovious from the fact that it is not relying on the key.
//TODO: Take a call on wether this is actually useful or if the number of packets dont matter.
    int len=strlen(ciphertext_in);
    int count=0;
    if(len%2!=0){printf("ERROR: len in remove junk is not even!");}
    for(int i=0;i<len;i++){ //len is even
            i++; //junk character.
            ciphertext_out[count++]=ciphertext_in[i]; //real character
    }
    ciphertext_out[count]='\0';
}

int get_gcd(int a, int b) { //gemini made this. pls forgive
    a = (a > 0) ? a : -a; // Ensure numbers are non-negative for GCD calculation
    b = (b > 0) ? b : -b;
    while (b != 0) {
        int temp = b;
        b = a % b;
        a = temp;
    }
    return a;
}

int my_get_gcd(int a, int b)
{
    // Find Minimum of a and b
    int result = ((a < b) ? a : b);
    while (result > 0) {
        if (a % result == 0 && b % result == 0) {
            break;
        }
        result--;
    }
    
    return result; // Return gcd of a and b
}

//gemini made this, pls forgive.
int modInverse(int a, int m) { //Helper function to calculate the modular multiplicative inverse of 'a' modulo 'm' and returns if exists.
    //Uses the Extended Euclidean Algorithm. Returns -1 if the inverse does not exist (i.e., if gcd(a, m) != 1).
    int m0 = m;
    int y = 0, x = 1;

    a = (a % m + m) % m; // Ensure 'a' is within the range [0, m-1]

    // Handle cases where inverse doesn't exist or is trivial
    if (m == 1) return 0; // No inverse mod 1
    if (a == 0) return -1; // Inverse of 0 doesn't exist

    while (a > 1) {  // Extended Euclidean Algorithm Finds x, y such that a*x + m*y = gcd(a, m)
        int q = a / m; // q is quotient
        int t = m;

        m = a % m; //m is remainder now, process same as Euclidean algorithm
        a = t;
        t = y;

        y = x - q * y; //Update y and x
        x = t;
    }
    if (x < 0) {
        x = x + m0; // Ensure x is positive
    }

    // Check if the inverse found is valid (only needed if not guaranteed gcd=1)
    // int gcd_check = get_gcd(a_original, m0); // Not needed here, logic above handles non-coprime
    // if (gcd_check != 1) return -1; // Should not happen if loop terminates correctly

    return x; // x is the modular inverse
}

//gemini made this pls forgive. truly am sorry, just too hard and complex to impliment in code for 26.
int determinantOfMatrixUsingGaussian(int matrix[26][26]) { // Function to calculate the determinant of a 26x26 matrix modulo 26 using gaussian elimination.
    //Returns determinant or 0 if matrix is singular%26 (not invertible)
    int n=26;

    int temp_matrix[26][26]; //Create a local copy to avoid changing the original matrix passed in. We need to perform Gaussian elimination, which modifies the matrix.
    for (int i = 0; i < n; i++) { // Copy the input matrix to the temporary matrix
        for (int j = 0; j < n; j++) {
            temp_matrix[i][j] = (matrix[i][j] % 26 + 26) % 26; // Ensure initial values are 0-25 //just in case.
        }
    }

    int current_determinant = 1; 
    int num_row_swaps = 0;       // Count row swaps to adjust the sign later

    // Perform Forward Elimination to make the matrix upper triangular
    // Iterate through columns (pivot columns)
    for (int j = 0; j < n; j++) { // j is the current column index

        // --- Step 1: Find a pivot for the current column j ---
        int pivot_row = j; // Start looking for a pivot in the current row j
        // Find a row 'i' from 'j' downwards where the element temp_matrix[i][j] is non-zero and ideally coprime to 26 for modular inverse existence.
        while (pivot_row < n && temp_matrix[pivot_row][j] == 0) {
             pivot_row++; // Move to the next row if current element is 0
        }
        
        if (pivot_row == n) { // If no non-zero pivot is found in this column from row j downwards
            //printf("Debug (det func): Returning 0 (Singular: column %d is all zeros)\n", j);
            return 0;  // The matrix is singular modulo 26. Determinant is 0. This column is all zeros from row j downwards.
        }

        // --- Step 2: If the pivot is not in the current row j, swap rows ---
        if (pivot_row != j) {
            for (int k = 0; k < n; k++) { // Swap the current row j with the pivot_row
                int temp = temp_matrix[j][k];
                temp_matrix[j][k] = temp_matrix[pivot_row][k];
                temp_matrix[pivot_row][k] = temp;
            }
            num_row_swaps++; //swap counter++
            // Swapping rows multiplies the determinant by -1 (or 25 mod 26)
        }

        int pivot_element = temp_matrix[j][j]; // The pivot element is now temp_matrix[j][j]

        // --- Step 3: Check if the pivot element is coprime to 26 ---
        // For modular inverse to exist (needed for division in elimination)
        int pivot_gcd=get_gcd(pivot_element, 26);
        if ( pivot_gcd!= 1) {
             // The pivot is not coprime to 26 (divisible by 2 or 13).
             // The matrix is singular modulo 26. Determinant is 0.
             // print this for debugging to see why a matrix is rejected:
              //printf("Debug (det func): Returning 0 (Singular: pivot %d not coprime to 26 at (%d,%d), GCD is %d)\n", pivot_element, j, j, pivot_gcd);
             return 0;
        }

        // --- Step 4: Multiply the determinant by the pivot element ---
        current_determinant = (current_determinant * pivot_element) % 26;

        // --- Step 5: Eliminate elements below the pivot ---
        // Make all elements below the pivot temp_matrix[j][j] equal to zero.
        // Iterate through rows below the current pivot row 'j'
        for (int i = j + 1; i < n; i++) {
            int element_to_eliminate = temp_matrix[i][j]; // The element to make zero

            // If the element is already zero, no operation is needed for this row
            if (element_to_eliminate == 0) {
                continue;
            }

            // Calculate the multiplier 'm' = (element to eliminate) * (pivot's modular inverse) mod 26
            int pivot_inverse = modInverse(pivot_element, 26); // Get the modular inverse of the pivot
            int multiplier = (element_to_eliminate * pivot_inverse) % 26;

            // Perform the row operation: Row_i = (Row_i - multiplier * Row_j) mod 26
            // Apply this operation to elements in columns from j to the end of the row
            for (int k = j; k < n; k++) {
                int term_to_subtract = (multiplier * temp_matrix[j][k]) % 26;
                temp_matrix[i][k] = (temp_matrix[i][k] - term_to_subtract + 26) % 26; // Add 26 to handle potential negative results from subtraction
            }
        }
    }

    // --- Step 6: Adjust determinant sign based on row swaps ---
    // Each row swap multiplies the determinant by -1. Modulo 26, -1 is 25.
    if (num_row_swaps % 2 != 0) { // If an odd number of swaps occurred
        current_determinant = (26 - current_determinant) % 26; // This is equivalent to (25 * current_determinant) % 26
    }

    // Ensure the final determinant is in the range [0, 25]
    current_determinant = (current_determinant % 26 + 26) % 26;


    // The final determinant is the product of the diagonal elements after elimination,
    // adjusted for swaps. The loops already calculated this product step-by-step.

    return current_determinant; // Return the calculated determinant modulo 26
}


void makehillkey(int seed, int hillkey_out[26][26]){ //consider returning determinant.
    srand(seed);
    int len=26;
    int det=0, gcd=0;
    int count=0;
    do{
        for(int i=0;i<len;i++){
            for(int j=0;j<len;j++){
                hillkey_out[i][j]=rand()%26;
            }
        }
        det=determinantOfMatrixUsingGaussian(hillkey_out);
        count++;
        det=det%26;
        gcd=my_get_gcd(det,26);

        if((count%1000)==0){printf("%d",count);}

    }while(det==0); // Loop until an invertible matrix (det != 0 mod 26) is found
    printf("Determinant found");
} 

int makefirstanswerkey(int *keystream, int len_of_keystream, char *first_answer, int *keystream_out, int seed){
    int len_out=(strlen(first_answer)<len_of_keystream)?strlen(first_answer):len_of_keystream;
    int randstream[len_out];
    srand(seed);
    for(int i=0;i<len_out;i++){randstream[i]=rand();}
    
    for(int i=0;i<len_out;i++){
        srand((int)first_answer[i]*(keystream[i]+1)*(randstream[i]+1));
        keystream_out[i]=(rand()+seed)%26;
    }
    for(int i=0;i<(len_out-1);i++){
        srand(keystream_out[i]*keystream_out[i+1]); //i know its usually i-1 but i thought why not
        keystream_out[i]=(rand()*seed)%26;
    }
    srand(keystream_out[len_out-1]*keystream_out[0]);
    keystream_out[len_out-1]=(rand()*seed)%26;

    return (len_out);
}

int makesecondanswerkey(int *keystream,int *key1, int key1len, char *second_answer, int *keystream_out, int seed, int seed1){
    int len_out=(strlen(second_answer)<key1len)?strlen(second_answer):key1len;
    //key 1 is as long as the shorter bw keystream and answer 1. so we dont need to compare those 2 seperately.
    int randstream[len_out];
    srand(seed*seed1);
    for(int i=0;i<len_out;i++){randstream[i]=rand();}
    
    for(int i=0;i<(len_out);i++){
        srand((int)second_answer[i]*(keystream[i]+1)*(key1[i]+1)*(randstream[i]+1));
        keystream_out[i]=(rand()+seed+seed1)%26;
    }
    for(int i=0;i<(len_out-1);i++){
        srand(keystream_out[i]*keystream_out[i+1]); //i know its usually i-1 but i thought why not
        keystream_out[i]=(rand()*seed*seed1)%26;
    }
    srand(keystream_out[len_out-1]*keystream_out[0]);
    keystream_out[len_out-1]=(rand()*seed*seed1)%26;

    return (len_out);
}

void makepackets(char *ciphertext, char packets_out[][26]){ //same as before
    int j=0;
    char temp[26]={'\0'};
    int counter=7;
    sprintf(temp, "%03d%c%03d", (int)ceil(strlen(ciphertext)/18.0), ciphertext[0], j+1); //metadata that tells: num_of_packets, identifying attribute, packet_num
    for(int i=0;i<strlen(ciphertext);i++){
        temp[counter++]=ciphertext[i];
        if(counter==25){
            temp[25]='\0';
            strcpy(packets_out[j++],temp);
            if (j>98){return;}
            for(int k=0;k<26;k++){temp[k]='\0';}
            sprintf(temp, "%03d%c%03d", (int)ceil(strlen(ciphertext)/18.0), ciphertext[0], j+1); //metadata that tells: num_of_packets, identifying attribute, packet_num
            counter=7;
        }
    }
    if (strlen(ciphertext)%18!=0){
        temp[counter]='\0';
        strcpy(packets_out[j],temp);
    }
}

int makejunk(char packets_out[][26], int hillkey[][26]){ 
    char randtext[]="philosophyofeducationisalabelappliedtothestudyofthepurposeprocessnatureandidealsofeducationitcanbeconsideredabranchofbothphilosophyandeducationeducationcanbedefinedastheteachingandlearningofspecificskillsandtheimpartingofknowledgejudgmentandwisdomandissomethingbroaderthanthesocietalinstitutionofeducationweoftenspeakofmanyeducationalistsconsideritaweakandwoollyfieldtoofarremovedfromthepracticalapplicationsoftherealworldtobeusefulbutphilosophersdatingbacktoplatoandtheancientgreekshavegiventheareamuchthoughtandemphasisandthereislittledoubtthattheirworkhashelpedshapethepracticeofeducationoverthemillenniaplatoistheearliestimportanteducationalthinkerandeducationisanessentialelementintherepublichismostimportantworkonphilosophyandpoliticaltheorywrittenaroundbcinitheadvocatessomeratherextrememethodsremovingchildrenfromtheirmotherscareandraisingthemaswardsofthestateanddifferentiatingchildrensuitabletothevariouscastesthehighestreceivingthemosteducationsothattheycouldactasguardiansofthecityandcareforthelessablehebelievedthateducationshouldbeholisticincludingfactsskillsphysicaldisciplinemusicandartplatobelievedthattalentandintelligenceisnotdistributedgeneticallyandthusisbefoundinchildrenborntoallclassesalthoughhisproposedsystemofselectivepubliceducationforaneducatedminorityofthepopulationdoesnotreallyfollowademocraticmodelaristotleconsideredhumannaturehabitandreasontobeequallyimportantforcestobecultivatedineducationtheultimateaimofwhichshouldbetoproducegoodandvirtuouscitizensheproposedthatteachersleadtheirstudentssystematicallyandthatrepetitionbeusedasakeytooltodevelopgoodhabitsunlikesocratesemphasisonquestioninghislistenerstobringouttheirownideasheemphasizedthebalancingofthetheoreticalandpracticalaspectsofsubjectstaughtamongwhichheexplicitlymentionsreadingwritingmathematicsmusicphysicaleducationliteraturehistoryandawiderangeofsciencesaswellasplaywhichhealsoconsideredimportantduringthemedievalperiodtheideaofperennialismwasfirstformulatedbystthomasaquinashisinworkdemagistroperennialismholdsthatoneshouldteachthosethingsdeemedtobeofeverlastingimportancetoallpeopleeverywherenamelyprinciplesandreasoningnotjustfactswhichareapttochangeovertimeandthatoneshouldteachfirstaboutpeoplenotmachinesortechniquesitwasoriginallyreligiousinnatureanditwasonlymuchlaterthatatheoryofsecularperennialismdevelopedduringtherenaissancethefrenchskepticmicheldemontaignewasoneofthefirsttocriticallylookateducationunusuallyforhistimemontaignewaswillingtoquestiontheconventionalwisdomoftheperiodcallingintoquestionthewholeedificeoftheeducationalsystemandtheimplicitassumptionthatuniversityeducatedphilosopherswerenecessarilywiserthanuneducatedfarmworkersforexample";
    //were going with this rand text instead of just taking random characters one at a time so that people cant detect junk usinc Index of Coincidence.
    //this is still easily breakable.
    char plainrandtext[100];
    srand(time(0));

    plainrandtext[0] = '\0'; //gpt made these lines:
    int rand_start = rand() % (strlen(randtext) - 100); // ensure at least 100 chars available
    int rand_len = 49 + rand() % 50; 
    strncpy(plainrandtext, randtext + rand_start, rand_len);
    plainrandtext[rand_len] = '\0';

    char cipherrandtext[100];
    char cipherjunktext[500];
    int key[100];
    int len=makekey(plainrandtext,randtext,key);
    vigenerre_encrypt(plainrandtext,cipherrandtext,key,len);
    insertjunkintostream(cipherrandtext,cipherjunktext);

    int num=(int)ceil(strlen(cipherjunktext)/18.0);
    char packets[num][26];
    makepackets(cipherjunktext,packets);
    hill_encrypt(num,packets,hillkey,packets_out);

    return(num);

}

int makejunkkeystream(int *keystream_out){ //same as before
    char randtext[]="philosophyofeducationisalabelappliedtothestudyofthepurposeprocessnatureandidealsofeducationitcanbeconsideredabranchofbothphilosophyandeducationeducationcanbedefinedastheteachingandlearningofspecificskillsandtheimpartingofknowledgejudgmentandwisdomandissomethingbroaderthanthesocietalinstitutionofeducationweoftenspeakofmanyeducationalistsconsideritaweakandwoollyfieldtoofarremovedfromthepracticalapplicationsoftherealworldtobeusefulbutphilosophersdatingbacktoplatoandtheancientgreekshavegiventheareamuchthoughtandemphasisandthereislittledoubtthattheirworkhashelpedshapethepracticeofeducationoverthemillenniaplatoistheearliestimportanteducationalthinkerandeducationisanessentialelementintherepublichismostimportantworkonphilosophyandpoliticaltheorywrittenaroundbcinitheadvocatessomeratherextrememethodsremovingchildrenfromtheirmotherscareandraisingthemaswardsofthestateanddifferentiatingchildrensuitabletothevariouscastesthehighestreceivingthemosteducationsothattheycouldactasguardiansofthecityandcareforthelessablehebelievedthateducationshouldbeholisticincludingfactsskillsphysicaldisciplinemusicandartplatobelievedthattalentandintelligenceisnotdistributedgeneticallyandthusisbefoundinchildrenborntoallclassesalthoughhisproposedsystemofselectivepubliceducationforaneducatedminorityofthepopulationdoesnotreallyfollowademocraticmodelaristotleconsideredhumannaturehabitandreasontobeequallyimportantforcestobecultivatedineducationtheultimateaimofwhichshouldbetoproducegoodandvirtuouscitizensheproposedthatteachersleadtheirstudentssystematicallyandthatrepetitionbeusedasakeytooltodevelopgoodhabitsunlikesocratesemphasisonquestioninghislistenerstobringouttheirownideasheemphasizedthebalancingofthetheoreticalandpracticalaspectsofsubjectstaughtamongwhichheexplicitlymentionsreadingwritingmathematicsmusicphysicaleducationliteraturehistoryandawiderangeofsciencesaswellasplaywhichhealsoconsideredimportantduringthemedievalperiodtheideaofperennialismwasfirstformulatedbystthomasaquinashisinworkdemagistroperennialismholdsthatoneshouldteachthosethingsdeemedtobeofeverlastingimportancetoallpeopleeverywherenamelyprinciplesandreasoningnotjustfactswhichareapttochangeovertimeandthatoneshouldteachfirstaboutpeoplenotmachinesortechniquesitwasoriginallyreligiousinnatureanditwasonlymuchlaterthatatheoryofsecularperennialismdevelopedduringtherenaissancethefrenchskepticmicheldemontaignewasoneofthefirsttocriticallylookateducationunusuallyforhistimemontaignewaswillingtoquestiontheconventionalwisdomoftheperiodcallingintoquestionthewholeedificeoftheeducationalsystemandtheimplicitassumptionthatuniversityeducatedphilosopherswerenecessarilywiserthanuneducatedfarmworkersforexample";
    char randusn[100];
    char randpwd[100];
    srand(time(0));
    randusn[0] = '\0'; 
    randpwd[0] = '\0';
    int rand_start = rand() % (strlen(randtext) - 100); // ensure at least 100 chars available
    int rand_len = 49 + rand() % 50; // copy 90 to 99 characters
    strncpy(randusn, randtext + rand_start, rand_len);
    randusn[rand_len] = '\0';
    rand_start = rand() % (strlen(randtext) - 100);
    rand_len = 49 + rand() % 50;
    strncpy(randpwd, randtext + rand_start, rand_len);
    randpwd[rand_len]='\0';

    for(int i=0;i<((strlen(randpwd)<strlen(randusn))?strlen(randpwd):strlen(randusn));i++){
        keystream_out[i]=((int)randusn[i]*(int)randpwd[i])%26;
    }
    return (strlen(randpwd)<strlen(randusn))?strlen(randpwd):strlen(randusn);
}

void namepackets(char packetnames_out[][100],int numpackets,int seed){ //same as before
    char letters[]="abcdefghijklmnopqrstuvwxyz0123456789";

    for(int i=0;i<numpackets;i++){
        //we need to name the packets in a recreatable yet seemingly random way so that we can fetch those packets again but the guy cant.
        //name should be dependent only on keystream (which comes from usn and password) and not on packet contents as we should be able to recreate to fetch the right packets.
        srand(seed + i);
        for(int j=0;j<100;j++){
            packetnames_out[i][j]=letters[rand()%strlen(letters)];
        }
        packetnames_out[i][99]='\0';

        char temp[113];
        sprintf(temp,"storage/%s.txt",packetnames_out[i]);
        FILE *pF=fopen(temp,"r");
        if (pF){ //this means file already exists. so we throw error and stop
            printf("FILE ALREADY EXISTS ERROR!: %s",temp);
        }

        for(int k=0;k<i;k++){if(!strcmp(packetnames_out[k],packetnames_out[i])){printf("FILENAME already EXISTS ERROR!");}}

    }
}

//TODO: IMP change this to make it check for any empty directories and fill those first. maybe evne invcrease nubmer of junk somehow?
void namejunk(char packetpaths_out[][513],int numpackets,int *keystream, int len_of_keystream){ //same as before
    //this is the same as the above function but if the file exists, we just generate another name, doesnt matter.
    //cause it doesnt need to be reproducible.
    char packetnames[numpackets][100];
    char letters[]="abcdefghijklmnopqrstuvwxyz0123456789";
    unsigned long long int num_unique_to_user=1;
    for(int i=0;i<len_of_keystream;i++){num_unique_to_user*=(keystream[i]+1);} //avoids 0 multiplication.
    num_unique_to_user+=len_of_keystream;

    char subdirs[100][256];
    int num_subdirs = get_subdirectories("storage", subdirs, 100); //gets (up to) 100 subdirs from storage.

    //TODO: check and make sure reproducibility.
    //PROBLEM THIS IS NOT REPRODUCIBLE. MAKE SURE IT DOESNT HAPPEN!!
    if (num_subdirs == 0) { //if no subdirectories found. failsafe. consider changing it to make dirs based on key instead of just using default.
        printf("ERROR: No subdirectories found. Creating 'default'...\n");
        mkdir("storage/default");
        strcpy(subdirs[0], "default");
        num_subdirs = 1;
    }

    for(int i=0;i<numpackets;i++){
        //we need to name the packets in a recreatable yet seemingly random way so that we can fetch those packets again but the guy cant.
        //name should be dependent only on keystream (which comes from usn and password) and not on packet contents as we should be able to recreate to fetch the right packets.
        srand(num_unique_to_user/(keystream[i%len_of_keystream]+1)*(i+1)); //(avoids division/multiplication by 0)
        int unique_name=0; //keeps writing the path till a unique one found. 
        //yes i know this is inefficient, just roll with it for now.
        while(unique_name==0){
            for(int j=0;j<100;j++){
                packetnames[i][j]=letters[rand()%strlen(letters)];
            }
            packetnames[i][99]='\0';

            char temp[513];
            //srand(time(0));
            int subdir_index = rand() % num_subdirs;  //chooses a number and takes subdir[that num]
            sprintf(temp,"storage/%s/%s.txt", subdirs[subdir_index], packetnames[i]); //prints directory name into filepath   

            FILE *pF=fopen(temp,"r");
            if (!pF){ //meaning unique name created.
                unique_name=1;
                strcpy(packetpaths_out[i],temp);
            }
        }
    }
}

void writepacketsintofiles(char packetpaths[][513],int numpacks,char packets[][26],char junkpaths[][513],int numjunk,char junk[][26], int *keystream, int len_of_keystream){ //same as before
    //yes, I know this is V inneficient, just roll with it for this version. consider using a boolean array
    int writtenpackets[numpacks];
    for(int k=0;k<numpacks;k++){writtenpackets[k]=0;}
    int writtenjunk[numjunk];
    for(int k=0;k<numjunk;k++){writtenjunk[k]=0;}
    srand(keystream[((int)junk[2][19])%len_of_keystream]); //gives a random seed
    int countwrittenpacks=0, countwrittenjunk=0;
    while( countwrittenpacks!=numpacks || countwrittenjunk!=numjunk){ 
        //we need to write all packets and junkpackets at random so that attacker cant know if the packet is junk or not, or the order they go in by seeing when it was created.
        //checking_if_all_packets_written
            // countwrittenjunk=0;
            // countwrittenpacks=0;
            // for(int k=0;k<numpacks;k++){if(writtenpackets[k]==1){countwrittenpacks++;}}
            // for(int k=0;k<numjunk;k++){if(writtenjunk[k]==1){countwrittenjunk++;}}
            // if(countwrittenjunk==numjunk && countwrittenpacks==numpacks){break;}
        if(rand()%2==1){ //50/50 chnance of writing junk or packet
            if(countwrittenpacks==numpacks){continue;}
            //packet
            int temp= rand()%numpacks;
            if (writtenpackets[temp]==0){ //this means packet is not empty, so we write packet and make it empty
                writetofile(packetpaths[temp], packets[temp]); //writing
                writtenpackets[temp]=1;
                countwrittenpacks++;
                }
            else{continue;}//packet is empty already, run loop again
        }
        else{
            if(countwrittenjunk==numjunk){continue;}
            //junk
            int temp= rand()%numjunk;
            if (writtenjunk[temp]==0){ //this means packet is not empty, so we write packet and make it empty
                writetofile(junkpaths[temp], junk[temp]); //writing
                writtenjunk[temp]=1;
                countwrittenjunk++;
                }
            else{continue;}//packet is empty already, run loop again
        }
    }

}


void handle_encryption_tasks(char *plaintext, int *key_given, int len_of_key_given, int seed_passed){
    char ciphertext[200];

    vigenerre_encrypt(plaintext,ciphertext,key_given,len_of_key_given);
    //now ciphertext has vigenerre encrypted plaintext

    char cipherjunktext[500];
    insertjunkintostream(ciphertext,cipherjunktext);
    //now ciphertext is filled with junk making it longer and slightly disturbing frequency analysis.

    // now distributing contents based on seed.
    int numpacks=(int)ceil(strlen(cipherjunktext)/18.0);
    if(numpacks>25){printf("ERROR: contents too long.");exit(1);}
    char packets[numpacks][26];
    char packetnames[numpacks][100];
    char packetpaths[numpacks][513]; //maybe make this 313
    char junkpaths[26][513];
    char junk [26][26]; 
    int junkkeystream[100];
    for(int i=0;i<26;i++){for(int j=0;j<26;j++){junk[i][j]='\0';}}


    makepackets(cipherjunktext,packets);
    //now packets have the ciphertext with the junk with the metadata(unencrypted)

    char newpackets[numpacks][26];

    int hillkey[26][26];
    makehillkey(seed_passed,hillkey);
    hill_encrypt(numpacks,packets,hillkey,newpackets);
    //now newpackets have the final encrypted packets (even metadata is encrypted.)

    int numjunk=makejunk(junk,hillkey);

    int junkkeylen=makejunkkeystream(junkkeystream);

    namepackets(packetnames,numpacks,seed_passed);//needs to be reproducible, based on all seed
    namejunk(junkpaths,numjunk,junkkeystream,junkkeylen); //doenst need to be reproducible, but shouldnt overwrite actual packets.

    getpaths(packetpaths,packetnames,numpacks,seed_passed);

    writepacketsintofiles(packetpaths,numpacks,newpackets,junkpaths,numjunk,junk,key_given,len_of_key_given);
    printf("Encrypted and Saved.");
}

void signup(int *keystream, int len_of_key, int seed){
    //note that overflow problems may still occur cause of how everything is [100], so make sure you enter small stuff for this version.
    char first_answer[100];
    int first_key[100];
    char second_answer[100];
    int second_key[100];
    int tempkey[100];
    int tempkeylen;

    char plaintext[100];
    
    inputstring("your mothers full name", first_answer);
    inputstring("the name of any good friend", second_answer);
    //since two people can have the same answer, we use a combination of the keystream (which is from usn and pwd) and the answers to make new keystreams
    // once created and stored, while reading, only needs to be found and used. never made again. 
    // hence this can be anything. in the future, make it more secure.
    int key1len=makefirstanswerkey(keystream, len_of_key, first_answer, first_key, seed); //made using keystream and ans1
    int seed1=getuniquetouserseed(first_key, key1len);
    int key2len=makesecondanswerkey(keystream, first_key, key1len, second_answer, second_key, seed, seed1); //made using keystream and ans1key and ans2
    int seed2=getuniquetouserseed(second_key,key2len);
    for(int i=0;i<100;i++){first_answer[i]='\0';second_answer[i]='\0';}
    //   now the keystream is from usn and password. key1 is from Q1, keystream. key2 is from Q2, keystream, key1.
    //   this kinda doesnt matter though as they are just created once and then stored.

    //locations of key1 is from seed and encryption is from keystream.
    //locations of key2 is from seed1 and seed and encryption is from keystream and key1.
    //locations of contents is from seed2, seed1 and seed and encryption is from keystream, key1 and key2.

    //encrypting contents from keystream, key1 and key2
    inputstring("Contents",plaintext);
    tempkeylen=key2len;
    for(int i=0;i<tempkeylen;i++){
        tempkey[i]=(keystream[i]+first_key[i]+second_key[i])%26;
    }
    handle_encryption_tasks(plaintext, tempkey, tempkeylen, (seed*seed1*seed2));
    for(int i=0;i<100;i++){plaintext[i]='\0'; tempkey[i]=0;} //for safety.

    //encrypting key2 from keystream and key1
    tempkeylen=key1len;
    for(int i=0;i<key2len;i++){plaintext[i]=(char)(second_key[i]+97);}
    plaintext[99]='\0';//praying the length of key is under 100
    for(int i=0;i<tempkeylen;i++){
        tempkey[i]=(keystream[i]+first_key[i])%26;
    }
    handle_encryption_tasks(plaintext, tempkey, tempkeylen, (seed*seed1));
    for(int i=0;i<100;i++){second_key[i]=0;tempkey[i]=0;plaintext[i]='\0';} //for safety.

    //encrypting key1 from keystream 
    for(int i=0;i<key1len;i++){plaintext[i]=(char)(first_key[i]+97);}
    plaintext[99]='\0'; //praying the length of key is under 100
    handle_encryption_tasks(plaintext, keystream, len_of_key, seed);
    for(int i=0;i<100;i++){first_key[i]=0;tempkey[i]=0;plaintext[i]=0;} //for safety.
}

void getpackets(int numpacks, char packetnames[100][513], char packets_out[numpacks][26]){ //same as before
    for(int i=0;i<numpacks;i++){
        readcontents(packetnames[i],packets_out[i]);
    }
}

void openpackets(char *ciphertext_out, char packets[][26], int numpacks){ //same as before
    int count=0;
    ciphertext_out[0]='\0';
    for(int i=0;i<numpacks;i++){
        for(int j=7;j<26;j++){
            if (packets[i][j]!='\0' && packets[i][j]>=97 && packets[i][j]<=122){
                ciphertext_out[count++]=packets[i][j];
            }
        }
    }
    ciphertext_out[count]='\0';
}

int getpacketnames(char packetnames_out[][100],int seed){
    char letters[]="abcdefghijklmnopqrstuvwxyz0123456789";

    int numpackets=0;
    srand(seed+0);
    char firstpacketname[100];
    for(int j=0;j<100;j++){
        firstpacketname[j]=letters[rand()%strlen(letters)];
    }
    firstpacketname[99]='\0';
    char firstpacketpath[513];

    //getting first packet path
    char subdirs[100][256];
    int num_subdirs = get_subdirectories("storage", subdirs, 100); //gets (up to) 100 subdirs from storage.

    //we get the first packet name this way as we know there must be atleast one packet.
    // but we dont know how many packets there are, so we dont know how many names to generate.
    // so we open the first packet and see the first 3 letters of metadata which tells us how many packets there are.
    
    srand(seed);
    int subdir_index = rand() % num_subdirs;  //chooses a number and takes subdir[that num]
    sprintf(firstpacketpath,"storage/%s/%s.txt", subdirs[subdir_index], firstpacketname); //prints directory name into filepath   

    char firstpacket[26];
    readcontents(firstpacketpath, firstpacket);    
    char total_packets[4];
    slice(total_packets,firstpacket,0,2);
    numpackets=atoi(total_packets); //test this once.

    for(int i=0;i<numpackets;i++){
        //we need to name the packets in a recreatable yet seemingly random way so that we can fetch those packets again but the guy cant.
        //name should be dependent only on keystream (which comes from usn and password) and not on packet contents as we should be able to recreate to fetch the right packets.
        srand(seed+i);
        for(int j=0;j<100;j++){
            packetnames_out[i][j]=letters[rand()%strlen(letters)];
        }
        packetnames_out[i][99]='\0';
    }
    return numpackets;
}

void getfullplaintext(int *keystream, int len_of_key, int seed, char *plaintext_out){
    char plaintext[100];
    char ciphertext[200];
    char packetnames[100][100];
    char packetpaths[100][513];
    int numpackets=getpacketnames(packetnames,seed);

    getpaths(packetpaths,packetnames,numpackets,seed);

    char packets[numpackets][26];
    
    //there is no need to order the packets we get since getpacketnames gets the names in the same order as it was encrypted.
    //maybe this is a security issue. TODO: check and fix if needed.
    getpackets(numpackets,packetpaths,packets); 
    openpackets(ciphertext,packets,numpackets); 
    vigenerre_decrypt(ciphertext, plaintext, keystream, len_of_key);
    strcpy(plaintext_out,plaintext);
}

//TODO: consider making a reconstruct_all_keys function cause thats done in both read and delete.
void my_read(int *keystream, int len_of_key, int seed){
    char plaintext[100];
    int first_key[100];
    int key1len;
    int second_key[100];
    int key2len;
    int tempkey[100];
    int tempkeylen;
    int seed1;
    int seed2;

    //locations of key1 is from seed and encryption is from keystream.
    //locations of key2 is from seed1 and seed and encryption is from keystream and key1.
    //locations of contents is from seed2, seed1 and seed and encryption is from keystream, key1 and key2.

    //getting decrypted key1 using seed and keystream
    getfullplaintext(keystream, len_of_key, seed, plaintext);
    key1len=strlen(plaintext);
    for(int i=0;i<key1len;i++){first_key[i]=(int)(plaintext[i]-97);}
    seed1=getuniquetouserseed(first_key, key1len);
    for(int i=0;i<100;i++){plaintext[i]='\0';}

    //getting decrypted key2 using seed1 and seed and keystream and key1
    tempkeylen=key1len;
    for(int i=0;i<tempkeylen;i++){tempkey[i]=(keystream[i]+first_key[i])%26;}
    getfullplaintext(tempkey, tempkeylen, (seed*seed1), plaintext);
    key2len=strlen(plaintext);
    for(int i=0;i<key2len;i++){second_key[i]=(int)(plaintext[i]-97);}
    seed2=getuniquetouserseed(second_key,key2len);
    for(int i=0;i<100;i++){plaintext[i]='\0';tempkey[i]=0;}

    //getting decrypted contents using seed2, seed1 and seed and keystream and key1 and key2
    tempkeylen=key2len;
    for(int i=0;i<tempkeylen;i++){tempkey[i]=(keystream[i]+first_key[i]+second_key[i])%26;}
    getfullplaintext(tempkey, tempkeylen, (seed*seed1*seed2), plaintext);
    for(int i=0;i<100;i++){tempkey[i]=0;first_key[i]=0;second_key[i]=0;}
    printf("%s",plaintext);
}

void delete(int *keystream, int len_of_key, int seed){
    char plaintext[100];
    int first_key[100];
    int key1len;
    int second_key[100];
    int key2len;
    int tempkey[100];
    int tempkeylen;
    int seed1;
    int seed2;

    //getting decrypted key1 using seed and keystream
    getfullplaintext(keystream, len_of_key, seed, plaintext);
    key1len=strlen(plaintext);
    for(int i=0;i<key1len;i++){first_key[i]=(int)(plaintext[i]-97);}
    seed1=getuniquetouserseed(first_key, key1len);
    for(int i=0;i<100;i++){plaintext[i]='\0';}

    //getting decrypted key2 using seed1 and seed and keystream and key1
    tempkeylen=key1len;
    for(int i=0;i<tempkeylen;i++){tempkey[i]=(keystream[i]+first_key[i])%26;}
    getfullplaintext(tempkey, tempkeylen, (seed*seed1), plaintext);
    key2len=strlen(plaintext);
    for(int i=0;i<key2len;i++){second_key[i]=(int)(plaintext[i]-97);}
    seed2=getuniquetouserseed(second_key,key2len);
    for(int i=0;i<100;i++){plaintext[i]='\0';tempkey[i]=0;}

    //now we have all 3 seeds.
    for(int i=0;i<100;i++){first_key[i]=0;second_key[i]=0;}

    char packetnames[100][100];
    char packetpaths[100][513];
    int numpackets=getpacketnames(packetnames,(seed*seed1*seed2)); 
    getpaths(packetpaths,packetnames,numpackets,(seed*seed1*seed2));
    deletepackets(packetpaths, numpackets);
    
    numpackets=getpacketnames(packetnames,(seed*seed1)); 
    getpaths(packetpaths,packetnames,numpackets,(seed*seed1));
    deletepackets(packetpaths, numpackets);

    numpackets=getpacketnames(packetnames,(seed));
    getpaths(packetpaths,packetnames,numpackets,seed);
    deletepackets(packetpaths, numpackets);

}

void edit(int *keystream, int len_of_key, int seed){
    my_read(keystream, len_of_key, seed);
    printf("\n");
    delete(keystream, len_of_key, seed);
    printf("\n");
    signup(keystream, len_of_key, seed);
    printf("Saved.");
}
int main(){
    char username[100];
    char password[100];
    int keystream[100];
    inputstring("Username",username);
    inputstring("Password",password);

    int len_of_key=makekey(username,password,keystream);
    for(int i=0;i<100;i++){username[i]='\0';password[i]='\0';}
    int seed=getuniquetouserseed(keystream, len_of_key);

    char useless[10];
    fgets(useless,10,stdin); //clears
    
    char choice;
    printf("Enter choice (S for signup, R for read, E for edit, D for delete):\n");
    scanf("%c",&choice);
    switch (choice)
    {
    case 'S':
        signup(keystream,len_of_key, seed);
        break;
    case 'R':
        my_read(keystream,len_of_key, seed);
        break;
    case 'E':
        edit(keystream,len_of_key, seed);
        break;
    case 'D':
        delete(keystream, len_of_key, seed);
        break;
    default:
        printf("Invalid input.");
        break;
    }

    return 0;
}