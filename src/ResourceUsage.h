#if !defined(ResourceUsage_h)
#define ResourceUsage_h

#include <sys/time.h>
#include <sys/resource.h>
#include <unistd.h>


class ResourceUsage {
public:
    static const int SELF=0;
    static const int CHILDREN=1;

    ResourceUsage(int who=SELF);
    ~ResourceUsage();

    double user_time()    { return ru.ru_utime.tv_sec + (double) ru.ru_utime.tv_usec * 1E-6; };
    double sys_time()     { return ru.ru_stime.tv_sec + (double) ru.ru_stime.tv_usec * 1E-6; };
    long   maxrss()       { return ru.ru_maxrss; };	/* maximum resident set size */
    long   ixrss()        { return ru.ru_ixrss; };	/* integral shared memory size */
    long   idrss()        { return ru.ru_idrss; };	/* integral unshared data size */
    long   isrss()        { return ru.ru_isrss; };	/* integral unshared stack size */
    long   minor_fault()  { return ru.ru_minflt; };	/* page reclaims */
    long   major_fault()  { return ru.ru_majflt; };	/* page faults */
    long   swaps()        { return ru.ru_nswap; };	/* swaps */
    long   in_block()     { return ru.ru_inblock; };	/* block input operations */
    long   out_block()    { return ru.ru_oublock; };	/* block output operations */
    long   msg_send()     { return ru.ru_msgsnd; };	/* messages sent */
    long   msg_recv()     { return ru.ru_msgrcv; };	/* messages received */
    long   signals()      { return ru.ru_nsignals; };	/* signals received */
    long   vol_ctx_sw()   { return ru.ru_nvcsw; };	/* voluntary context switches */
    long   invol_ctx_sw() { return ru.ru_nivcsw; };	/* involuntary context switches */

private:
    struct rusage ru;
};

#endif

