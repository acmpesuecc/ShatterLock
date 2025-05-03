//Needs:
//Encryption and decryption functions. text goes in, text goes out.
// function to make and operate on text document or dat document TODO:
//signup, read, edit and delete functions   TODO:
//function to split up encrypted text with correct prefixes and make packets. TODO: encrypt the metadata and add a secodn layer of encryption to the whole thing.
// till shashi gets here, a function to distribute packets among that folder
//similarly to piece packets back

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <time.h> // Needed for time(0)
// #include <sys/stat.h> // Needed for mkdir (if using)
// #include <sys/types.h> // Needed for mkdir (if using)

void inputstring(char *what, char *input_to_this_string){
    printf("Enter %s:",what);
    // Using scanf("%s", ...) can be dangerous for buffer overflows.
    // For simplicity in this example, we'll keep it, but be aware.
    // A safer alternative is fgets.
    scanf("%s",input_to_this_string);
}

int makekey(char *usn, char *pwd, int *keystream_out){
    int len = (strlen(pwd) < strlen(usn)) ? strlen(pwd) : strlen(usn);
    // Add a check to ensure len is positive
    if (len <= 0) return 0;

    for(int i = 0; i < len; i++){
        // Ensure characters are lowercase letters for simple modulo arithmetic
        // Assuming usn and pwd contain only lowercase letters 'a'-'z'
        // Add checks to handle non-lowercase characters if necessary
        if (usn[i] >= 'a' && usn[i] <= 'z' && pwd[i] >= 'a' && pwd[i] <= 'z') {
             keystream_out[i] = (((int)usn[i] - 'a') * ((int)pwd[i] - 'a')) % 26;
        } else {
            // Handle cases where characters are not lowercase letters
            // For now, we'll set the keystream value to 0, but this affects security.
            keystream_out[i] = 0;
        }
    }
    return len;
}

