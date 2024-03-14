/*
 * mfs.c - client side filesystem implementation
 * just simple network wrappers with made-up serialization
 * created on mar 14 2024 by ashish ahuja
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "udp.h"
#include "mfs.h"

#define DEBUG

#define TIMEOUT 5 

struct sockaddr_in addrSnd, addrRcv;
int mfs_sd;

int MFS_Init(char *hostname, int port) {
	mfs_sd = UDP_Open(6996);
	int rc = UDP_FillSockAddr(&addrSnd, hostname, port);
}

char *proc_call(char *msg) {
	struct timeval tv;

	fd_set fdset;
	FD_ZERO(&fdset);
	FD_SET(mfs_sd, &fdset);

	do {
		tv.tv_sec = TIMEOUT;
		tv.tv_usec = 0;
#ifdef DEBUG
		printf("client::sending request %s\n", msg);
#endif
		int rc = UDP_Write(mfs_sd, &addrSnd, msg, BUFFER_SIZE);
		if (rc < 0) {
			fprintf(stderr,"client::send fail\n");
			exit(1);
		}

#ifdef DEBUG
		printf("client::waiting for reply\n");
#endif
	} while (select(mfs_sd + 1, &fdset, NULL, NULL, &tv) != 1); 

	char *reply = malloc(BUFFER_SIZE);
	int rc = UDP_Read(mfs_sd, &addrRcv, reply, BUFFER_SIZE); 

#ifdef DEBUG
	printf("client::got reply [size:%d contents:(%s)\n", rc, reply);
#endif
	return reply;
}

int MFS_Lookup(int pinum, char *name) {
	char *msg = malloc(BUFFER_SIZE);
	int bw = sprintf(msg, "0 %d", pinum);
	memcpy(msg + bw + 1, name, strlen(name) + 1);

	char *reply = proc_call(msg);

	int ret;
	sscanf(reply, "%d", &ret);

	free(msg); free(reply);
	return ret;
}

int MFS_Write(int inum, char* buffer, int offset, int nbytes) {
	char *msg = malloc(BUFFER_SIZE);
	int bw = sprintf(msg, "2 %d %d %d", inum, offset, nbytes);
	memcpy(msg + bw + 1, buffer, nbytes);

	char *reply = proc_call(msg);

	int ret;
	sscanf(reply, "%d", &ret);

	free(msg); free(reply);
	return ret;
}

int MFS_Read(int inum, char *buffer, int offset, int nbytes) {
	char *msg = malloc(BUFFER_SIZE);
	int bw = sprintf(msg, "3 %d %d %d", inum, offset, nbytes);

	char *reply = proc_call(msg);

	int cur = 0, ret = 0;
	sscanf(reply, "%d%n", &ret, &cur);
	memcpy(buffer, reply + cur + 1, nbytes);
	free(msg); free(reply);
	return ret;
}

int MFS_Creat(int pinum, int type, char* name) {
	char *msg = malloc(BUFFER_SIZE);
	int bw = sprintf(msg, "4 %d %d", pinum, type);
	memcpy(msg + bw + 1, name, strlen(name) + 1);

	char *reply = proc_call(msg);

	int ret;
	sscanf(reply, "%d", &ret);

	free(msg); free(reply);
	return ret;
}

int MFS_Unlink(int pinum, char *name) {
	char *msg = malloc(BUFFER_SIZE);
	int bw = sprintf(msg, "5 %d", pinum);
	memcpy(msg + bw + 1, name, strlen(name) + 1);

	char *reply = proc_call(msg);

	int ret;
	sscanf(reply, "%d", &ret);

	free(msg); free(reply);
	return ret;
}

/*
int main(void) {
	char *hostname = "localhost"; int portnum = 6969;
	MFS_Init(hostname, portnum);

	printf("mfs_lookup 0 .. : %d\n", MFS_Lookup(0, ".."));
	printf("ufs_lookup 0 . : %d\n", MFS_Lookup(0, "."));
	printf("ufs_creat 0 1 abcd : %d\n", MFS_Creat(0, MFS_REGULAR_FILE, "abcd"));
	printf("ufs_lookup 0 abcd : %d\n", MFS_Lookup(0, "abcd"));
	printf("ufs_creat 0 0 real_dir : %d\n", MFS_Creat(0, MFS_DIRECTORY, "real_dir"));
	//printf("ufs_unlink 0 real_dir : %d\n", ufs_unlink(nfs, 0, "real_dir"));
	printf("ufs_lookup 0 real_dir : %d\n", MFS_Lookup(0, "real_dir"));
	printf("ufs_lookup 2 . : %d\n", MFS_Lookup(2, "."));
	printf("ufs_write 1 'hello world\\0' 0 12 : %d\n", MFS_Write(1, "hello world\0", 0, 12));
	printf("ufs_write 1 'bye' 7 3 : %d\n", MFS_Write(1, "bye", 7, 3));

	char *buf = malloc(100);
	printf("ufs_read 1 buf 0 12 : %d\n", MFS_Read(1, buf, 0, 12));
	printf("buf %s\n", buf);

	return 0;
}*/


