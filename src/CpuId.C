#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include "CpuId.h"


CpuId::CpuId()
{
   int before=0, after=0;

   this->init();          //initialize class members

   if (this->isCpuidEnabled()) { //check if support for cpuid
      if (this->doCpuid0() >= 1) {
         //then retrieve standard feature information
         this->doCpuid1();

         if (strcmp("GenuineIntel", this->vendor) == 0) {
            if (this->brand_id == 0) {
               this->doBrandString();
            }
            if (this->lsfn == 0xA) {
               this->doCpuidIntelArch();
            }
         }
      }
   } else {
      //This processor does not support CPUID.
      printf("CPUID is not enabled.\n");
   }
}

void
CpuId::getBasicReport(baseCpuid * base)
{
   base->cpuid_enabled = this->cpuid_enabled;
   strcpy(base->vendor, this->vendor);
   base->lsfn = this->lsfn;
   base->stepping = this->stepping;
   base->model = this->model;
   base->family = this->family;
   base->brand_id = this->brand_id;
   strcpy(base->brand_string, this->brand_string);
   base->clflush_size = this->clflush_size;
   base->nb_node_id = this->nb_node_id;
   base->cpu_number = this->cpu_number;
}

void
CpuId::init()
{
   this->cpuid_enabled = 0;
   memset (this->vendor, '\0', 16);
   this->lsfn = 0;
   this->stepping=0;
   this->model=0;
   this->family=0;
   this->brand_id=0;
   memset (this->brand_string, '\0', 48);
   this->clflush_size=0;
   this->nb_node_id = 0;
   this->cpu_number = 0;
}

bool
CpuId::isCpuidEnabled()
{
    /*
     * This is the code to test if processor supports CPUID.
     * If the ID flag (bit 21) in the EFLAGS register can be
     * set and cleared by software, then the processor supports
     * CPUID.
     * NOTE: the pop and push instructions required the 64 bit
     *       register operands for this to work properly on
     *       SLES 10.1 with Xeon 5100 and AMD Opeteron processors.
     *       Earlier versions of SLES had allowed the 32 bit registers.
     *       We think this is probably related to a change in the gcc
     *       compiler.
     */

    int before=0, after=0;

    __asm__ __volatile__(
        "pushf ;"
        "pop %%rax;"
        "movl %%eax,%0;"
        "movl $0x00200000,%%ebx;" 
        "xor %%ebx,%%eax ;"
        "push %%rax;"
        "popf ;"
        "pushf ;"
        "pop %%rax;"
        "movl %%eax,%1;"
        ""
    : "=r"(before), "=r"(after)
    :
    : "%eax", "%ebx"
    );

    this->cpuid_enabled = (before != after);

    return (this->cpuid_enabled);
}

int
CpuId::doCpuid0()
{
    /*
     * Run the CPUID instruction; store results in local and class variables.
     * By moving 0 into EAX, this tells CPUID to report basic CPUID
     * information and vendor information. In EAX, the processor returns
     * the highest value that CPUID recognizes for returning basic
     * processor information.  The vendor string is returned in EBX, EDX, and
     * ECX (in that order).
     */
    int eax=0, ebx=0, ecx=0, edx=0;

     __asm__ __volatile__(
        "movl $0x0,%%eax;"
        "cpuid ;"
        "movl %%eax,%0;"
        "movl %%ebx,%1;"
        "movl %%ecx,%2;"
        "movl %%edx,%3;"
        ""
    : "=r"(eax), "=r"(ebx), "=r"(ecx), "=r"(edx)
    :
    : "%eax", "%ebx", "%ecx", "%edx"
    );

    this->lsfn = eax;  /*highest value recognized by CPUID for this proc.*/

    this->vendor[ 0] = (ebx>> 0) & 0xff;
    this->vendor[ 1] = (ebx>> 8) & 0xff;
    this->vendor[ 2] = (ebx>>16) & 0xff;
    this->vendor[ 3] = (ebx>>24) & 0xff;
    this->vendor[ 4] = (edx>> 0) & 0xff;
    this->vendor[ 5] = (edx>> 8) & 0xff;
    this->vendor[ 6] = (edx>>16) & 0xff;
    this->vendor[ 7] = (edx>>24) & 0xff;
    this->vendor[ 8] = (ecx>> 0) & 0xff;
    this->vendor[ 9] = (ecx>> 8) & 0xff;
    this->vendor[10] = (ecx>>16) & 0xff;
    this->vendor[11] = (ecx>>24) & 0xff;
    this->vendor[12] = '\0';

#ifdef DEBUG_CPU    
    printf ("CPUID-0:\n");
    printf ("eax = 0x%8.8lx\n",eax);
    printf("lsfn   = %d\n", this->lsfn);
    printf("vendor = %s\n", this->vendor);
#endif
    return this->lsfn;
}

