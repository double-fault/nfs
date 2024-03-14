/*
 * test.c - file for testing the whole system
 * created by ashish ahuja on mar 14 2024
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>
#include <string.h>

#include "mfs.h"

char *get_rand_str(int len) {
	char *ret = malloc(sizeof(char) * (len + 1));
	ret[len] = '\0';
	for (int i = 0; i < len; ++i) {
		ret[i] = 'a' + (rand() % 26);
	}
	return ret;
}

int main(void) {
	puts("----------->WARNING: RUN ON EMPTY DISK<------------");
	srand(time(NULL));

	char *hostname = "localhost"; int portnum = 6969;
	MFS_Init(hostname, portnum);

	//Run tests on empty disk image
	assert(MFS_Lookup(0, "..") == 0);
	assert(MFS_Lookup(0, ".") == 0);

	/*
	 * Make file /dir/dir2/file
	 * Write a 10,000 byte random string to this file, by writing 5 times (2000 bytes each).
	 * Then do a few random reads of 4000 bytes and compare.
	 */
	assert(MFS_Creat(0, MFS_DIRECTORY, "dir") == 0);
	assert(MFS_Creat(1, MFS_DIRECTORY, "dir2") == 0);
	assert(MFS_Creat(2, MFS_REGULAR_FILE, "file") == 0);

	assert(MFS_Lookup(0, "dir") == 1);
	assert(MFS_Lookup(1, "dir2") == 2);
	assert(MFS_Lookup(2, "file") == 3);

	char *str = get_rand_str(10000); 
	for (int i = 0; i < 5; ++i) {
		assert(MFS_Write(3, str + (2000 * i), 2000 * i, 2000) == 0); 
	}

	for (int i = 0; i <= 5; ++i) {
		char *buf = malloc(4010);
		assert(MFS_Read(3, buf, 1000 * i, 4000) == 0);  
		assert(!memcmp(buf, str + 1000 * i, 4000));
	}

	/*
	 * Change some bytes from 3000-6000.
	 */

	char *str2 = get_rand_str(3000);
	memcpy(str + 3000, str2, 3000);
	assert(MFS_Write(3, str2, 3000, 3000) == 0);

	for (int i = 0; i <= 5; ++i) {
		char *buf = malloc(4010);
		assert(MFS_Read(3, buf, 1000 * i, 4000) == 0);  
		assert(!memcmp(buf, str + 1000 * i, 4000));
	}
	free(str);
	free(str2);

	assert(MFS_Unlink(1, "dir2") == -1);
	assert(MFS_Unlink(2, "file") == 0);
	assert(MFS_Lookup(2, "file") == -1);
	assert(MFS_Unlink(1, "dir2") == 0);
	assert(MFS_Lookup(1, "dir2") == -1);

	return 0;
}


