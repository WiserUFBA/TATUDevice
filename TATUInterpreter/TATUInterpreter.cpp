#include "TATUInterpreter.h"
#include <stdint.h>

bool TATUInterpreter::parse(unsigned char *string, unsigned int length){
    uint8_t i, j = 0;
    str_hash = 5381;
    cmd.OBJ.VAR = TATU_TYPE_ALIAS;

    switch(string[0]){
        case COMMAND_SET:
            cmd.OBJ.TYPE = TATU_SET;
            if(string[4] == CODE_INFO){
                cmd.OBJ.CODE = TATU_CODE_INFO;
                j = 9;

            }
            else if(string[4] == CODE_STATE){
                cmd.OBJ.CODE = TATU_CODE_STATE;
                j = 10; 
            }
            else if(string[4] == CODE_VALUE){
                cmd.OBJ.CODE = TATU_CODE_VALUE;
                j = 10; 
            }
            break;
        case COMMAND_EDIT:
            cmd.OBJ.TYPE = TATU_EDIT;

            if(string[5] == CODE_INFO){
                cmd.OBJ.CODE = TATU_CODE_INFO;
                j = 10; 

            }
            else if(string[5] == CODE_STATE){
                cmd.OBJ.CODE = TATU_CODE_STATE;
                j = 11; 
            }
            else if(string[4] == CODE_VALUE){
                cmd.OBJ.CODE = TATU_CODE_VALUE;
                j = 11; 
            }
            break;
        case COMMAND_GET:
            cmd.OBJ.TYPE = TATU_GET;
            
            if(string[4] == CODE_ALL){
                j = 8;
                cmd.OBJ.CODE = TATU_CODE_ALL;
                return true;
            }
            else if(string[4] == CODE_INFO){
                cmd.OBJ.CODE = TATU_CODE_INFO;
                j = 9; 
            }
            else if(string[4] == CODE_STATE){
                cmd.OBJ.CODE = TATU_CODE_STATE;
                j = 9; 

            }
            else if(string[4] == CODE_VALUE){
                cmd.OBJ.CODE = TATU_CODE_VALUE;
                j = 10; 
            }
            break;

    }

    if (!j)
        return false;

    if(IS_NUM(string[j])){ cmd.OBJ.VAR = TATU_TYPE_PIN; cmd.PIN = (uint8_t)atoi(&string[strlen(string)+1]);}
    else if(IS_SYS(string[j])){ j++; cmd.OBJ.VAR = TATU_TYPE_SYSTEM;}

    HASH_DJB(j, length, string, str_hash);
    VAR_COPY(j, length, string);

    return true;
}