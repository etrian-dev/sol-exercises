// header file for parse_opt.c
#ifndef PARSE_OPT_H
#define PARSE_OPT_H

// declarations because c99 gets bitchy
#if __STDC_VERSION__ <= 199901L
int getopt(int argc, char **argv, const char *opstring);
extern int optind;
extern int opterr;
#endif

// recognized options are -w and -l (no arguments)
#define OPSTRING "wl"
// useful macros to indicate if counting is turned on or off
#define COUNT_ON 1
#define COUNT_OFF 0

// prints the help message to stdout
void print_usage(const char *program);

// parses options passed in argv and sets count_words and count_lines accordingly
// 0 -> success (either -h or some option has been recognized)
int parse_opt(int argc, char **argv, int *count_words, int *count_lines);

#endif
