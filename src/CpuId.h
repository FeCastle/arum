#if !defined(CpuId_h)
#define CpuId_h

#include <sys/resource.h>
#include <unistd.h>

//#define DEBUG_CPU

struct baseCpuid {
    int cpuid_enabled;     // is CPUID enabled on this machine

                           // function 0 values
    char vendor[16];       // processor vendor
    int lsfn;              // largest standard function number

                           // function 1 values
    int stepping;          // processor stepping
    int model;             // effective model number
    int family;            // effective family number
    int brand_id;          // only brand ID or brand string is supported
    char brand_string[48]; // 
    int clflush_size;      // cache line flush size (brand ID must be non-zero)
    int nb_node_id;        // northbridge node ID
    int cpu_number;
};

class CpuId {
public:
    CpuId();
    ~CpuId();

    void getBasicReport(baseCpuid * base);

private:
    int cpuid_enabled;		// is CPUID enabled on this machine

				// function 0 values
    char vendor[16];		// processor vendor
    int lsfn;			// largest standard function number

				// function 1 values
    int stepping;		// processor stepping
    int model;			// effective model number
    int family;			// effective family number
    int brand_id;		// only brand ID or brand string is supported
    char brand_string[48];
    int clflush_size;		// cache line flush size (brand ID must be non-zero)
    int nb_node_id;		// northbridge node ID
    int cpu_number;		// CPU number (within the server)

    void init();
    bool isCpuidEnabled();
    int  doCpuid0();            // get basic processor info and lsfn
    void doCpuid1();            // get standard features
    void doBrandString();       // get the brand string if brand id is 0
    void doCpuidIntelArch();    // get performance counter info for architecural
                                //   capabilities
};

#endif

