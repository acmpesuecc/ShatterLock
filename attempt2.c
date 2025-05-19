//gonna do the exact same thing as with attempt1.c but with 2 additional security questions only asked at the start 
// to create 2 extra keys that will also be hidden. then use all 3 keystreams to encrypt and decrypt.
// also going to add more junk in the actual packets so that theres more packets. but also add more junk in junk packets so the ratio stays the same.
// i was thinking we could have alternate characters be junk. so that people cant use kasiski test to get the key and stuff.
// the only point of this is to increase the number of packets the attacker needs to fetch and also the number of packets in the system 
// so that its harder t brute force.
// i think we should have a ratio of real files to junk files and try to achieve that at every point.
// challenges i think will arise:
//      how to encrypt using all 3 keys in a strong way?
//      structure of getting key 2 and 3 from password and so on
//      smarter generation of junk (achieving ratios)
// look into qsort()
// TODO: make sure ppl cant get to know length of key 1, 2 or 3. make sure packets stored are always of the same length
//      regardless of length of key.
// TODO: encrypt the metadata and add a secodn layer of encryption to the whole thing. (layers and layered metadata encryption?)
// TODO: once the encryption is better, encrypt it in layers using key 1, 2 and keystream instead of 1 tempkeystream like here.
// in a future version, have it such that one user can have many contents

// TODO: IMP Obscure more. i.e when choosing directories, choose all directories atleast once.
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

// Multiplies a set of row vectors (R1 x 26) by a 26x26 key matrix for Hill cipher encryption.
// m1: R1 x 26 matrix (each row is a message vector)
// key: 26x26 Hill cipher key
// m_out: R1 x 26 output (each row is the encrypted vector as lowercase letters)
void multiply_matrices(int R1, int m1[R1][26], int key[26][26], char m_out[R1][26]) { //copilot helped with this, pls forgive.
    char letters[] = "abcdefghijklmnopqrstuvwxyz";
    for (int i = 0; i < R1; i++) {
        for (int j = 0; j < 26; j++) {
            int sum = 0;
            for (int k = 0; k < 26; k++) {
                sum = (sum + m1[i][k] * key[k][j]) % 26;
            }
            m_out[i][j] = letters[(sum + 26) % 26];
        }
    }
}

// Encrypts numpacks packets using the Hill cipher key (packets_in × hillkey) //copilot helped with this, pls forgive.
void hill_encrypt(int numpacks, char packets_in[numpacks][26], int hillkey[26][26], char packets_out[numpacks][26]) {
    int packets_num[numpacks][26];
    for (int i = 0; i < numpacks; i++) {
        for (int j = 0; j < 26; j++) {
            packets_num[i][j] = ((int)packets_in[i][j] + 26 - 97) % 26;
        }
    }
    multiply_matrices(numpacks, packets_num, hillkey, packets_out);
}

// Decrypts numpacks packets using the Hill cipher key (by inverting the key) //copilot made this, pls forgive.
void hill_decrypt(int numpacks, char packets_in[numpacks][26], int hillkey[26][26], char packets_out[numpacks][26]) {
    int invkey[26][26];
    if (!invertMatrixMod26(hillkey, invkey)) {
        printf("Error: Hill key is not invertible!\n");
        return;
    }
    int packets_num[numpacks][26];
    for (int i = 0; i < numpacks; i++) {
        for (int j = 0; j < 26; j++) {
            packets_num[i][j] = ((int)packets_in[i][j] + 26 - 97) % 26;
        }
    }
    multiply_matrices(numpacks, packets_num, invkey, packets_out);
}

