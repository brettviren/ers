/*
 *  StreamFactory.cxx
 *  ERS
 *
 *  Created by Serguei Kolos on 21.01.05.
 *  Modified by Serguei Kolos on 21.11.05.
 *  Copyright 2005 CERN. All rights reserved.
 *
 */

#include <iostream>

#include <ers/Issue.h>
#include <ers/OutputStream.h>
#include <ers/StreamFactory.h>
#include <ers/Severity.h>
#include <ers/ers.h>
#include <ers/internal/Util.h>
#include <ers/internal/NullStream.h>
#include <ers/internal/PluginManager.h>
#include <ers/internal/macro.h>
#include <ers/internal/SingletonCreator.h>

/** This method returns the singleton instance. 
  * It should be used for every operation on the factory. 
  * \return a reference to the singleton instance 
  */
ers::StreamFactory &
ers::StreamFactory::instance()
{
    /**Singleton instance
      */
    static ers::StreamFactory * instance = ers::SingletonCreator<ers::StreamFactory>::create();
    
    return *instance;
} // instance

/** Builds a stream from a textual key 
  * The key should have the format \c stream_name[(stream_parameters)]
  * For some streams parameters can be ommitted. 
  * For instance to write to the error stream, the key is: 
  * \c stderr
  * \param format the format, which describes new stream
  * \note the stream is allocated on the heap, it is the caller's responsibility to delete it.
  */
ers::OutputStream *
ers::StreamFactory::create_out_stream( const std::string & format ) const
{
    std::string key = format;
    std::string param;
    std::string::size_type start = format.find( '(' );    
    if ( start != std::string::npos )
    {
	key = format.substr( 0, start );
	std::string::size_type end = format.find( ')', start );
        if ( end != std::string::npos )
            param = format.substr( start + 1, end - start - 1 );
    }    	

    OutFunctionMap::const_iterator it = m_out_factories.find( key );
    
    if( it != m_out_factories.end() )
    {
	try
        {
            return it->second( param );
        }
        catch( ers::Issue & issue )
        {
            ERS_INTERNAL_ERROR( issue )
        }
    }
    else
    {
    	ERS_INTERNAL_ERROR( "Creator for the \"" << key << "\" stream is not found" )
    }
    
    return 0; 
}

/** Builds a stream from a textual key 
  * The key should have the format \c stream_name[(stream_parameters)]
  * For some streams parameters can be ommitted. 
  * For instance to write to the error stream, the key is: 
  * \c stderr
  * \param param parameter to be passed to the new stream constructor
  * \note the stream is allocated on the heap, it is the caller's responsibility to delete it.
  */
ers::InputStream *
ers::StreamFactory::create_in_stream(	const std::string & stream, 
        				const std::string & param ) const
{
    return create_in_stream( stream, { param } );
}

/** Builds a stream from a textual key 
  * The key should have the format \c stream_name[(stream_parameters)]
  * For some streams parameters can be ommitted. 
  * For instance to write to the error stream, the key is: 
  * \c stderr
  * \param params parameters to be passed to the new stream constructor
  * \note the stream is allocated on the heap, it is the caller's responsibility to delete it.
  */
ers::InputStream *
ers::StreamFactory::create_in_stream(	
	const std::string & stream, 
	const std::initializer_list<std::string> & params ) const
{
    InFunctionMap::const_iterator it = m_in_factories.find( stream );
    
    if( it != m_in_factories.end() )
    {
	try
        {
            return it->second( params );
        }
        catch( ers::Issue & issue )
        {
	    throw ers::InvalidFormat( ERS_HERE, stream, issue ); 
        }
    }
    
    throw ers::InvalidFormat( ERS_HERE, stream ); 
}

/** Registers a factory function with the stream factory.
  * The callback is function that takes two parameters <ol>
  * <li>a string describing the protocol, for instance \c file </li>
  * <li>a string describing a uri, can be a path, a suffix or anything</li>
  * </ol>
  * The function should return a heap allocated stream, or null if it does not
  * understand the protocol. 
  * \param name name of the stream type (human display only).
  * \param callback the creator function
  */
void
ers::StreamFactory::register_in_stream( const std::string & name, InputStreamCreator callback )
{
    m_in_factories[name] = callback;
}

/** Registers a factory function with the stream factory.
  * The callback is function that takes two parameters <ol>
  * <li>a string describing the protocol, for instance \c file </li>
  * <li>a string describing a uri, can be a path, a suffix or anything</li>
  * </ol>
  * The function should return a heap allocated stream, or null if it does not
  * understand the protocol. 
  * \param name name of the stream type (human display only).
  * \param callback the creator function
  */
void
ers::StreamFactory::register_out_stream( const std::string & name, OutputStreamCreator callback )
{
    m_out_factories[name] = callback;
}

std::ostream & 
ers::operator<<( std::ostream & out, const ers::StreamFactory & sf )
{
    StreamFactory::OutFunctionMap::const_iterator oit = sf.m_out_factories.begin();
    for( ; oit != sf.m_out_factories.end(); ++oit )
    {	
	out << oit->first << "\t\"" << oit->second << "\"" << std::endl;
    }
    
    StreamFactory::InFunctionMap::const_iterator iit = sf.m_in_factories.begin();
    for( ; iit != sf.m_in_factories.end(); ++iit )
    {	
	out << iit->first << "\t\"" << iit->second << "\"" << std::endl;
    }
    return out;
}
