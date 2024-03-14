/*
 * server.c - file server implementation
 * simple wrapper around ufs.c
 * created on mar 14 2024 by ashish ahuja
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ufs.h"
#include "udp.h"

// some ops like write have max nbytes of 4096, so 8192 seems good
#define BUFFER_SIZE (8192)

#define DEBUG

int main(int argc, char **argv) {
	assert(argc > 2);
	int portnum = strtol(argv[1], NULL, 10);

	int sd = UDP_Open(portnum); 
	assert(sd > -1);

	ufs *nfs = ufs_init(argv[2]); assert(nfs != NULL);

	/*
	 * serialization fmt: normal ints in the beginning in a null-terminated
	 * string (space-separated), remaining bytes are whatever buffer etc.
	 */
	while (1) {
		struct sockaddr_in addr;
		char *msg = malloc(sizeof(char) * BUFFER_SIZE);
#ifdef DEBUG
		printf("server::waiting...\n");
#endif
		int rc = UDP_Read(sd, &addr, msg, BUFFER_SIZE); 

#ifdef DEBUG
		printf("server:: read message [size:%d contents:(%s)]\n", rc, msg);
#endif

		if (rc <= 0) {
			free(msg);
			continue;
		}
		char *reply = malloc(sizeof(char) * BUFFER_SIZE);

		int fnum; int cur = 0, cur2 = 0;
		sscanf(msg, "%d%n", &fnum, &cur);

		//sprintf adds a null character at the end be careful
		if (fnum == 0) {
			//MFS_Lookup
			int pinum; char *name;
			sscanf(msg + cur, "%d%n", &pinum, &cur2);
			name = msg + cur + cur2 + 1;

			int ret = ufs_lookup(nfs, pinum, name);

			sprintf(reply, "%d", ret);
		} else if (fnum == 2) {
			//MFS_Write
			int inum; int offset; int nbytes; char *buf;
			sscanf(msg + cur, "%d%d%d%n", &inum, &offset, &nbytes, &cur2);
			buf = msg + cur + cur2 + 1;

			printf("inum buf offset nbytes %d %d %d\n", inum, offset, nbytes);
			int ret = ufs_write(nfs, inum, buf, offset, nbytes);
			sprintf(reply, "%d", ret);
		} else if (fnum == 3) {
			//MFS_Read
			int inum, offset, nbytes;
			sscanf(msg + cur, "%d%d%d", &inum, &offset, &nbytes); 
			char *buf = malloc(nbytes);

			int ret = ufs_read(nfs, inum, buf, offset, nbytes);

			int cw = sprintf(reply, "%d", ret);
			memcpy(reply + cw + 1, buf, nbytes);
			free(buf);
		} else if (fnum == 4) {
			//MFS_Creat
			int pinum, type; char *name;
			sscanf(msg + cur, "%d%d%n", &pinum, &type, &cur2);
			name = msg + cur + cur2 + 1;

			int ret = ufs_creat(nfs, pinum, type, name); 
			sprintf(reply, "%d", ret);
		} else if (fnum == 5) {
			//MFS_Unlink
			int pinum; char *name;
			sscanf(msg + cur, "%d%n", &pinum, &cur2);
			name = msg + cur + cur2 + 1;

			int ret = ufs_unlink(nfs, pinum, name);
			sprintf(reply, "%d", ret);
		}

#ifdef DEBUG
		printf("server::replying %s\n", reply);
#endif
		rc = UDP_Write(sd, &addr, reply, BUFFER_SIZE);
	}
	return 0;
}