void
CpuId::doCpuid1()
{
    /*
     * Running CPUID with 1 in EAX yields Model, Family,
     * and Stepping information in EAX; Brand, CLFLUSH size,
     * max nr of logical processors in this physical package, and
     * initial APIC ID in EBX; extended feature information in ECX;
     * and Feature information in EDX
    */
    int eax=0, ebx=0, ecx=0, edx=0;

    __asm__ __volatile__(
        "movl $0x1,%%eax;"
        "cpuid ;"
        "movl %%eax,%0;"
        "movl %%ebx,%1;"
        "movl %%ecx,%2;"
        "movl %%edx,%3;"
        ""
    : "=r"(eax), "=r"(ebx), "=r"(ecx), "=r"(edx)
    :
    : "%eax", "%ebx", "%ecx", "%edx"
    );

    union {
       struct {
          unsigned stepping:4;
          unsigned model:4;
          unsigned family:4;
          unsigned res0:4;
          unsigned extended_model:4;
          unsigned extended_family:8;
          unsigned res1:4;
       } fields;
       unsigned eax;
    } a;

    a.eax = eax;

    this->stepping = a.fields.stepping;

    if (this->family < 0xF) {
        this->model  = a.fields.model;
        this->family = a.fields.family;
    } else {
        this->model  = a.fields.model + (a.fields.extended_model << 4);
        this->family = a.fields.family + a.fields.extended_family;
    }
#ifdef DEBUG_CPU
    printf("CPUID-1:\n");
    printf("a.fields.stepping        = 0x%lx\n", a.fields.stepping);
    printf("a.fields.model           = 0x%lx\n", a.fields.model);
    printf("a.fields.family          = 0x%lx\n", a.fields.family);
    printf("a.fields.res0            = 0x%lx\n", a.fields.res0);
    printf("a.fields.extended_model  = 0x%lx\n",
            a.fields.extended_model);
    printf("a.fields.extended_family = 0x%lx\n",
            a.fields.extended_family);
    printf("a.fields.res1            = 0x%lx\n", a.fields.res1);
    printf("\n");

    printf("this->stepping           = 0x%lx\n", this->stepping);
    printf("this->model              = 0x%lx\n", this->model);
    printf("this->family             = 0x%lx\n", this->family);
    printf("\n");

    printf("eax = 0x%8.8lx\n", eax);
#endif
    union {
        struct {
            unsigned brand_id:8;
            unsigned clflush_size:8;
            unsigned reserved:8;
            unsigned northbridge_node_id:3;
            unsigned cpu_number:5;
        } fields;
        unsigned ebx;
    } b;
    b.ebx = ebx;

    this->brand_id     = b.fields.brand_id;
    this->clflush_size = b.fields.clflush_size;
    this->nb_node_id   = b.fields.northbridge_node_id;
    this->cpu_number   = b.fields.cpu_number;

#ifdef DEBUG_CPU
    printf("ebx = 0x%8.8lx\n", ebx);
    printf("ecx = 0x%8.8lx\n", ecx);
#endif

    union {
        struct {
            unsigned on_chip_x87:1;
            unsigned vm_extensions:1;
            unsigned debug_extensions:1;
            unsigned page_size_extensions_1:1;
            unsigned time_stamp_counter:1;
            unsigned k86_msrs:1;
            unsigned physical_address_extensions:1;
            unsigned machine_check:1;
            unsigned cmpxchg8b:1;
            unsigned apic:1;
            unsigned reserved_10:1;
            unsigned sysenter:1;
            unsigned mtrrs:1;
            unsigned page_global_extensions:1;
            unsigned machine_check_arch:1;
            unsigned conditional_move:1;
            unsigned page_attribute_table:1;
            unsigned page_size_extensions_2:1;
            unsigned reserved_18:1;
            unsigned clflush:1;
            unsigned reserved_20:3;
            unsigned mmx:1;
            unsigned fxsave:1;
            unsigned sse:1;
            unsigned sse2:1;
            unsigned reserved_27:5;
        } fields;
        unsigned edx;
    } d;
    d.edx = edx;

    /*
     * EDX reports on the standard features supported by this
     * processor. There are differences between AMD x86_64 and
     * Intel 64 and IA32 archictectures:
     *    Bit      AMD              Intel
     *     0  x87 FPU             x87 FPU
     *     1  virtual mode ext.   virtual mode ext.
     *     2  debugging ext.      debugging ext.
     *     3  page size ext.      page size ext.
     *     4  tsc                 tsc
     *     5  AMD K86 MSRs        MSRs
     *     6  Phy. Addr. Ext.     Phy. Addr. Ext.
     *     7  Mach. Check Excep.  Mach. Check Excep.
     *     8  CMPXCHG8B           CMPXCHG8B
     *     9  APIC                APIC
     *    10  Reserved            Reserved
     *    11  SYSENTER/SYSEXIT    SYSENTER/SYSEXIT
     *    12  MTRRs               MTRRs
     *    13  Page Global Ext.    Page Global Ext.
     *    14  Mach. Check Arch.   Mach. Check Arch.
     *    15  Cond. Move          Cond. Move
     *    16  Page Attr. Table    Page Attr. Table
     *    17  Page Sz. Ext.       Page Sz. Ext.
     *    18  Reserved            Process Serial Number
     *    19  CLFLUSH             CLFLUSH
     *    20  Reserved            Reserved
     *    21  Reserved            Debug Store
     *    22  Reserved            Therm. Mon. & SW controlled clock
     *    23  MMX                 MMX
     *    24  FXSAVE and FXRSTOR  FXSAVE and FXRSTOR
     *    25  SSE                 SSE
     *    26  SSE2                SSE2
     *    27  Reserved            Self Snoop
     *    28  Reserved            Multi-Threading (HTT)
     *    29  Reserved            Thermal Monitor
     *    30  Reserved            Reserved
     *    31  Reserved            Pending Break Enable
     * reserved 10,20-22,27-31        10,20,30
     *
     */
#ifdef DEBUG_CPU
    printf("edx = 0x%8.8lx\n", edx);
#endif
}

