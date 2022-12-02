#define _GNU_SOURCE
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <openssl/md5.h>

void mine(char *str)
{
	char source[100];
	unsigned char out[16];
	MD5_CTX ctx;
	unsigned i = 0;
	unsigned len = strlen(str) - 1;

	memcpy(source, str, len);

	do {
		int add = sprintf(&source[len], "%06d", i);

		if (i % 10000 == 0) {
			printf("%d => %s\n", i, source);
		}
	
		MD5_Init(&ctx);
		MD5_Update(&ctx, source, len + add);
		MD5_Final(out, &ctx);

		if (!out[0] &&
		    !out[1] &&
		    out[2] < 0x10) {
			printf("%d => %s\n", i, source);
			unsigned i = 0;
			for (i = 0; i < 16; ++i) {
				unsigned int n = out[i];
				printf("%02x ", n);
			}
			printf("\n");
			break;
		}

		++i;
	} while (i < 100000000);

	printf("%d\n", i);
}

int main(int argc, char *argv[])
{
	char temp[1024];
	char *pos;
	FILE *f;

	if (argc < 2) {
		fprintf(stderr, "Usage: %s <input file>\n", argv[0]);
		return 1;
	}

	f = fopen(argv[1], "r");
	if (!f) {
		fprintf(stderr, "Failed to open file '%s' (%s)\n", argv[1], strerror(errno));
		return 1;
	}

	do {
		pos = fgets(temp, sizeof(temp) - 1, f);
		if (!pos)
			break;

		mine(pos);

	} while(1);

	fclose(f);

	return 0;
}
