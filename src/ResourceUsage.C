#include <stdio.h>
#include <sys/time.h>

#include "ResourceUsage.h"

ResourceUsage::ResourceUsage( int who )
{
    if ( who == SELF ) {
	getrusage( RUSAGE_SELF, &this->ru );
    } else {
	getrusage( RUSAGE_CHILDREN, &this->ru );
    }
}

ResourceUsage::~ResourceUsage()
{
}