int makejunkkeystream(int *keystream_out){
    char randtext[]="philosophyofeducationisalabelappliedtothestudyofthepurposeprocessnatureandideals"
                    "ofeducationitcanbeconsideredabranchofbothphilosophyandeducationeducationcanbedefined"
                    "astheteachingandlearningofspecificskillsandtheimpartingofknowledgejudgmentandwisdom"
                    "andissomethingbroaderthanthesocietalinstitutionofeducationweoftenspeakofmanyeducationalists"
                    "consideritaweakandwoollyfieldtoofarremovedfromthepracticalapplicationsoftherealworld"
                    "tobeusefulbutphilosophersdatingbacktoplatoandtheancientgreekshavegiventheareamuchthought"
                    "andemphasisandthereislittledoubtthattheirworkhashelpedshapethepracticeofeducation"
                    "overthemillenniaplatoistheearliestimportanteducationalthinkerandeducationisanessential"
                    "elementintherepublichismostimportantworkonphilosophyandpoliticaltheorywrittenaroundbcinit"
                    "headvocatessomeratherextrememethodsremovingchildrenfromtheirmotherscareandraisingthemaswardsofthestate"
                    "anddifferentiatingchildrensuitabletothevariouscastesthehighestreceivingthemosteducationsothat"
                    "theycouldactasguardiansofthecityandcareforthelessablehebelievedthateducationshouldbeholistic"
                    "includingfactsskillsphysicaldisciplinemusicandartplatobelievedthattalentandintelligenceisnot"
                    "distributedgeneticallyandthusisbefoundinchildrenborntoallclassesalthoughhisproposedsystemofselective"
                    "publiceducationforaneducatedminorityofthepopulationdoesnotreallyfollowademocraticmodelaristotle"
                    "consideredhumannaturehabitandreasontobeequallyimportantforcestobecultivatedineducationtheultimateaim"
                    "ofwhichshouldbetoproducegoodandvirtuouscitizensheproposedthatteachersleadtheirstudentssystematically"
                    "andthatrepetitionbeusedasakeytooltodevelopgoodhabitsunlikesocratesemphasisonquestioninghislistenersto"
                    "bringouttheirownideasheemphasizedthebalancingofthetheoreticalandpracticalaspectsofsubjectstaughtamong"
                    "whichheexplicitlymentionsreadingwritingmathematicsmusicphysicaleducationliteraturehistoryandawiderange"
                    "ofsciencesaswellasplaywhichhealsoconsideredimportantduringthemedievalperiodtheideaofperennialismwasfirst"
                    "formulatedbystthomasaquinashisinworkdemagistroperennialismholdsthatoneshouldteachthosethingsdeemedtobeof"
                    "everlastingimportancetoallpeopleeverywherenamelyprinciplesandreasoningnotjustfactswhichareapttochange"
                    "overtimeandthatoneshouldteachfirstaboutpeoplenotmachinesortechniquesitwasoriginallyreligiousinnature"
                    "anditwasonlymuchlaterthatatheoryofsecularperennialismdevelopedduringtherenaissancethefrenchskepticmicheldemontaigne"
                    "wasoneofthefirsttocriticallylookateducationunusuallyforhistimemontaignewaswillingtoquestiontheconventional"
                    "wisdomoftheperiodcallingintoquestionthewholeedificeoftheeducationalsystemandtheimplicitassumptionthat"
                    "universityeducatedphilosopherswerenecessarilywiserthanuneducatedfarmworkersforexample";

    char randusn[100];
    char randpwd[100];
    // srand(time(0)); // REMOVE THIS LINE - srand should be called once in main

    randusn[0] = '\0';
    randpwd[0] = '\0';

    // Ensure there's enough text to pick 100 characters from
    if (strlen(randtext) < 200) {
        fprintf(stderr, "Error: Not enough text in randtext for makejunkkeystream.\n");
        return 0; // Indicate failure
    }

    int rand_start = rand() % (strlen(randtext) - 100); // ensure at least 100 chars available
    int rand_len = 49 + rand() % 50; // copy 49 to 98 characters
    strncpy(randusn, randtext + rand_start, rand_len);
    randusn[rand_len] = '\0';

    rand_start = rand() % (strlen(randtext) - 100);
    rand_len = 49 + rand() % 50;
    strncpy(randpwd, randtext + rand_start, rand_len);
    randpwd[rand_len]='\0';

    int len = (strlen(randpwd) < strlen(randusn)) ? strlen(randpwd) : strlen(randusn);
    if (len <= 0) return 0;

     for(int i = 0; i < len; i++){
        // Ensure characters are lowercase letters
         if (randusn[i] >= 'a' && randusn[i] <= 'z' && randpwd[i] >= 'a' && randpwd[i] <= 'z') {
            keystream_out[i] = (((int)randusn[i] - 'a') * ((int)randpwd[i] - 'a')) % 26;
         } else {
             keystream_out[i] = 0; // Handle non-lowercase
         }
    }
    return len;
}

void writetofile(char *filename, char *contents){
    char temp[100];
    // Using snprintf is safer than sprintf to prevent buffer overflows
    snprintf(temp, sizeof(temp), "storage/%s.txt", filename);

    FILE *pF=fopen(temp,"w"); // "w" mode truncates the file
    if (!pF){
        // Use stderr for error messages
        fprintf(stderr, "File open error for %s\n", temp);
        // Consider returning an error code instead of exiting
        // exit(1); // Fail fast - might be too aggressive
        return; // Indicate failure to the caller
    }
    fprintf(pF, "%s", contents);
    fclose(pF);
}

void makepackets(char *ciphertext, char packets_out[][26]){
    int j = 0; // Packet index
    int ciphertext_len = strlen(ciphertext);
    if (ciphertext_len == 0) return; // Nothing to packetize

    int num_total_packets = (int)ceil((double)ciphertext_len / 18.0);

    char temp[26];
    int counter = 7; // Start writing ciphertext content at index 7

    for(int i = 0; i < ciphertext_len; i++){
        if(counter == 7) { // Start of a new packet
            memset(temp, '\0', sizeof(temp)); // Clear temp buffer
            // Metadata: num_of_packets (3 digits), identifying attribute (1 char), packet_num (3 digits)
            // Assuming ciphertext[0] is a valid identifying attribute
            sprintf(temp, "%03d%c%03d", num_total_packets, ciphertext[0], j + 1);
            counter = 7; // Reset counter after writing metadata
        }

        temp[counter++] = ciphertext[i];

        if(counter == 25 || i == ciphertext_len - 1){ // Packet is full or it's the last character
            temp[25] = '\0'; // Ensure null termination
            // Check if j is within bounds before copying
            if (j < 100) { // Assuming packets_out has 100 rows
               strcpy(packets_out[j++], temp);
            } else {
               fprintf(stderr, "Warning: More than 100 packets generated. Some data may be lost.\n");
               return; // Stop if we exceed allocated space
            }
        }
    }
}


