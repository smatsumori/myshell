/* created by Shoya Matsumori
 * SID: 61417276
 */
#include "myshell.h"

int main(int argc, char const* argv[])
{
	while (1) {
		printf("$");
		if (fgets(input, sizeof(input), stdin) == NULL) {
			printf("\nExiting.../n");
			exit(0);
		}
	}
	return 0;
}