void
CpuId::doBrandString()
{
    /*
     * Intel processors that do not support brand index (family 15, model 3
     * and greater) should use this to obtain the brand string and maximum
     * qualified frequency of the processor.  This method runs CPUID three
     * times, with input values in EAX of 80000002h, 80000003h, and 80000004h.
     * 16 ASCII characters are returned across EAX, EBX, ECX, and EDX for each
     * input value.  The final set of chars returned in EDX for EAX input
     * 80000004h input is null terminated.
     */
    int eax=0, ebx=0, ecx=0, edx=0;

     __asm__ __volatile__(
        "movl $0x80000002,%%eax;"
        "cpuid ;"
        "movl %%eax,%0;"
        "movl %%ebx,%1;"
        "movl %%ecx,%2;"
        "movl %%edx,%3;"
        ""
    : "=r"(eax), "=r"(ebx), "=r"(ecx), "=r"(edx)
    :
    : "%eax", "%ebx", "%ecx", "%edx"
    );

    this->brand_string[ 0] = (eax>> 0) & 0xff;
    this->brand_string[ 1] = (eax>> 8) & 0xff;
    this->brand_string[ 2] = (eax>>16) & 0xff;
    this->brand_string[ 3] = (eax>>24) & 0xff;
    this->brand_string[ 4] = (ebx>> 0) & 0xff;
    this->brand_string[ 5] = (ebx>> 8) & 0xff;
    this->brand_string[ 6] = (ebx>>16) & 0xff;
    this->brand_string[ 7] = (ebx>>24) & 0xff;
    this->brand_string[ 8] = (ecx>> 0) & 0xff;
    this->brand_string[ 9] = (ecx>> 8) & 0xff;
    this->brand_string[10] = (ecx>>16) & 0xff;
    this->brand_string[11] = (ecx>>24) & 0xff;
    this->brand_string[12] = (edx>> 0) & 0xff;
    this->brand_string[13] = (edx>> 8) & 0xff;
    this->brand_string[14] = (edx>>16) & 0xff;
    this->brand_string[15] = (edx>>24) & 0xff;

    eax = 0;
    ebx = 0;
    ecx = 0;
    edx = 0;

    __asm__ __volatile__(
        "movl $0x80000003,%%eax;"
        "cpuid ;"
        "movl %%eax,%0;"
        "movl %%ebx,%1;"
        "movl %%ecx,%2;"
        "movl %%edx,%3;"
        ""
    : "=r"(eax), "=r"(ebx), "=r"(ecx), "=r"(edx)
    :
    : "%eax", "%ebx", "%ecx", "%edx"
    );

    this->brand_string[16] = (eax>> 0) & 0xff;
    this->brand_string[17] = (eax>> 8) & 0xff;
    this->brand_string[18] = (eax>>16) & 0xff;
    this->brand_string[19] = (eax>>24) & 0xff;
    this->brand_string[20] = (ebx>> 0) & 0xff;
    this->brand_string[21] = (ebx>> 8) & 0xff;
    this->brand_string[22] = (ebx>>16) & 0xff;
    this->brand_string[23] = (ebx>>24) & 0xff;
    this->brand_string[24] = (ecx>> 0) & 0xff;
    this->brand_string[25] = (ecx>> 8) & 0xff;
    this->brand_string[26] = (ecx>>16) & 0xff;
    this->brand_string[27] = (ecx>>24) & 0xff;
    this->brand_string[28] = (edx>> 0) & 0xff;
    this->brand_string[29] = (edx>> 8) & 0xff;
    this->brand_string[30] = (edx>>16) & 0xff;
    this->brand_string[31] = (edx>>24) & 0xff;

    eax = 0;
    ebx = 0;
    ecx = 0;
    edx = 0;

    __asm__ __volatile__(
        "movl $0x80000004,%%eax;"
        "cpuid ;"
        "movl %%eax,%0;"
        "movl %%ebx,%1;"
        "movl %%ecx,%2;"
        "movl %%edx,%3;"
        ""
    : "=r"(eax), "=r"(ebx), "=r"(ecx), "=r"(edx)
    :
    : "%eax", "%ebx", "%ecx", "%edx"
    );

    this->brand_string[32] = (eax>> 0) & 0xff;
    this->brand_string[33] = (eax>> 8) & 0xff;
    this->brand_string[34] = (eax>>16) & 0xff;
    this->brand_string[35] = (eax>>24) & 0xff;
    this->brand_string[36] = (ebx>> 0) & 0xff;
    this->brand_string[37] = (ebx>> 8) & 0xff;
    this->brand_string[38] = (ebx>>16) & 0xff;
    this->brand_string[39] = (ebx>>24) & 0xff;
    this->brand_string[40] = (ecx>> 0) & 0xff;
    this->brand_string[41] = (ecx>> 8) & 0xff;
    this->brand_string[42] = (ecx>>16) & 0xff;
    this->brand_string[43] = (ecx>>24) & 0xff;
    this->brand_string[44] = (edx>> 0) & 0xff;
    this->brand_string[45] = (edx>> 8) & 0xff;
    this->brand_string[46] = (edx>>16) & 0xff;
    this->brand_string[47] = (edx>>24) & 0xff;

#ifdef DEBUG_CPU
    printf ("CPUID-BRAND_STRING:%s\n", this->brand_string);
#endif

}
void
CpuId::doCpuidIntelArch()
{
    /*
     * If the processor is a "newer" Intel processor, it
     * supports a set of fixed-function performance counter
     * registers and facilities for programming them. Intel
     * refers to these capabilities as "Architectural", meaning
     * that the visible behavior of each counter (of the fixed set)
     * is consistent across a family of processor implementations. An Intel
     * processor supporting this returns 0Ah in EAX when seeded with 0
     * when CPUID is executed.
     */

     int eax=0, ebx=0, ecx=0, edx=0;
     printf ("Intel processor supports Architectural counters\n");

     __asm__ __volatile__(
         "movl $0xA,%%eax;"
         "cpuid ;"
         "movl %%eax,%0;"
         "movl %%ebx,%1;"
         "movl %%ecx,%2;"
         "movl %%edx,%3;"
         ""
     : "=r"(eax), "=r"(ebx), "=r"(ecx), "=r"(edx)
     :
     : "%eax", "%ebx", "%ecx", "%edx"
     );
#ifdef DEBUG_CPU
     printf ("EAX = 0x%8.8lx\n", eax);
     printf ("EBX = 0x%8.8lx\n", eax);
     printf ("ECX = 0x%8.8lx\n", eax);
     printf ("EDX = 0x%8.8lx\n", eax);
#endif
}

CpuId::~CpuId()
{
}

