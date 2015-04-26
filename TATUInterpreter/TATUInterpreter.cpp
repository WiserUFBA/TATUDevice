#include "TATUInterpreter.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

uint8_t atoi_T(char *p){
    int k = 0;
    while(*p){k = (k << 3) + (k << 1) + (*p) - '0'; p++; }
    return k;
}

bool TATUInterpreter::parse(char *string, unsigned int length){
    uint8_t i, j = 0;
    str_hash = 5381;
    cmd.STRUCTURE = 0;
    cmd.OBJ.VAR = TATU_TYPE_ALIAS;

    switch(string[0]){
        case COMMAND_SET:
            cmd.OBJ.TYPE = TATU_SET;
            j = 9;
            switch(string[4]){
                case CODE_INFO:
                    cmd.OBJ.CODE = TATU_CODE_INFO;
                    break;
                case CODE_STATE:
                    cmd.OBJ.CODE = TATU_CODE_STATE;
                    j++;
                    break;
                case CODE_VALUE:
                    cmd.OBJ.CODE = TATU_CODE_VALUE;
                    j++;
                    break;
            }
            break;
        case COMMAND_EDIT:
            cmd.OBJ.TYPE = TATU_EDIT;
            j = 10;
            switch(string[5]){
                case CODE_INFO:
                    cmd.OBJ.CODE = TATU_CODE_INFO;
                    break;
                case CODE_STATE:
                    cmd.OBJ.CODE = TATU_CODE_STATE;
                    j++; 
                    break;
                case CODE_VALUE:
                    cmd.OBJ.CODE = TATU_CODE_VALUE;
                    j++; 
            }
            break;
        case COMMAND_GET:
            cmd.OBJ.TYPE = TATU_GET;
            j = 9;
            
            switch(string[4]){
                case CODE_ALL:
                    cmd.OBJ.CODE = TATU_CODE_ALL;
                    return true;
                case CODE_INFO:
                    cmd.OBJ.CODE = TATU_CODE_INFO;
                    break; 
                case CODE_STATE:
                    cmd.OBJ.CODE = TATU_CODE_STATE;
                    break; 
                case CODE_VALUE:
                    cmd.OBJ.CODE = TATU_CODE_VALUE;
                    j++;
                   break; 
            }
            break;
        case COMMAND_POST:
            for(j = 6; string[j] != ':'; j++);
            strcpy(string, &string[++j]);
            return true;
    }

    if (!j) return false;

    if(IS_NUM(string[j])){ cmd.OBJ.VAR = TATU_TYPE_PIN; cmd.OBJ.PIN = atoi_T(&string[strlen(string)+1]);}
    else if(IS_SYS(string[j])){ j++; cmd.OBJ.VAR = TATU_TYPE_SYSTEM;}

    HASH_DJB(j, length, string, str_hash);
    VAR_COPY(j, length, string);

    return true;
}