void slice(char *sliced_out, char *to_slice, int from, int to){
    int count = 0;
    // Add bounds checking
    int to_slice_len = strlen(to_slice);
    if (from < 0 || to < from || from >= to_slice_len) {
        sliced_out[0] = '\0'; // Return empty string on invalid slice
        return;
    }

    for(int i = from; i <= to && i < to_slice_len; i++){
        sliced_out[count++] = to_slice[i];
    }
    sliced_out[count] = '\0'; // Null terminate
}

int sort_and_verify_packets(char packets[][26], int numpacks){
    if (numpacks <= 0) return 0; // Nothing to sort or verify

    char ogtotpacks[4];
    slice(ogtotpacks, packets[0], 0, 2);

    // Simple bubble sort for demonstration - not efficient for large numpacks
    for(int i = 0; i < numpacks - 1; i++){
        char totpacks_i[4];
        slice(totpacks_i, packets[i], 0, 2);
        char id_char_i = packets[i][3];

        // Verify metadata consistency during sorting
        if (strcmp(totpacks_i, ogtotpacks) != 0 || id_char_i != packets[0][3]) {
            fprintf(stderr, "Verification failed: Inconsistent total packets or identifying attribute.\n");
            return 1; // Verification failed
        }

        for(int j = 0; j < numpacks - i - 1; j++){
            char nopackj[4], nopackj1[4];
            slice(nopackj, packets[j], 4, 6);
            slice(nopackj1, packets[j+1], 4, 6);

            // Convert packet numbers to integers for comparison
            if(atoi(nopackj) > atoi(nopackj1)){
                // Swap packets
                char temp[26];
                strcpy(temp, packets[j]);
                strcpy(packets[j], packets[j+1]);
                strcpy(packets[j+1], temp);
            }
        }
    }

    // Final check on the last packet after sorting
    if (numpacks > 0) {
         char totpacks_last[4];
        slice(totpacks_last, packets[numpacks-1], 0, 2);
        char id_char_last = packets[numpacks-1][3];
         if (strcmp(totpacks_last, ogtotpacks) != 0 || id_char_last != packets[0][3]) {
            fprintf(stderr, "Verification failed: Inconsistent metadata in the last packet.\n");
            return 1; // Verification failed
        }
        // Also check if the last packet number is numpacks
        char last_packet_num_str[4];
        slice(last_packet_num_str, packets[numpacks-1], 4, 6);
        if (atoi(last_packet_num_str) != numpacks) {
             fprintf(stderr, "Verification failed: Last packet number does not match total number of packets.\n");
             return 1; // Verification failed
        }
    }


    return 0; // Verification successful
}

void openpackets(char *ciphertext_out, char packets[][26], int numpacks){
    int count = 0;
    ciphertext_out[0] = '\0'; // Start with an empty string

    for(int i = 0; i < numpacks; i++){
        // Start from index 7 to skip metadata
        for(int j = 7; j < 26; j++){
            // Only append if it's a valid lowercase letter and not null terminator
            if (packets[i][j] != '\0' && packets[i][j] >= 'a' && packets[i][j] <= 'z'){
                ciphertext_out[count++] = packets[i][j];
            } else if (packets[i][j] == '\0') {
                 // If we hit a null terminator within the data section, stop processing this packet
                 break;
            }
        }
    }
    ciphertext_out[count] = '\0'; // Null terminate the final string
}

void encrypt(char *plaintext, char *ciphertext_out, int *keystream, int len){
    int plaintext_len = strlen(plaintext);
    for(int i = 0; i < plaintext_len; i++){
        // Ensure plaintext character is a lowercase letter
        if (plaintext[i] >= 'a' && plaintext[i] <= 'z') {
             // Perform Caesar cipher with keystream
            ciphertext_out[i] = (char)(((((int)plaintext[i] - 'a') + keystream[i % len]) % 26) + 'a');
        } else {
            // Handle non-lowercase letters (e.g., copy directly or skip)
            // For now, let's just copy them directly
            ciphertext_out[i] = plaintext[i];
        }
    }
    ciphertext_out[plaintext_len] = '\0'; // Null terminate
}

