#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>
#include <byteswap.h>
#include "sample.h"

#define ByteSwapNible(a) ( (a >> 4) | (a << 4) ) 
#define debug 0

int len = sizeof(pkt1)/sizeof(pkt1[0]);
int imsi_idx = 26;


uint8_t ConvertToByteArray2(uint8_t* varout, char *str, uint8_t len, uint8_t isSwapNible, uint8_t flag_F)
{
	uint16_t mIdx = 0;
	char tmp;

	while(mIdx < len) {
		tmp = str[mIdx++];
		if(tmp >= '0' && tmp <='9')
			tmp -= '0';
		else if(tmp >= 'a' && tmp <= 'f')
			tmp -= 'a' - 10;
		else if(tmp >= 'A' && tmp <= 'F')
			tmp -= 'A' - 10;
		else
			return 0;

		if(mIdx%2) {
			*varout = (uint8_t)(tmp << 4);
		} else {
			*varout |= (uint8_t)(tmp & 0x0f);
			if(isSwapNible)
				*varout = ByteSwapNible(*varout);

			//
			varout++;
			*varout = 0xFF;
		}
	}

	if(mIdx%2) {
		if(flag_F)
			*varout |= (uint8_t) 0x0F;
		*varout = ByteSwapNible(*varout);
	}
	return ((mIdx + 1)/2);
}

int main (int argc, char* argv[]) {
    int opt;
    unsigned char *no_pktc, *file_name, *prefix;
    int no_pkti;
    unsigned char imsi_str[16];
    uint8_t imsi_hex[8];
    unsigned char tmp[len];

    memcpy(tmp,pkt1,len);

    while ((opt = getopt (argc, argv, "hn:c:f:")) != -1)
        switch (opt)
        {
        case 'h':
            printf("Generate hexdump file for gsm-map packet with diffirent imsi:\n"
                    "Usage: ./sample -c <prefix> -f <output> -n <num packet>\n"
                    "  -c <prefix>\t mnc+mcc, max is 5 digit\n"
                    "  -f <output>\t name of output file\n"
                    "  -n <numpkt>\t number packet need to generate\n");
            exit(0);
        case 'n':
            no_pktc = optarg;
            break;
        case 'c':
            prefix = optarg;
            break;
        case 'f':
            file_name = optarg;
            break;
        case '?':
            if (optopt == 'c' || optopt == 'n' || optopt == 'f')
                fprintf (stderr, "Option -%c requires an argument.\n", optopt);
            else if (isprint (optopt))
                fprintf (stderr, "Unknown option `-%c'.\n", optopt);
            else
            fprintf (stderr,
                    "Unknown option character `\\x%x'.\n",
                    optopt);
            return 1;
        default:
            abort ();
    }

    no_pkti = atoi(no_pktc);

    for (int i = 0; i < strlen(prefix); i++) {
            imsi_str[i] = prefix[i];
    }

    char *remain_imsi = imsi_str + strlen(prefix);

    FILE *fd = fopen(file_name,"wb+");
    
    for (int j = 0; j < no_pkti; j ++) {

        sprintf(remain_imsi, "%010d",j);
        ConvertToByteArray2(imsi_hex,imsi_str,strlen(imsi_str),1,1);

        memcpy(tmp+imsi_idx,imsi_hex,8);

        #if debug
        printf("imsi str %s\n",imsi_str);
        for (int i= 0; i < 8; i ++) {
            printf("%02x ", imsi_hex[i]);
        }
        printf("\n");
        #endif

        for (int i = 0; i < len; i++) {
            if (i % 8 == 0) {
                fprintf(fd,"%06x ",i);
            }

            if (i % 8 == 7) {
                fprintf(fd,"%02x\n", tmp[i]);
            } else {
                fprintf(fd,"%02x ", tmp[i]);
            }       

        } 
        fprintf(fd,"\n");
    }
    fclose(fd);
    #if debug
        printf("number packet %s, file name %s, prefix %s\n", no_pktc, file_name, prefix);
    #endif
    return 0;
}