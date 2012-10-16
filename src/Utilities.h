#if !defined( Utilities_h )
#define Utilities_h

class Utilities {
public:
    static const long MaxFileNameLen   = 512; 

    static int isprefix    ( const char* prefix, const char* string );
    static int iscaseprefix( const char* prefix, const char* string );

    static char* read_stdin( long &len );
    static char* read_file ( const char* file, long &len );

    static int can_read_file  ( const char* file );
    static int can_write_file ( const char* file );

private:
    Utilities();
    ~Utilities();
};

#endif
