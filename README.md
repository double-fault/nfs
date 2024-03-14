# NFS

A quick and dirty implementation of ostep's [filesystem-distributed-ufs](https://github.com/remzi-arpacidusseau/ostep-projects/tree/master/filesystems-distributed-ufs) project.

Use build.sh to build. Create an empty disk image using `./mkfs -f <disk name>`. Then, start the server up using `./server <port no.> <disk name>`. `mfs.c` has some tests to make sure everything is working fine; run `./client` and if everything is ok then none of the asserts will fail.

`ufs.c` has the file system implementation, `server.c` puts a wrapper around `ufs.c` and `mfs.c` has the client-side stuff.  

