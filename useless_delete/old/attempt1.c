// TODO: encrypt the metadata and add a secodn layer of encryption to the whole thing.

#include <stdio.h> // for printf and scanf
#include <string.h> //for strlen, strcpy
#include <math.h> //for ceil
#include <stdlib.h> //for rand functions
#include <time.h> //for time(0) when randomness needed.

#include <dirent.h> //-shashi //for scanning of subdirectories in linux(/posix) systems. (used in get_subdirectories)
#include <sys/stat.h> //-shashi //lets us access file/directory metadata like permissions and type. (used to differentiate bw directories and files in get_sibdirectories)

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

void inputstring(char *what, char *input_to_this_string){
    printf("Enter %s:",what);
    scanf("%s",input_to_this_string);
}

int makekey(char *usn, char *pwd, int *keystream_out){
    for(int i=0;i<((strlen(pwd)<strlen(usn))?strlen(pwd):strlen(usn));i++){
        srand((int)usn[i]*(int)pwd[i]);
        keystream_out[i]=(rand())%26;
    }
    return ((strlen(pwd)<strlen(usn))?strlen(pwd):strlen(usn));
}

int makejunkkeystream(int *keystream_out){
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

void writetofile(char *filepath, char *contents) { 

    //just checking for error
    FILE *check = fopen(filepath, "r");
    if (check) {
        fclose(check);
        printf("FILE IS REPEATING ERROR: %s\n", filepath);
        return;
    }
    
    FILE *pF = fopen(filepath, "w"); //just checking for error
    if (!pF) {
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

void makepackets(char *ciphertext, char packets_out[][26]){
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

void slice(char *sliced_out, char *to_slice, int from, int to){
    int count=0; //we assume size of sliced_out alloted is greater than or equal to (size of the slice)+1
    for(int i=from;i<=to;i++){
        sliced_out[count++]=to_slice[i];
    }
    sliced_out[count]='\0';
}

int sort_and_verify_packets(char packets[][26], int numpacks){
    char ogtotpacks[4];
    slice(ogtotpacks,packets[0],0,2);
    for(int i=0;i<numpacks;i++){
        char totpacks[4];
        slice(totpacks,packets[i],0,2);
        if (strcmp(totpacks,ogtotpacks) || packets[i][3]!=packets[0][3]){printf("ERROR VERIFYING PACKETS.");return 1;}
            for(int j=0;j >numpacks-i-1; j++){
                char nopackj[4],nopackj1[4];
                slice(nopackj,packets[j],4,6);
                slice(nopackj1,packets[j+1],4,6);
                if(atoi(nopackj) > atoi(nopackj1)){
                    char temp[26];
                    strcpy(temp,packets[j]);
                    strcpy(packets[j],packets[j+1]);
                    strcpy(packets[j+1],temp);
               }
            }
        }
    return 0;
}

void getpackets(int numpacks, char packetnames[100][513], char packets_out[numpacks][26]){
    for (int i = 0; i < numpacks; i++) {
        readcontents(packetnames[i], packets_out[i]);
    }
}

void openpackets(char *ciphertext_out, char packets[][26], int numpacks){
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

void encrypt(char *plaintext, char *ciphertext_out, int *keystream, int len){ //V basic and insecure.
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

int makejunk(char packets_out[][26]){
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
    int key[100];
    int len=makekey(plainrandtext,randtext,key);
    encrypt(plainrandtext,cipherrandtext,key,len);
    int num=(int)ceil(strlen(cipherrandtext)/18.0);
    makepackets(cipherrandtext,packets_out);
    return(rand_len);

}

void namepackets(char packetnames_out[][100],int numpackets,int seed){
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

void namejunk(char packetnames_out[][100],int numpackets,int *keystream, int len_of_keystream){
    //this is the same as the above function but if the file exists, we just generate another name, doesnt matter.
    //cause it doesnt need to be reproducible.
    char letters[]="abcdefghijklmnopqrstuvwxyz0123456789";
    unsigned long long int num_unique_to_user=1;
    for(int i=0;i<len_of_keystream;i++){num_unique_to_user*=(keystream[i]+1);} //avoids 0 multiplication.
    num_unique_to_user+=len_of_keystream;
    for(int i=0;i<numpackets;i++){
        //we need to name the packets in a recreatable yet seemingly random way so that we can fetch those packets again but the guy cant.
        //name should be dependent only on keystream (which comes from usn and password) and not on packet contents as we should be able to recreate to fetch the right packets.
        srand(num_unique_to_user/(keystream[i%len_of_keystream]+1)*(i+1)); //(avoids division/multiplication by 0)
        int unique_name=0; //keeps writing the name till a unique one found. 
        //yes i know this is inefficient, just roll with it for now.
        while(unique_name==0){
            for(int j=0;j<100;j++){
                packetnames_out[i][j]=letters[rand()%strlen(letters)];
            }
            packetnames_out[i][99]='\0';

            char temp[113];
            sprintf(temp,"storage/%s.txt",packetnames_out[i]);
            FILE *pF=fopen(temp,"r");
            if (!pF){ //meaning unique name created.
                unique_name=1;
            }
        }
    }
}

int getpacketnames(char packetnames_out[][100], int seed, int *keystream, int len_of_keystream) {
    char letters[] = "abcdefghijklmnopqrstuvwxyz0123456789";

    int numpackets = 0;
    srand(seed + 0);
    char firstpacketname[100];
    for (int j = 0; j < 100; j++) {
        firstpacketname[j] = letters[rand() % strlen(letters)];
    }
    firstpacketname[99] = '\0';
    char firstpacketpath[513];

    //
    char subdirs[100][256];
    int num_subdirs = get_subdirectories("storage", subdirs, 100); //gets (up to) 100 subdirs from storage.

    srand(seed);
    int subdir_index = rand() % num_subdirs;  //chooses a number and takes subdir[that num]
    sprintf(firstpacketpath,"storage/%s/%s.txt", subdirs[subdir_index], firstpacketname); //prints directory name into filepath   

    // Use keystream to read the first packet
    char firstpacket[26];

    readcontents(firstpacketpath, firstpacket); 
    //we get the first packet name this way as we know there must be atleast one packet.
    // but we dont know how many packets there are, so we dont know how many names to generate.
    // so we open the first packet and see the first 3 letters of metadata which tells us how many packets there are.

    // Get the number of packets from metadata
    char total_packets[4];
    slice(total_packets, firstpacket, 0, 2);
    numpackets = atoi(total_packets); //test this once. idk if it should be total_packets[4] or 3

    for (int i = 0; i < numpackets; i++) {
        //we need to name the packets in a recreatable yet seemingly random way so that we can fetch those packets again but the guy cant.
        //name should be dependent only on keystream (which comes from usn and password) and not on packet contents as we should be able to recreate to fetch the right packets.
        srand(seed + i);  // ensure reproducibility
        for (int j = 0; j < 100; j++) {
            packetnames_out[i][j] = letters[rand() % strlen(letters)];
        }
        packetnames_out[i][99] = '\0';
    }
    return numpackets;
}

void writepacketsintofiles(char packetpath[][513],int numpacks,char packets[][26],char junkpath[][513],int numjunk,char junk[][26], int *keystream, int len_of_keystream){
    //yes, I know this is V inneficient, just roll with it for this version. consider using a boolean array
    int writtenpackets[numpacks];
    for(int k=0;k<numpacks;k++){writtenpackets[k]=0;}
    int writtenjunk[numjunk];
    for(int k=0;k<numjunk;k++){writtenjunk[k]=0;}
    srand(keystream[((int)junk[2][19])%len_of_keystream]); //gives a random seed. fixed using seed in future versions.
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
                writetofile(packetpath[temp], packets[temp]);
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
                writetofile(junkpath[temp],junk[temp]);
                writtenjunk[temp]=1;
                countwrittenjunk++;
                }
            else{continue;}//packet is empty already, run loop again
        }
    }

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

void signup(int *keystream, int len_of_key, int seed){
    char plaintext[100];
    char ciphertext[200];
    inputstring("Contents",plaintext);
    encrypt(plaintext,ciphertext,keystream,len_of_key);

    int numpacks=(int)ceil(strlen(ciphertext)/18.0);
    char packets[numpacks][26];
    char packetnames[numpacks][100];
    char junknames[30][100];
    char junk [30][26]; 
    int junkkeystream[100];
    char packetpaths[numpacks][513]; //idk why its 513 btw.
    char junkpaths[30][513];
    for(int i=0;i<30;i++){for(int j=0;j<26;j++){junk[i][j]='\0'; junknames[i][j]='\0';}}

    makepackets(ciphertext,packets);
    int len_of_junk=makejunk(junk);
    int junkkeylen=makejunkkeystream(junkkeystream);

    namepackets(packetnames,numpacks,seed);//needs to be reproducible
    namejunk(junknames,(int)ceil(len_of_junk/18.0),junkkeystream,junkkeylen); //doenst need to be reproducible, but shouldnt overwrite actual packets.
    int junkseed=getuniquetouserseed(junkkeystream,junkkeylen);
    getpaths(packetpaths,packetnames,numpacks,seed);
    getpaths(junkpaths,junknames,numpacks,junkseed);

    writepacketsintofiles(packetpaths,numpacks,packets,junkpaths,(int)ceil(len_of_junk/18.0),junk,keystream,len_of_key);
    printf("Encrypted and Saved.");
}

void my_read(int *keystream, int len_of_key, int seed) {
    char plaintext[100];
    char ciphertext[200];
    char packetnames[100][100];
    char packetpaths[100][513];

    int numpackets = getpacketnames(packetnames, seed, keystream, len_of_key);

    getpaths(packetpaths,packetnames,numpackets,seed);

    char packets[numpackets][26];
    //there is no need to order the packets we get since getpacketnames gets the names in the same order as it was encrypted.
    //maybe this is a security issue. TODO: check and fix if needed.
    getpackets(numpackets, packetpaths, packets); 
    openpackets(ciphertext, packets, numpackets);
    decrypt(ciphertext, plaintext, keystream, len_of_key);
    printf("%s", plaintext);
}



void delete(int *keystream, int len_of_key, int seed) { //YET TO TEST
    char packetnames[100][100];
    char packetpaths[100][513];
    int numpackets = getpacketnames(packetnames, seed, keystream, len_of_key); 
    getpaths(packetpaths,packetnames,numpackets,seed);
    deletepackets(packetpaths, numpackets);
}

void edit(int *keystream, int len_of_key, int seed){ //YET TO TEST
    //TODO: This seems like reptetive code. see if you can replace it by calling signup.
    // the only reason i havent done it now is cause i dont want anything to be deleted until JUST before entering new contents.
    //we print old contents, then ask for new contents, then delete old contents and store new contents. 
    // (we also add new junk cause otherwise the attacker can see the date and time changed and know which files to open.)
    my_read(keystream, len_of_key, seed); //printing old contents
    printf("\n");

    // //taking in new contents
    // char plaintext[100];
    // char ciphertext[100];
    // inputstring("new contents",plaintext);
    // encrypt(plaintext,ciphertext,keystream,len_of_key);

    // int numpacks=(int)ceil(strlen(ciphertext)/18.0);
    // char packets[numpacks][26];
    // char packetnames[numpacks][100];
    // char junknames[100][100];
    // char junk [6][26]; 
    // int junkkeystream[100];
    // for(int i=0;i<100;i++){for(int j=0;j<26;j++){junk[i][j]='\0'; junknames[i][j]='\0';}}

    // makepackets(ciphertext,packets);
    // int len_of_junk=makejunk(junk);
    // int junkkeylen=makejunkkeystream(junkkeystream);

    // namepackets(packetnames,numpacks,keystream,len_of_key);
    // namepackets(junknames,(int)ceil(len_of_junk/18.0),junkkeystream,junkkeylen);

    //deleting old contents
    delete(keystream, len_of_key, seed); //edited by shashi to pass nessecary stuff
    printf("Deleted old files\n");

    signup(keystream, len_of_key, seed);
    // writepacketsintofiles(packetnames,numpacks,packets,junknames,(int)ceil(len_of_junk/18.0),junk,keystream,len_of_key);
    printf("Encrypted and Saved.");

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
        delete(keystream, len_of_key, seed); //updated by shashi to pass whats needed
        break;
    default:
        printf("Invalid input.");
        break;
    }

    return 0;
}