"""Error reporting test program

This module is part of the Error Reporting Service (ERS) 
package of the ATLAS TDAQ system.
"""

__author__ = "Serguei Kolos (Serguei.Kolos@cern.ch)"
__version__ = "$Revision: 1.0 $"
__date__ = "$Date: 2010/05/05 21:57:20 $"

# It's necessay to properly instantiate exceptions in the underlying C++ modules
import sys, dl
sys.setdlopenflags(dl.RTLD_LAZY|dl.RTLD_GLOBAL)

import ers
import ipc
import logging

class Exception( ers.Issue ):
    def __init__( self, msg, args, cause ):
	ers.Issue.__init__( self, msg, args, cause )

class PermissionDenied( Exception ):
    def __init__( self, cause = None ):
	Exception.__init__( self, 'Permission denied', {}, cause )

class CantOpenFile( Exception ):
    def __init__( self, fname, err, cause = None ):
	Exception.__init__( 
	    self,
	    'Can not open "%s" file, error = %d' % ( fname, err ),
	    { 'filename' : fname, 'error' : err },
	    cause )

cof = CantOpenFile( "t", 1 )

#create the logger
logger = logging.getLogger("main")
logger.setLevel(logging.DEBUG)

#add ers handler
ers.addLoggingHandler("main")

class Test:
    def method(self):
        raise PermissionDenied( )
            
def function( arg1, arg2 ):
    logger.debug('This is a debug message')
    logger.info('This is an info message')
    logger.warning('This is a warning message')
    logger.error('This is an error message')
    logger.critical('This is a critical error message')
    try:
        t = Test( )
        t.method( )
    except PermissionDenied, e:
	raise CantOpenFile( "test.py", 13, e )

import time

if __name__ == "__main__":
    try:
    	function( "test", "ers" )
    except ers.Issue, e:
	ers.warning( e )
