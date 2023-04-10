
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdbool.h>

#define PASSWORD_LENGTH 128
#define MAX_USERS_NUMBER 8


int state;

typedef struct user {
    char nick[128];
    char pass[PASSWORD_LENGTH];
} User;

typedef struct record {
    char name[64];
    char email[64];
    char nick[128];
    char pass[PASSWORD_LENGTH];
} Record;

typedef struct record_arr {
    User user;
    Record array[32];
    int last;
} UserRecordArray;


typedef struct user_arr {
    User array[MAX_USERS_NUMBER];
    int last;
} UserArray;


void encrypt(char original[], char encrypted[]){
    // if character is a letter (case-sensitive) Caesar Cypher, if special character it is left
    int length = strlen(original);
    for (int i=0; i < length; i++){
        // in ascii big letters are in range [65,90]
        if ( original[i] >= 65 && original[i] <= 90){
            encrypted[i] = (original[i] - 'A' + 3) % 26 + 'A';
        }
        // in ascii small letters are in range [97,122]
        else if (original[i] >= 97 && original[i] <= 122){
            encrypted[i] = (original[i] - 'a' + 3) % 26 + 'a';
        }
        else {
            encrypted[i] = original[i];
        }
    }
    encrypted[length] = '\0';
}

void decrypt(char original[], char decrypted[]){
    // if character is a letter (case-sensitive) Caesar Cypher, if special character it is left
    int length = strlen(original);
    for (int i=0; i < length; i++){
        // in ascii big letters are in range [65,90]
        if ( original[i] >= 65 && original[i] <= 90){
            decrypted[i] = (original[i] - 'A' - 3) % 26 + 'A';
        }
        // in ascii small letters are in range [97,122]
        else if (original[i] >= 97 && original[i] <= 122){
            decrypted[i] = (original[i] - 'a' - 3) % 26 + 'a';
        }
        else {
            decrypted[i] = original[i];
        }
    }
    decrypted[length] = '\0';
}

bool userAlreadyRegistered(FILE *fp, char searched_nick[]){
    char buffer[300];
    while (fgets(buffer, 300, fp) != 0){
        char nick[128];
        char pass[128];
        sscanf(buffer, "%s %s", nick, pass);
        printf("nick: %s\n", nick);
        if (strcmp(searched_nick, nick) == 0){
            return true;
        }
    }
    return false;
}

bool userAuthentication(FILE *fp, char given_nick[], char given_password[]){
    char buffer[300];
    while (fgets(buffer, 300, fp) != 0){
        char nick[128];
        char pass[128];
        sscanf(buffer, "%s %s\n", nick, pass);
        if (strcmp(given_nick, nick) == 0 && strcmp(given_password, pass) == 0){
            return true;
        }
    }
    return false;
}

bool checkIfExists(char filename[]){
    FILE * fp = fopen(filename, "r");
    if ( fp == NULL)
        return false;
    fclose(fp);
    return true;
}

void registerUser(char nick[], char pass[]){
    char encrypted[PASSWORD_LENGTH];
    encrypt(pass,encrypted);
    FILE *fp;
    int exists = checkIfExists("users.txt");
    if ( exists == false){
        fp = fopen("users.txt", "w");
        fprintf(fp, "%s %s\n", nick, encrypted);
    }
    else {
        fp = fopen("users.txt", "r+");
        if (userAlreadyRegistered(fp, nick) == true){
            state = -1;
            fclose(fp);
            return;
        }
        fprintf(fp, "%s %s\n", nick, encrypted);
    }
    fclose(fp);
}


bool login(char nick[], char pass[]){
    char encrypted[PASSWORD_LENGTH];
    encrypt(pass,encrypted);
    FILE *fp = fopen("users.txt", "r");
    if (userAuthentication(fp, nick, encrypted) == true){
        fclose(fp);
        return true;
    }
    fclose(fp);
    return false;
}

void initializeUserRecordArray(UserRecordArray *array, User user){
    array->user = user;
    array->last = 0;
}

void createRecord(Record *record, char name[], char email[], char nick[], char pass[]){
    strncpy(record->name, name, 64);
    strncpy(record->email, email, 64);
    strncpy(record->nick, nick, 128);
    strncpy(record->pass, pass, 128);
}

int findRecordInArray(UserRecordArray *array, char name[]){
    for (int i=0; i < array->last; i++){
        if (strcmp(array->array[i].name, name) == 0){
            return i;
        }
    }
    return -1;
}

void addRecordToArray(UserRecordArray *array, Record record){
    int index = findRecordInArray(array, record.name);
    if ( index != -1 ) return;
    if (32 == array->last) { 
        fprintf(stderr, "Maximum amount of records achieved!\n");
        return;
    }
    array->array[array->last++] = record;
}

void deleteRecordFromArray(UserRecordArray *array, char name[]){
    int index = findRecordInArray(array, name);
    if ( index == -1 ) return;
    for (int i=index; i < array->last; i++)
        array->array[i] = array->array[i+1];
    array->last--;
}

void showRecordsNames(char **array, char user_name[]){
    FILE *fp = fopen("users.txt", "r");
    char buffer[300];
    int i=0;
    while (fgets(buffer, 300, fp) != 0){
        char nick[128];
        char pass[128];
        sscanf(buffer, "%s %s\n", nick, pass);
        strncpy(array[i++], nick, 128);
    }
    fclose(fp);
}

void showRecords(UserRecordArray *array, char user_name[]){
    FILE *fp = fopen("passwords.txt", "r");
    char buffer[1024];
    while (fgets(buffer, 1024, fp) != 0){
        char name[64];
        char email[64];
        char nick[128];
        char pass[128];
        sscanf(buffer, "%s %s %s %s\n", name, email, nick, pass);
        if (strcmp(user_name, nick) == 0){
            Record record;
            char decrypted[PASSWORD_LENGTH];
            decrypt(pass, decrypted);
            createRecord(&record, name, email, nick, decrypted);
            addRecordToArray(array, record);
        }
    }
    fclose(fp);
}

void showSingleRecord(Record *record, char user_name[], char record_name[]){
    FILE *fp = fopen("passwords.txt", "r");
    char buffer[1024];
    while (fgets(buffer, 1024, fp) != 0){
        char name[64];
        char email[64];
        char nick[128];
        char pass[128];
        sscanf(buffer, "%s %s %s %s", name, email, nick, pass);
        if (strcmp(user_name, nick) == 0 && strcmp(record_name, name) == 0){
            createRecord(record, name, email, nick, pass);
        }
    }
    fclose(fp);
}

void exportPasswords(UserRecordArray array){
    FILE *fp = fopen("passwords.txt", "w");
    for (int i=0; i < array.last; i++){
        Record record = array.array[i];
        char encrypted[128];
        encrypt(record.pass, encrypted);
        fprintf(fp, "%s %s %s %s\n", record.name, record.email, record.nick, encrypted);
    }
    fclose(fp);
}

