/*
 *  IssueFactory.cxx
 *  ers
 *
 *  Created by Matthias Wiesmann on 30.11.04.
 *  Modified by Serguei Kolos on 10.08.05.
 *  Copyright 2005 CERN. All rights reserved.
 *
 */

#include <ers/IssueFactory.h>
#include <ers/StreamFactory.h>
#include <ers/Issue.h>
#include <ers/ers.h>

ERS_DECLARE_ISSUE( ers, DefaultIssue, , )

/** Returns the singleton instance of the factory.
  * \return a pointer to the singleton instance 
  */
ers::IssueFactory &
ers::IssueFactory::instance()
{
    static ers::IssueFactory instance;
    return instance;
} // instance

/** Register an issue type with the factory 
  * \param name the name that will be used to lookup new instances 
  * \param creator a pointer to the function used to create new instance for that particular type of function
  */
void 
ers::IssueFactory::register_issue( const std::string & name, IssueCreator creator )
{
    FunctionMap::const_iterator it = m_creators.find(name); 
    if ( it != m_creators.end() )
    {
	ERS_WARNING( "Creator for the \"" << name << "\" Issue has been already registered" )
    }
    m_creators[name] = creator;
} // register_Issue

/** Builds an issue out of the name it was registered with 
  * \param name the name used to indentify the class 
  * \return an newly allocated instance of type \c name or DefaultIssue 
  * \note If the requested type cannot be resolved an instance of type DefaultIssue 
  */
ers::Issue * 
ers::IssueFactory::create( const std::string & name ) const
{
    FunctionMap::const_iterator it = m_creators.find(name); 
    if ( it == m_creators.end() )
    {
	return new DefaultIssue( ERS_HERE );
    }
    
    return (it->second)(); 
} // build

