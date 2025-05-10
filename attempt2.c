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
// TODO: encrypt the metadata and add a secodn layer of encryption to the whole thing. (layers and layered metadata encryption?)
// TODO: once the encryption is better, encrypt it in layers using key 1, 2 and keystream instead of 1 tempkeystream like here.
//in a future version, have it such that one user can have many contents

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>

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

void getusefulchars(char *with_junk_in ,char *useful_out)// takes every other character only.
//please note that this has no cryptographic value. it is only to increase the number of packets.
//this is ovious from the fact that it is not relying on the key.
//TODO: Take a call on wether this is actually useful or if the number of packets dont matter.
{}

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

void encrypt(char *plaintext, char *ciphertext_out, int *keystream, int len){ //going with the same encryption as before
    for(int i=0;i<strlen(plaintext);i++){
        ciphertext_out[i]=(char)(((((int)plaintext[i])+keystream[i%len])+26-97)%26)+97;
    }
    ciphertext_out[strlen(plaintext)]='\0'; //null_terminating
}


void decrypt(char *ciphertext, char *plaintext_out, int *keystream, int len){ //same decryption algorithm as before.
    for(int i=0;i<strlen(ciphertext);i++){
        plaintext_out[i]=(char)(((((int)ciphertext[i])-keystream[i%len])+26-97)%26)+97;
    }
    plaintext_out[strlen(ciphertext)]='\0'; //null_terminating
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

int makejunk(char packets_out[][26]){ //same as before
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

void namejunk(char packetnames_out[][100],int numpackets,int *keystream, int len_of_keystream){ //same as before
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

void writepacketsintofiles(char packetnames[][100],int numpacks,char packets[][26],char junknames[][100],int numjunk,char junk[][26], int *keystream, int len_of_keystream){ //same as before
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


void handle_encryption_tasks(char *plaintext, int *key_given, int len_of_key_given, int seed_passed){
    char ciphertext[200];

    encrypt(plaintext,ciphertext,key_given,len_of_key_given);

    // now distributing contents based on all three seeds.
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

    namepackets(packetnames,numpacks,seed_passed);//needs to be reproducible, based on all seed
    namejunk(junknames,(int)ceil(len_of_junk/18.0),junkkeystream,junkkeylen); //doenst need to be reproducible, but shouldnt overwrite actual packets.
    
    writepacketsintofiles(packetnames,numpacks,packets,junknames,(int)ceil(len_of_junk/18.0),junk,key_given,len_of_key_given);
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

void getpackets(int numpacks, char packetnames[100][100], char packets_out[numpacks][26]){ //same as before
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

void getfullplaintext(int *keystream, int len_of_key, int seed, char *plaintext_out){
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
    strcpy(plaintext_out,plaintext);
}

void read(int *keystream, int len_of_key, int seed){
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
    int numpackets=getpacketnames(packetnames,(seed*seed1*seed2)); //deletes contents
    deletepackets(packetnames, numpackets);

    numpackets=getpacketnames(packetnames,(seed*seed1)); //deletes key2
    deletepackets(packetnames, numpackets);

    numpackets=getpacketnames(packetnames,(seed)); //deletes key1
    deletepackets(packetnames, numpackets);

}

void edit(int *keystream, int len_of_key, int seed){
    read(keystream, len_of_key, seed);
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
        read(keystream,len_of_key, seed);
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