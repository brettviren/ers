#include <dlfcn.h>
#include <ers/internal/Util.h>
#include <ers/internal/PluginManager.h>
#include <ers/internal/macro.h>
#include <ers/ers.h>
#include <vector>
#include <iostream>

namespace
{
    const std::string SEPARATOR = ":";
    const std::string DefaultLibraryName = "ErsBaseStreams";
    const char * const EnvironmentName = "TDAQ_ERS_STREAM_LIBS";
}

namespace ers
{

    PluginManager::SharedLibrary::SharedLibrary( const std::string & name )
    {
    #ifdef __macos__
	std::string library = "lib" + name + ".dylib" ;
    #else
	std::string library = "lib" + name + ".so" ;
    #endif

	handle_ = dlopen( library.c_str(), RTLD_LAZY|RTLD_GLOBAL );
	char * error = dlerror();

	if ( !handle_ )
	{
	    throw PluginException( error );
	}
    }

    PluginManager::SharedLibrary::~SharedLibrary( )
    {
	////////////////////////////////////////////////////////////////////
	// Library should be unloaded, but with some compilers (e.g gcc323)
	// this results in crash of program, because it is not recognized
	// that the library is still in use
	////////////////////////////////////////////////////////////////////

	//dlclose( handle_ );
    }

    PluginManager::~PluginManager()
    {
	LibMap::iterator it = libraries_.begin();
	for ( ; it != libraries_.end(); )
	{
	    delete it->second;
	    libraries_.erase( it++ );
	}
    }

    PluginManager::PluginManager( )
    {
	const char * env = ::getenv( EnvironmentName );
        std::string config( env ? env + SEPARATOR + DefaultLibraryName : DefaultLibraryName );
        
        std::vector<std::string> libs;
    	ers::tokenize( config, SEPARATOR, libs );
        
	for ( size_t i = 0; i < libs.size(); i++ )
	{
	    LibMap::iterator it = libraries_.find( libs[i] );
	    if ( it == libraries_.end() )
	    {
		try
                {
                    libraries_[libs[i]] = new SharedLibrary( libs[i] );
                }
                catch( PluginException & ex )
                {
		    ERS_INTERNAL_DEBUG( 1, "Library " << libs[i] << " can not be loaded because " << ex.reason() )
                }
	    }
	}
    }
}
