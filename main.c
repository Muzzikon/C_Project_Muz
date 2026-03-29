#include <stdio.h>

int main(int argc, char *argv[]) {
	if (argc < 3) {
		printf("Usage: %s <file> <operation>\n", argv[0]);
		printf("Example: %s data.csv -kd_nearest\n", argv[0]);
		return 0;
	}

	printf("Input file: %s\n", argv[1]);
	printf("Operation: %s\n", argv[2]);

	return 0;
}
