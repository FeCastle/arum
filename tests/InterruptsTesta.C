#define TESTFILE "accounts.alf"
extern FILE* Interrupts_in;

int
main( int ac, char *av[] )
{
    Interrupts_in = fopen(TESTFILE, "r");
    Interrupts_parse();

    return 0;
}

