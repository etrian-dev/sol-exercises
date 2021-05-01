// header with useful information shared by the server and the clients

#ifndef UTIL_H_INCLUDED
#define UTIL_H_INCLUDED

// macro to wipe out debug prints from release executables
#if defined(DEBUG)
#define DBG(X) X
#else
#define DBG(X)
#endif

// known path to create the socket file
#define ADDR "sock"

// maximum socket path lenght
#define PATHLEN_MAX 108

#define BUFSZ 1000

#endif
