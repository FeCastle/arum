#if !defined( Arch_h )
#define Arch_h

#define MAX_LINE_LEN 100

#define MAX_REG_SETS            6
#define NONSUPPORT_VENDOR      -1
#define INTEL_VENDOR            1
#define AMD_VENDOR              2
#define INTEL_VENDOR_STR	"GenuineIntel"
#define AMD_VENDOR_STR		"AuthenticAMD"

#define MAX_ARCH_FILE_LEN	16
#define MAX_EVT_NAME_LEN	30
#define MAX_REG_NAME_LEN    	20
#define MAX_ADDR_AS_CHAR_LEN	9
#define MAX_RTYPE_AS_CHAR_LEN	2
#define MAX_CTRNUM_AS_CHAR_LEN	3

//Vendor, family, model combinations
#define NONSUPPORT_ID		-1

#define INTEL_15_6_STR		"Intel-15-6"
#define INTEL_15_6_ID		1

#define AMD_15_65_STR		"AMD-15-65"
#define AMD_15_65_ID		2

#define OK			1
#define E_END_OF_LIST		-1
#define E_EMPTY_LIST		-2
#define E_NULL_NODE		-3
#define E_FORMAT		-4
#define E_NO_NAME		-5
#define E_NO_INDEX		-6

//#define DEBUG_ARCH

struct ArchId {
   char vendor[16];
   int family;
   int model;
};

struct RegNode {
    int rType;   
    char * name;
    int number;
    unsigned long addr;
    struct RegNode * next;
};

struct EventNode {
    char * name;
    int index;
    int setCount;
    struct RegNode registerSets[MAX_REG_SETS];
    struct EventNode * next;
};
 
class Arch{
   struct charNode {
      char line [MAX_LINE_LEN];
      struct charNode * next;
   };

   public:
      //Arch();
      Arch(const char * archStr);
      //Arch(const Arch &);
      ~Arch();
      //bool addLine(const char * data);
      bool readFile();
      bool isEmpty() const;
      int getCount() const;
      void resetIterator();
      int incrementIterator();
      int printIterator() const;
      bool parseEvent(EventNode * e);
      bool getIteratorStatus() const;
      void printNodes() const;
      
   private:
      Arch(const Arch & a){} 
      Arch & operator=(const Arch & a) {return * this;}
      charNode * head;
      charNode * tail;
      charNode * iterator;
      int count;
      int archId;
      char archStr[MAX_ARCH_FILE_LEN];
      int determineArchitecture(const char * archStr);
      bool addNode (const char * data);
      bool addLine(const char * data);
};

#endif //Arch_h