void decrypt(char *ciphertext, char *plaintext_out, int *keystream, int len){
     int ciphertext_len = strlen(ciphertext);
    for(int i = 0; i < ciphertext_len; i++){
        // Ensure ciphertext character is a lowercase letter
         if (ciphertext[i] >= 'a' && ciphertext[i] <= 'z') {
            // Perform decryption
            plaintext_out[i] = (char)(((((int)ciphertext[i] - 'a') - keystream[i % len] + 26) % 26) + 'a');
         } else {
            // Handle non-lowercase letters (should match encryption)
            plaintext_out[i] = ciphertext[i];
         }
    }
    plaintext_out[ciphertext_len] = '\0'; // Null terminate
}

void makejunk(char packets_out[][26]){
    char randtext[]="philosophyofeducationisalabelappliedtothestudyofthepurposeprocessnatureandideals"
                    "ofeducationitcanbeconsideredabranchofbothphilosophyandeducationeducationcanbedefined"
                    "astheteachingandlearningofspecificskillsandtheimpartingofknowledgejudgmentandwisdom"
                    "andissomethingbroaderthanthesocietalinstitutionofeducationweoftenspeakofmanyeducationalists"
                    "consideritaweakandwoollyfieldtoofarremovedfromthepracticalapplicationsoftherealworld"
                    "tobeusefulbutphilosophersdatingbacktoplatoandtheancientgreekshavegiventheareamuchthought"
                    "andemphasisandthereislittledoubtthattheirworkhashelpedshapethepracticeofeducation"
                    "overthemillenniaplatoistheearliestimportanteducationalthinkerandeducationisanessential"
                    "elementintherepublichismostimportantworkonphilosophyandpoliticaltheorywrittenaroundbcinit"
                    "headvocatessomeratherextrememethodsremovingchildrenfromtheirmotherscareandraisingthemaswardsofthestate"
                    "anddifferentiatingchildrensuitabletothevariouscastesthehighestreceivingthemosteducationsothat"
                    "theycouldactasguardiansofthecityandcareforthelessablehebelievedthateducationshouldbeholistic"
                    "includingfactsskillsphysicaldisciplinemusicandartplatobelievedthattalentandintelligenceisnot"
                    "distributedgeneticallyandthusisbefoundinchildrenborntoallclassesalthoughhisproposedsystemofselective"
                    "publiceducationforaneducatedminorityofthepopulationdoesnotreallyfollowademocraticmodelaristotle"
                    "consideredhumannaturehabitandreasontobeequallyimportantforcestobecultivatedineducationtheultimateaim"
                    "ofwhichshouldbetoproducegoodandvirtuouscitizensheproposedthatteachersleadtheirstudentssystematically"
                    "andthatrepetitionbeusedasakeytooltodevelopgoodhabitsunlikesocratesemphasisonquestioninghislistenersto"
                    "bringouttheirownideasheemphasizedthebalancingofthetheoreticalandpracticalaspectsofsubjectstaughtamong"
                    "whichheexplicitlymentionsreadingwritingmathematicsmusicphysicaleducationliteraturehistoryandawiderange"
                    "ofsciencesaswellasplaywhichhealsoconsideredimportantduringthemedievalperiodtheideaofperennialismwasfirst"
                    "formulatedbystthomasaquinashisinworkdemagistroperennialismholdsthatoneshouldteachthosethingsdeemedtobeof"
                    "everlastingimportancetoallpeopleeverywherenamelyprinciplesandreasoningnotjustfactswhichareapttochange"
                    "overtimeandthatoneshouldteachfirstaboutpeoplenotmachinesortechniquesitwasoriginallyreligiousinnature"
                    "anditwasonlymuchlaterthatatheoryofsecularperennialismdevelopedduringtherenaissancethefrenchskepticmicheldemontaigne"
                    "wasoneofthefirsttocriticallylookateducationunusuallyforhistimemontaignewaswillingtoquestiontheconventional"
                    "wisdomoftheperiodcallingintoquestionthewholeedificeoftheeducationalsystemandtheimplicitassumptionthat"
                    "universityeducatedphilosopherswerenecessarilywiserthanuneducatedfarmworkersforexample";

    char plainrandtext[100];
    // srand(time(0)); // REMOVE THIS LINE - srand should be called once in main

    plainrandtext[0] = '\0';
     if (strlen(randtext) < 200) {
        fprintf(stderr, "Error: Not enough text in randtext for makejunk.\n");
        return; // Indicate failure
    }

    int rand_start = rand() % (strlen(randtext) - 100); // ensure at least 100 chars available
    int rand_len = 49 + rand() % 50; // copy 49 to 98 characters
    strncpy(plainrandtext, randtext + rand_start, rand_len);
    plainrandtext[rand_len] = '\0';

    char cipherrandtext[100];
    int key[100];
    // makejunk uses makekey which uses randtext and plainrandtext for key generation.
    // It might be better to use a separate, truly random key for junk.
    // For now, keeping the existing logic but noting potential improvement.
    int len = makekey(plainrandtext, randtext, key); // This makekey uses rand() internally via makejunkkeystream

    // Encrypt the random text
    encrypt(plainrandtext, cipherrandtext, key, len);

    // Create junk packets from the encrypted random text
    // This assumes cipherrandtext will fit into 100 packets of size 26-7=19 chars
    // strlen(cipherrandtext) <= 100 * 19 = 1900. Current size is max 99 chars, so it fits.
    makepackets(cipherrandtext, packets_out);
}

