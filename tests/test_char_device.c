#include <stdio.h>
#include <stdlib.h>


int main(int argc, char const *argv[])
{
	if (argc < 2) {
		return -1;
	}
	char *filename = argv[1];

	char *data = NULL;
	if (argc >= 3) {
		data = argv[2];
	}

	FILE* f = fopen(filename, "r+");
	if (!f) {
		return -1;
	}

	if (data) {
		fwrite(data, 1, strlen(data), f);
	} else {
		char buf [1024] = "";
		fread(buf, 1, sizeof(buf), f);
		printf("%s\n", buf);
	}

	fclose(f);
	return 0;
}