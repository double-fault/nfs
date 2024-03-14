gcc test.c mfs.c udp.c -o client
gcc server.c ufs.c udp.c -o server
gcc mkfs.c -o mkfs