void namepackets(char packetnames_out[][100], int numpackets, int *keystream, int len_of_keystream){
    char letters[] = "abcdefghijklmnopqrstuvwxyz0123456789";
    int letters_len = strlen(letters);

    if (len_of_keystream <= 0) {
         fprintf(stderr, "Error: Keystream length is zero or negative in namepackets.\n");
         return;
    }

    for(int i = 0; i < numpackets; i++){
        // Seed rand based on the keystream and packet index for deterministic naming
        // This seed will be the same each time with the same keystream and index.
        // This is crucial for retrieving packets later.
        // Using a combination of keystream value and index for a more unique seed per packet name
        srand(keystream[i % len_of_keystream] + i + (unsigned int)time(0)); // Added time(0) for more variation if called multiple times in a program run

        for(int j = 0; j < 99; j++){ // Generate 99 characters for the name
            packetnames_out[i][j] = letters[rand() % letters_len];
        }
        packetnames_out[i][99] = '\0'; // Null terminate
    }
}

void writepacketsintofiles(char packetnames[][100], int numpacks, char packets[][26], char junknames[][100], int numjunk, char junk[][26]){
    // Arrays to track if a packet/junk item has been written
    int writtenpackets[numpacks];
    for(int k = 0; k < numpacks; k++){
        writtenpackets[k] = 0; // 0 means not written
    }
    int writtenjunk[numjunk];
    for(int k = 0; k < numjunk; k++){
        writtenjunk[k] = 0; // 0 means not written
    }

    int countwrittenpacks = 0;
    int countwrittenjunk = 0;

    // Loop as long as there are packets OR junk left to write
    while(countwrittenpacks < numpacks || countwrittenjunk < numjunk){
        // Decide whether to try writing a packet or junk based on random chance
        // and whether there are items of that type still unwritten.
        int try_packet = 0;
        int try_junk = 0;

        if (countwrittenpacks < numpacks && countwrittenjunk < numjunk) {
            // If both types are available, randomly choose
            if (rand() % 2 == 1) {
                try_packet = 1;
            } else {
                try_junk = 1;
            }
        } else if (countwrittenpacks < numpacks) {
            // Only packets are left, must try writing a packet
            try_packet = 1;
        } else if (countwrittenjunk < numjunk) {
            // Only junk is left, must try writing junk
            try_junk = 1;
        } else {
            // Should not happen if the while condition is correct, but as a safeguard
            break; // All items are written
        }

        if (try_packet) {
            // Find and write an unwritten packet
            while(1) { // Loop until an unwritten packet is found
                int temp = rand() % numpacks;
                if (writtenpackets[temp] == 0) { // If this packet hasn't been written
                    writetofile(packetnames[temp], packets[temp]); // Write it
                    writtenpackets[temp] = 1; // Mark as written
                    countwrittenpacks++; // Increment the count of written packets
                    break; // Exit the inner loop
                }
            }
        } else if (try_junk) {
            // Find and write unwritten junk
             while(1) { // Loop until unwritten junk is found
                int temp = rand() % numjunk;
                if (writtenjunk[temp] == 0) { // If this junk hasn't been written
                    writetofile(junknames[temp], junk[temp]); // Write it
                    writtenjunk[temp] = 1; // Mark as written
                    countwrittenjunk++; // Increment the count of written junk
                    break; // Exit the inner loop
                }
            }
        }
         // If neither try_packet nor try_junk is set (shouldn't happen with correct logic),
         // the outer loop condition will eventually become false.
    }
}


