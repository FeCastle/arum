#include "Utilities.h"

#include <time.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/stat.h>

int
Utilities::isprefix( const char* prefix, const char* string )
{
    for (int i=0; prefix[i] != '\0'; i++) { 
	if (string[i] == '\0') { 
			    // content is too short
	    return 0;
	} else if (string[i] != prefix[i]) { 
			    // string doesn't match
	    return 0;
	}
    }
    return 1;
}

int
Utilities::iscaseprefix( const char* prefix, const char* string )
{
    for (int i=0; prefix[i] != '\0'; i++) { 
	if (string[i] == '\0') { 
			    // content is too short
	    return 0;
	} else if (tolower(string[i]) != tolower(prefix[i])) { 
			    // string doesn't match
	    return 0;
	}
    }
    return 1;
}

char*
Utilities::read_stdin( long &len )
{
    char* result = NULL;

    char fname[ Utilities::MaxFileNameLen ];
    sprintf( fname, "/tmp/%d", time(NULL) );
    FILE *fp = fopen( fname, "w+" );
    if (fp != NULL) {
	unlink( fname );
	int c;
	for(len=0; (c=fgetc(stdin)) != EOF; len++) {
	    fputc(c, fp);
	}
	rewind(fp);
	result = new char[ len+1 ];
	fread( result, len, 1, fp );
	result[ len ] = '\0';
	fclose( fp );
    }

    return result;
}

char*
Utilities::read_file( const char *file, long &len )
{
    char* result = NULL;

    FILE *fp = fopen( file, "r" );
    if (fp != NULL) {
	fseek( fp, 0L, SEEK_END );
	len = ftell( fp );
	rewind(fp);
	result = new char[ len+1 ];
	fread( result, len, 1, fp );
	result[ len ] = '\0';
	fclose( fp );
    } else {
	len = 0L;
    }

    return result;
}

int
Utilities::can_read_file( const char *file )
{
    struct stat sb;
    if (stat( file, &sb ) == 0) {
				// file must exist
	if (S_ISREG(sb.st_mode)) {
				// file must be a regular file
	    if (sb.st_mode & (S_IRUSR | S_IRGRP | S_IROTH)) {
				// file must be readable
		return 1;
	    }
	}
    }

    return 0;
}

int
Utilities::can_write_file( const char *file )
{
    struct stat sb;
    if (stat( file, &sb ) == 0) {
				// file must exist
	if (S_ISREG(sb.st_mode)) {
				// file must be a regular file
	    if (sb.st_mode & (S_IWUSR | S_IWGRP | S_IWOTH)) {
				// file must be writable
		return 1;
	    }
	}
    }

    return 0;
}
