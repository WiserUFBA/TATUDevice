#include <stdio.h>
#include <stdint.h>
#include <string.h>

#define HASH_DJB(START, LEN, INPUT, OUTPUT) for(i = START; i < LEN; i++){ OUTPUT = ((OUTPUT << 5) + OUTPUT) + INPUT[i]; }
#define al 1
#define ou 0x1

int main(){
	int i;
	uint32_t str_hash = 5381;
	char string[100];
	printf("// Developed by Ramon Costa and Jeferson Lima\n// DJB2 Const Generator V 1.0\n");
	//for(i = 1; i <  algumacoisa;i++);
	while(scanf("%s",string) != EOF){
		str_hash = 5381;
		HASH_DJB(0,strlen(string),string,str_hash);
		printf("#define H_%s 0x%X\n", string, str_hash);
		printf("#define H_%s %d\n", string, str_hash);

	}
	return 0;
}