int main(){
    // Seed the random number generator ONCE at the beginning of main
    srand(time(0));

    // Create the storage directory if it doesn't exist
    // This is OS-dependent. For Linux/macOS, you can use:
    // system("mkdir -p storage");
    // For Windows, you might need:
    // system("mkdir storage");
    // Or use platform-independent C functions like mkdir (requires <sys/stat.h> and <sys/types.h>)
    // For simplicity here, assume the 'storage' directory exists or create it manually.
    // fprintf(stdout, "Please ensure a directory named 'storage' exists in the same folder as the executable.\n");


    char plaintext[100];
    char ciphertext[100]; // Needs to be large enough to hold encrypted plaintext
    char username[100];
    char password[100];
    int keystream[100]; // Keystream size should be at least the minimum of usn/pwd length

    inputstring("Plaintext (lowercase letters only)", plaintext);
    inputstring("Username (lowercase letters only)", username);
    inputstring("Password (lowercase letters only)", password);

    int len_of_key = makekey(username, password, keystream);
    // Consider clearing username and password from memory after key generation
    // For demonstration, we'll skip this, but it's good practice for security.
    // memset(username, 0, sizeof(username));
    // memset(password, 0, sizeof(password));

    encrypt(plaintext, ciphertext, keystream, len_of_key);

    // Calculate numpacks based on the actual ciphertext length
    int numpacks = (strlen(ciphertext) > 0) ? (int)ceil((double)strlen(ciphertext) / 18.0) : 0;

    if (numpacks == 0) {
        printf("Plaintext resulted in empty ciphertext or is too short. No packets to write.\n");
        return 0; // Exit if nothing to write
    }

    // Allocate space for packets and packet names. Max 100 packets for now.
    // Consider dynamic allocation if numpacks can exceed 100 significantly.
    if (numpacks > 100) {
         fprintf(stderr, "Warning: Number of packets (%d) exceeds the allocated space (100). Data will be lost.\n", numpacks);
         numpacks = 100; // Cap at 100 to prevent buffer overflow
    }
    char packets[100][26]; // Using fixed size 100 to match junknames/junk
    char packetnames[100][100];

    int numjunk = 100; // Fixed number of junk packets
    char junknames[100][100];
    char junk [100][26];

    // Initialize junk packets to zeros
    for(int i = 0; i < numjunk; i++){
        memset(junk[i], '\0', sizeof(junk[i]));
    }

    int junkkeystream[100]; // Keystream for naming junk packets

    // Generate junk packets (this also calls makejunkkeystream internally)
    makejunk(junk); // makejunk now generates the junk packets directly

    // Generate a separate keystream for naming junk packets if needed,
    // or use the main keystream if that's the design.
    // Let's generate a separate one using makejunkkeystream as in the original code.
    int junklen = makejunkkeystream(junkkeystream); // makejunkkeystream should NOT seed srand

    // Name the actual packets and junk packets
    namepackets(packetnames, numpacks, keystream, len_of_key);
    namepackets(junknames, numjunk, junkkeystream, junklen);

    // Write packets and junk to files
    writepacketsintofiles(packetnames, numpacks, packets, junknames, numjunk, junk);

    printf("Writing process finished. Check the 'storage' directory.\n");

    // Keep the commented out sorting/unpacking for later development
    // int error=sort_and_verify_packets(packets,numpacks); //sorts packets recieved before unpacking
    // openpackets(ciphertext,packets,numpacks); //unpackets into ciphertext
    // printf("\n %d %s\n\n",error,ciphertext);
    // for(int i=0;i<numpacks;i++){printf("%s ",packets[i]);} // Use numpacks here
    // printf("\n");
    // for(int i=0;i<numjunk;i++){if(junk[i][0]!='\0'){printf("%s ",junk[i]);}} // Use numjunk here


    return 0;
}
