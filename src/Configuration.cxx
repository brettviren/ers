/*
 *  Configuration.cxx
 *  ERS
 *
 *  Created by Matthias Wiesmann on 21.01.05.
 *  Copyright 2005 CERN. All rights reserved.
 *
 */

#include <iostream>

#include <ers/Configuration.h>
#include <ers/ers.h>
#include <ers/internal/SingletonCreator.h>

namespace
{
    void
    read_environment( const char * name, int & value )
    {
	const char * env = ::getenv( name );
	if ( env )
	{
	    if ( sscanf( env, "%d", &value ) != 1 )
	    {
		ERS_WARNING( "Wrong value \"" << env << "\" is given for the \"" << name << "\" environment" )
	    }
	}
    }
}


namespace
{
    ers::SingletonCreator<ers::Configuration>	_creator_;
}

/** This method returns the singleton instance. 
  * It should be used for every operation on the factory. 
  * \return a pointer to the singleton instance 
  */
ers::Configuration &
ers::Configuration::instance()
{
    /**Singleton instance
      */
    static ers::Configuration * instance = new ers::Configuration;
    
    _creator_.dummy();
    
    return *instance;
} // instance

/** Private constructor - can not be called by user code, use the \c instance() method instead
  * \see instance() 
  */
ers::Configuration::Configuration()
  : m_debug_level( 0 ),
    m_verbosity_level( 0 )
{
    static bool initialised = false;
    if ( !initialised )
    {
	initialised = true;
        read_environment( "TDAQ_ERS_DEBUG_LEVEL", m_debug_level );
	read_environment( "TDAQ_ERS_VERBOSITY_LEVEL", m_verbosity_level );
    }
}


std::ostream & 
ers::operator<<( std::ostream & out, const ers::Configuration & conf )
{
    out << "debug level = " << conf.m_debug_level << " verbosity level = " << conf.m_verbosity_level;
    return out;
}