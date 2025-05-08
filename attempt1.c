// FIX THIS ASAP
// ERROR: SHASHI's FILES ARE OVERWRITING SURAJ's FILES (users, not the people lol)
// ERROR: EVEN JUNK FILES OF A USER ARE OVERLAPPING WITH THE SAME USER.
// SO IT ONLY WORKS WITH ONE USER AND NO JUNK EITHER.
// TODO: encrypt the metadata and add a secodn layer of encryption to the whole thing.

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>

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

void writetofile(char *filename, char *contents){
    char temp[113];
    sprintf(temp,"storage/%s.txt",filename);
    FILE *pf=fopen(temp,"r");
    if(pf){printf("FILE IS REPEATING ERROR!");}
    FILE *pF=fopen(temp,"w");
    if (!pF){
        printf("File open error");
        exit(1);  // Fail fast
    }
    fprintf(pF, "%s", contents);
    fclose(pF);
}
void readcontents(char *filename, char *contents_out){
    char temp[113];
    sprintf(temp,"storage/%s.txt",filename);
    FILE *pF=fopen(temp,"r");
    char buffer[26];
    if (!pF){
        printf("File open error: %s",temp);
        exit(1);  // Fail fast
    }
    fgets(buffer, 26, pF); // only need to get once since file should only contain 26 characters.
    strcpy(contents_out,buffer);
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

void getpackets(int numpacks, char packetnames[100][100], char packets_out[numpacks][26]){
    for(int i=0;i<numpacks;i++){
        readcontents(packetnames[i],packets_out[i]);
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

void encrypt(char *plaintext, char *ciphertext_out, int *keystream, int len){ //this is a ceaser cipher btw.
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

int getuniquetouserseed(int *keystream, int len_of_keystream){ //gemini made this pls forgive me.
    if (keystream == NULL || len_of_keystream <= 0) {
        return 0;
    }
    long long int sum=0;
    for (int i = 0; i < len_of_keystream; i++) {
        // Add the keystream element. Modulo inside the loop helps prevent overflow
        // if keystream values or length are very large, keeping the sum within long long limits.
        // Using a large prime helps distribute the sum.
        sum = (sum + keystream[i]) % 99991; //99991 is a large uniqe number.
    }
    sum = (sum + len_of_keystream) % 99991;
    if (sum < 0) {
        sum += 99991; // Add the prime to make it positive
    }
    return ((int)sum);
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

int getpacketnames(char packetnames_out[][100],int seed){
    char letters[]="abcdefghijklmnopqrstuvwxyz0123456789";

    int numpackets=0;
    srand(seed+0);
    char firstpacketname[100];
    for(int j=0;j<100;j++){
        firstpacketname[j]=letters[rand()%strlen(letters)];
    }
    firstpacketname[99]='\0';
    //we get the first packet name this way as we know there must be atleast one packet.
    // but we dont know how many packets there are, so we dont know how many names to generate.
    // so we open the first packet and see the first 3 letters of metadata which tells us how many packets there are.

    char firstpacket[26];
    readcontents(firstpacketname, firstpacket);    
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

void writepacketsintofiles(char packetnames[][100],int numpacks,char packets[][26],char junknames[][100],int numjunk,char junk[][26], int *keystream, int len_of_keystream){
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
                writetofile(packetnames[temp], packets[temp]); //writing
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
                writetofile(junknames[temp], junk[temp]); //writing
                writtenjunk[temp]=1;
                countwrittenjunk++;
                }
            else{continue;}//packet is empty already, run loop again
        }
    }

}

void deletepackets(char packetnames[][100], int numpackets){
    for(int i=0;i<numpackets;i++){
        char temp[113];
        sprintf(temp,"storage/%s.txt",packetnames[i]);
        if (remove(temp) == 0) {
            printf("File deleted successfully.\n");
        } else {
            printf("Error: Unable to delete the file.\n");
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
    for(int i=0;i<30;i++){for(int j=0;j<26;j++){junk[i][j]='\0'; junknames[i][j]='\0';}}

    makepackets(ciphertext,packets);
    int len_of_junk=makejunk(junk);
    int junkkeylen=makejunkkeystream(junkkeystream);

    namepackets(packetnames,numpacks,seed);//needs to be reproducible
    namejunk(junknames,(int)ceil(len_of_junk/18.0),junkkeystream,junkkeylen); //doenst need to be reproducible, but shouldnt overwrite actual packets.
    //TODO: I was here.
    
    writepacketsintofiles(packetnames,numpacks,packets,junknames,(int)ceil(len_of_junk/18.0),junk,keystream,len_of_key);
    printf("Encrypted and Saved.");
}

void read(int *keystream, int len_of_key, int seed){
    char plaintext[100];
    char ciphertext[200];
    char packetnames[100][100];
    int numpackets=getpacketnames(packetnames,seed);
    char packets[numpackets][26];
    //there is no need to order the packets we get since getpacketnames gets the names in the same order as it was encrypted.
    //maybe this is a security issue. TODO: check and fix if needed.
    getpackets(numpackets,packetnames,packets);
    openpackets(ciphertext,packets,numpackets);
    decrypt(ciphertext, plaintext, keystream, len_of_key);
    printf("%s",plaintext);
}

void delete(int seed){ //YET TO TEST
    char packetnames[100][100];
    int numpackets=getpacketnames(packetnames,seed);
    deletepackets(packetnames,numpackets);
}

void edit(int *keystream, int len_of_key, int seed){ //YET TO TEST
    //TODO: This seems like reptetive code. see if you can replace it by calling signup.
    // the only reason i havent done it now is cause i dont want anything to be deleted until JUST before entering new contents.
    //we print old contents, then ask for new contents, then delete old contents and store new contents. 
    // (we also add new junk cause otherwise the attacker can see the date and time changed and know which files to open.)
    read(keystream, len_of_key, seed); //printing old contents
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
    delete(seed);
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

    // printf("Heres the issue: \n");
    // printf("First user's key is: \n");
    // strcpy(username,"suraj");
    // strcpy(password,"hello");
    // int len_of_key=makekey(username,password,keystream);
    // for(int i=0;i<len_of_key;i++){printf("%d",keystream[i]);}
    // printf("\nNow the second user's key is:\n");
    // strcpy(username,"shashi");
    // strcpy(password,"isnthere");
    // len_of_key=makekey(username,password,keystream);
    // for(int i=0;i<len_of_key;i++){printf("%d",keystream[i]);}
    // printf("\n the file names depend only on keystream. this causes one persons file to overwrite the other.");

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
        read(keystream,len_of_key, seed);
        break;
    case 'E':
        edit(keystream,len_of_key, seed);
        break;
    case 'D':
        delete(seed);
        break;
    default:
        printf("Invalid input.");
        break;
    }

    return 0;
}