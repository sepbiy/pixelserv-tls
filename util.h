#ifndef UTIL_H
#define UTIL_H

// common configuration items

#define _GNU_SOURCE             // using a bunch of gcc-specific stuff

// system includes used by more than one source file
#include <errno.h>              // EPIPE, errno, EINTR
#include <netdb.h>              // addrinfo(), AI_PASSIVE, gai_strerror(), freeaddrinfo()
#include <netinet/tcp.h>        // SOL_TCP, TCP_NODELAY
#include <signal.h>             // sig_atomic_t
#include <stdio.h>              // printf() and variants
#include <stdlib.h>             // exit(), EXIT_FAILURE
#include <string.h>             // lots of stuff!
#include <syslog.h>             // syslog(), openlog()
#include <unistd.h>             // close(), setuid(), TEMP_FAILURE_RETRY, fork()
#include <time.h>               // struct timespec, clock_gettime(), difftime()
#include <arpa/inet.h>

// preprocessor defines
#define VERSION "v2.0.0"

#define BACKLOG SOMAXCONN       // how many pending connections queue will hold
#define DEFAULT_IP "*"          // default IP address ALL - use this in messages only
#define DEFAULT_PORT "80"       // the default port users will be connecting to
#define DEFAULT_TIMEOUT 1       // default timeout for select() calls, in seconds
#define DEFAULT_KEEPALIVE (DEFAULT_TIMEOUT * 120)
                                // default keep-alive duration for HTTP/1.1 connections, in seconds
                                // it's the time a connection will stay active
                                // until another request comes and refreshes the timer
#define DEFAULT_THREAD_MAX 1200 // maximum number of concurrent service threads
#define SECOND_PORT "443"
#define MAX_PORTS 10
#define MAX_TLS_PORTS 9         // PLEASE ENSURE MAX_TLS_PORTS < MAX_PORTS

#ifdef DROP_ROOT
# define DEFAULT_USER "nobody"  // nobody used by dnsmasq
#endif

# define DEFAULT_STATS_URL "/servstats"
# define DEFAULT_STATS_TEXT_URL "/servstats.txt"

/* taken from glibc unistd.h and fixes musl */
#ifndef TEMP_FAILURE_RETRY
#define TEMP_FAILURE_RETRY(expression) \
  (__extension__                                                              \
    ({ long int __result;                                                     \
       do __result = (long int) (expression);                                 \
       while (__result == -1L && errno == EINTR);                             \
       __result; }))
#endif

#ifdef TEST
# define TESTPRINT printf
#else
# define TESTPRINT(x,y...)
#endif

// cross-thread count variables
extern volatile sig_atomic_t count; // req
extern volatile sig_atomic_t avg; // cumulative moving average request size
extern volatile sig_atomic_t _act; // avg count (updated at time of average calculation)
extern volatile sig_atomic_t rmx; // maximum encountered request size
extern volatile sig_atomic_t _tct; // time count
extern volatile sig_atomic_t tav; // cumulative moving average time in msec
extern volatile sig_atomic_t tmx; // max time in msec
extern volatile sig_atomic_t err;
extern volatile sig_atomic_t tmo;
extern volatile sig_atomic_t cls;
extern volatile sig_atomic_t nou;
extern volatile sig_atomic_t pth;
extern volatile sig_atomic_t nfe;
extern volatile sig_atomic_t ufe;
extern volatile sig_atomic_t gif;
extern volatile sig_atomic_t bad;
extern volatile sig_atomic_t txt;
extern volatile sig_atomic_t jpg;
extern volatile sig_atomic_t png;
extern volatile sig_atomic_t swf;
extern volatile sig_atomic_t ico;
extern volatile sig_atomic_t sta; // so meta!
extern volatile sig_atomic_t stt;
extern volatile sig_atomic_t noc;
extern volatile sig_atomic_t rdr;
extern volatile sig_atomic_t pst;
extern volatile sig_atomic_t hed;
extern volatile sig_atomic_t opt;
extern volatile sig_atomic_t cly;

extern volatile sig_atomic_t slh;
extern volatile sig_atomic_t slm;
extern volatile sig_atomic_t sle;
extern volatile sig_atomic_t slc;
extern volatile sig_atomic_t slu;
extern volatile sig_atomic_t kcc;
extern volatile sig_atomic_t kmx;
extern volatile sig_atomic_t kct;
extern float kvg;
extern volatile sig_atomic_t krq;
extern volatile sig_atomic_t clt;

struct Global {
    int argc;
    char** argv;
    const time_t select_timeout;
    const time_t http_keepalive;
    const int pipefd;
    const char* const stats_url;
    const char* const stats_text_url;
    const int do_204;
    const int do_redirect;
#ifdef DEBUG
    const int warning_time;
#endif
};

#define GLOBAL(p,e) ((struct Global *)p)->e

// util.c functions

// encapsulation of clock_gettime() to perform one-time degradation of source
//  when necessary
void get_time(struct timespec *time);

// generate version string
// note that caller is expected to call free()
//  on the return value when done using it
char* get_version(int argc, char* argv[]);

// stats string generator
// NOTES:
// - The return value is heap-allocated, so the caller is expected to call
//   free() on the return value when done using it in order to avoid a memory
//   leak.
// - The purpose of sta_offset is to allow accounting for an in-progess status
//   response.
// - Similarly, stt_offset is for an in-progress status.txt response.
char* get_stats(const int sta_offset, const int stt_offset);

float ema(float curr, int new, int *cnt);

double elapsed_time_msec(const struct timespec start_time);

#if defined(__GLIBC__) && defined(BACKTRACE)
void print_trace();
#endif

#endif // UTIL_H
