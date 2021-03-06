/*
 *  ThrowStream.cxx
 *  ers
 *
 *  Created by Serguei Kolos on 02.08.05.
 *  Copyright 2004 CERN. All rights reserved.
 *
 */

#include <ers/internal/ThrowStream.h>

ERS_REGISTER_OUTPUT_STREAM( ers::ThrowStream, "throw", ERS_EMPTY)

void ers::ThrowStream::write( const Issue & issue )
{
    chained().write( issue );
    issue.raise();
}




