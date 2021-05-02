// utility macro & functions definitions header
#ifndef UTIL_H_DEFINED
#define UTIL_H_DEFINED

// macro to wipe out debug prints from release executables
#if defined(DEBUG)
#define DBG(X) X
#else
#define DBG(X)
#endif

// the socket shared by the client and the server (hardcoded)
#define SOCKET "sock"
// defines the buffer size
#define BUFSZ 1000
// define maximum number of connection tries done by the client before giving up
#define MAXTRIES 100

// defines the path to bc(1) on this machine
#define BC "/usr/bin/bc"

void child(int *in_pipe, int *out_pipe);

#endif