void insertjunkintostream(char *ciphertext_in, char *ciphertext_out){
    char letters[]="abcdefghijklmnopqrstuvwxyz";
    srand(time(0));
    int len=strlen(ciphertext_in);
    int count=0, j=0;
    for(int i=0;i<len;i++){
            ciphertext_out[count++]=letters[rand()%strlen(letters)]; //junk character
            ciphertext_out[count]=ciphertext_in[i]; //real character
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


// New function to generate a 26x26 invertible Hill cipher key matrix
// using the L*U construction method.
// This method guarantees an invertible matrix modulo 26 on the first attempt.
// Parameters:
//   seed: The seed for the random number generator (to make key generation reproducible)
//   hillkey_out: The output matrix (26x26) where the invertible key will be stored
void makehillkey(int seed, int hillkey_out[26][26]) { //github copilot made this. pls forgive.
    srand(seed); // Seed the random number generator
    
    // This function generates a 26x26 invertible matrix modulo 26 using the L*U construction method.
    // The matrix is always invertible mod 26, and the process is deterministic for a given seed.
    // L: lower-triangular with 1s on the diagonal (invertible mod 26)
    // U: upper-triangular with random nonzero diagonal (invertible mod 26)
    int L[26][26] = {0};
    int U[26][26] = {0};
    // Fill L (lower-triangular, 1s on diagonal)
    for (int i = 0; i < 26; i++) {
        for (int j = 0; j <= i; j++) {
            if (i == j) {
                L[i][j] = 1; // 1s on diagonal
            } else {
                L[i][j] = rand() % 26; // random element in Z26
            }
        }
    }
    // Fill U (upper-triangular, random nonzero diagonal)
    for (int i = 0; i < 26; i++) {
        for (int j = i; j < 26; j++) {
            if (i == j) {
                // Diagonal must be coprime to 26 (i.e., odd and not divisible by 13)
                int val = (rand() % 25) + 1; // 1..25
                while (val % 2 == 0 || val % 13 == 0) {
                    val = (rand() % 25) + 1;
                }
                U[i][j] = val;
            } else {
                U[i][j] = rand() % 26;
            }
        }
    }
    // Multiply L and U to get the final key matrix (mod 26)
    for (int i = 0; i < 26; i++) {
        for (int j = 0; j < 26; j++) {
            int sum = 0;
            for (int k = 0; k < 26; k++) {
                sum = (sum + L[i][k] * U[k][j]) % 26;
            }
            hillkey_out[i][j] = sum;
        }
    }
    // Now hillkey_out is a random invertible matrix mod 26, reproducible from the seed.
}

// Computes the inverse of a 26x26 matrix modulo 26 using Gauss-Jordan elimination.
// Returns 1 on success, 0 if not invertible (should never happen with your construction).
// input: the matrix to invert (26x26)
// output: the resulting inverse (26x26)
int modinv26(int a) { //github copilot mad ethis. pls forgive.
    // Returns the modular inverse of a mod 26, or 0 if not invertible
    a = a % 26;
    for (int x = 1; x < 26; x++) {
        if ((a * x) % 26 == 1) return x;
    }
    return 0;
}

int invertMatrixMod26(int input[26][26], int output[26][26]) { //github copilot made this. pls forgive.
    int n = 26;
    int aug[26][52];
    // Set up augmented matrix [input | I]
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            aug[i][j] = input[i][j] % 26;
            if (aug[i][j] < 0) aug[i][j] += 26;
            aug[i][j + n] = (i == j) ? 1 : 0;
        }
    }
    // Gauss-Jordan elimination
    for (int col = 0; col < n; col++) {
        // Find pivot
        int pivot = -1;
        for (int row = col; row < n; row++) {
            if (aug[row][col] != 0 && modinv26(aug[row][col]) != 0) {
                pivot = row;
                break;
            }
        }
        if (pivot == -1) return 0; // Not invertible
        // Swap rows if needed
        if (pivot != col) {
            for (int k = 0; k < 2 * n; k++) {
                int tmp = aug[col][k];
                aug[col][k] = aug[pivot][k];
                aug[pivot][k] = tmp;
            }
        }
        // Scale pivot row
        int inv = modinv26(aug[col][col]);
        for (int k = 0; k < 2 * n; k++) {
            aug[col][k] = (aug[col][k] * inv) % 26;
            if (aug[col][k] < 0) aug[col][k] += 26;
        }
        // Eliminate other rows
        for (int row = 0; row < n; row++) {
            if (row == col) continue;
            int factor = aug[row][col];
            for (int k = 0; k < 2 * n; k++) {
                aug[row][k] = (aug[row][k] - factor * aug[col][k]) % 26;
                if (aug[row][k] < 0) aug[row][k] += 26;
            }
        }
    }
    // Extract inverse
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            output[i][j] = aug[i][j + n];
        }
    }
    return 1;
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
    //insertjunkintostream(cipherrandtext,cipherjunktext);

    int num=(int)ceil(strlen(cipherrandtext)/18.0);
    char packets[num][26];
    makepackets(cipherrandtext,packets_out);
    //hill_encrypt(num,packets,hillkey,packets_out);

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
    //TODO: CRITICAL: METADATA IS 001A002 AND HILL ENCRYPT IT MEANT TO HANDLE ONLY A-Z. ALSO CONTENTS OF PACKETS ARENT RIGHT.(so instead of 001, have it be aab or smthn like that k?)
    //      SOMEHOW, PACKET CONTENTS ARE ENDING IN .TXT AND ARE TOO LONG. (gemini said it might be improper null termination so fprintf isnt right.)
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
    char cipherjunktext[500];
    char packetnames[100][100];
    char packetpaths[100][513];
    int numpackets=getpacketnames(packetnames,seed);

    getpaths(packetpaths,packetnames,numpackets,seed);

    char packets[numpackets][26];
    
    //there is no need to order the packets we get since getpacketnames gets the names in the same order as it was encrypted.
    //maybe this is a security issue. TODO: check and fix if needed.
    getpackets(numpackets,packetpaths,packets); 
    openpackets(cipherjunktext,packets,numpackets);
    char ciphertext[200];
    removejunkfromstream(cipherjunktext,ciphertext);
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