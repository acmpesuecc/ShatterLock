//Needs:
//Encryption and decryption functions. text goes in, text goes out. 
// function to make and operate on text document or dat document TODO:
//signup, read, edit and delete functions   TODO:
//function to split up encrypted text with correct prefixes and make packets. TODO: encrypt the metadata and add a secodn layer of encryption to the whole thing.
// till shashi gets here, a function to distribute packets among that folder
//similarly to piece packets back

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
        keystream_out[i]=((int)usn[i]*(int)pwd[i])%26;
    }
    return ((strlen(pwd)<strlen(usn))?strlen(pwd):strlen(usn));
}

void writetofile(char *filename, char *contents){
    char temp[100];
    sprintf(temp,"%s.txt",filename);
    FILE *pF=fopen(temp,"w");
    char buffer[255];
    if (pF){
        fprintf(pF, "%s", contents);
        fclose(pF);
        printf("\nall ok\n");
    }
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
    printf("sliced: %s\n",sliced_out);
}

int sort_and_verify_packets(char packets[][26], int numpacks){
    char ogtotpacks[4];
    slice(ogtotpacks,packets[0],0,2);
    for(int i=0;i<numpacks;i++){
        char totpacks[4];
        slice(totpacks,packets[i],0,2);
        if (strcmp(totpacks,ogtotpacks) || packets[i][3]!=packets[0][3]){return 1;}
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

void makejunk(char packets_out[][26]){
    char randtext[]="philosophyofeducationisalabelappliedtothestudyofthepurposeprocessnatureandidealsofeducationitcanbeconsideredabranchofbothphilosophyandeducationeducationcanbedefinedastheteachingandlearningofspecificskillsandtheimpartingofknowledgejudgmentandwisdomandissomethingbroaderthanthesocietalinstitutionofeducationweoftenspeakofmanyeducationalistsconsideritaweakandwoollyfieldtoofarremovedfromthepracticalapplicationsoftherealworldtobeusefulbutphilosophersdatingbacktoplatoandtheancientgreekshavegiventheareamuchthoughtandemphasisandthereislittledoubtthattheirworkhashelpedshapethepracticeofeducationoverthemillenniaplatoistheearliestimportanteducationalthinkerandeducationisanessentialelementintherepublichismostimportantworkonphilosophyandpoliticaltheorywrittenaroundbcinitheadvocatessomeratherextrememethodsremovingchildrenfromtheirmotherscareandraisingthemaswardsofthestateanddifferentiatingchildrensuitabletothevariouscastesthehighestreceivingthemosteducationsothattheycouldactasguardiansofthecityandcareforthelessablehebelievedthateducationshouldbeholisticincludingfactsskillsphysicaldisciplinemusicandartplatobelievedthattalentandintelligenceisnotdistributedgeneticallyandthusisbefoundinchildrenborntoallclassesalthoughhisproposedsystemofselectivepubliceducationforaneducatedminorityofthepopulationdoesnotreallyfollowademocraticmodelaristotleconsideredhumannaturehabitandreasontobeequallyimportantforcestobecultivatedineducationtheultimateaimofwhichshouldbetoproducegoodandvirtuouscitizensheproposedthatteachersleadtheirstudentssystematicallyandthatrepetitionbeusedasakeytooltodevelopgoodhabitsunlikesocratesemphasisonquestioninghislistenerstobringouttheirownideasheemphasizedthebalancingofthetheoreticalandpracticalaspectsofsubjectstaughtamongwhichheexplicitlymentionsreadingwritingmathematicsmusicphysicaleducationliteraturehistoryandawiderangeofsciencesaswellasplaywhichhealsoconsideredimportantduringthemedievalperiodtheideaofperennialismwasfirstformulatedbystthomasaquinashisinworkdemagistroperennialismholdsthatoneshouldteachthosethingsdeemedtobeofeverlastingimportancetoallpeopleeverywherenamelyprinciplesandreasoningnotjustfactswhichareapttochangeovertimeandthatoneshouldteachfirstaboutpeoplenotmachinesortechniquesitwasoriginallyreligiousinnatureanditwasonlymuchlaterthatatheoryofsecularperennialismdevelopedduringtherenaissancethefrenchskepticmicheldemontaignewasoneofthefirsttocriticallylookateducationunusuallyforhistimemontaignewaswillingtoquestiontheconventionalwisdomoftheperiodcallingintoquestionthewholeedificeoftheeducationalsystemandtheimplicitassumptionthatuniversityeducatedphilosopherswerenecessarilywiserthanuneducatedfarmworkersforexample";
    char plainrandtext[100];
    srand(time(0));

    plainrandtext[0] = '\0'; //gpt made these lines:
    int rand_start = rand() % (strlen(randtext) - 100); // ensure at least 100 chars available
    int rand_len = 49 + rand() % 50; // copy 90 to 99 characters
    strncpy(plainrandtext, randtext + rand_start, rand_len);
    plainrandtext[rand_len] = '\0';

    char cipherrandtext[100];
    int key[100];
    int len=makekey(plainrandtext,randtext,key);
    encrypt(plainrandtext,cipherrandtext,key,len);
    int num=(int)ceil(strlen(cipherrandtext)/18.0);
    makepackets(cipherrandtext,packets_out);

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

    int numpacks=(int)ceil(strlen(ciphertext)/18.0);
    char packets[numpacks][26];
    char junk [100][26];
    for(int i=0;i<100;i++){strcpy(junk[i],"\0");}
    makepackets(ciphertext,packets);
    makejunk(junk);
    printf("%s\n",ciphertext);
    decrypt(ciphertext,plaintext,keystream,len_of_key);
    printf("%s\n\n",plaintext);
    printf("\n");
    int error=sort_and_verify_packets(packets,numpacks);
    openpackets(ciphertext,packets,numpacks);
    printf("\n %d %s\n\n",error,ciphertext);
    for(int i=0;i<sizeof(packets)/sizeof(packets[0]);i++){printf("%s ",packets[i]);}
    printf("\n");
    for(int i=0;i<100;i++){if(junk[i][0]!='\0'){printf("%s ",junk[i]);}}

    writetofile("poem","This_is_a_poem.");
    return 0;
}