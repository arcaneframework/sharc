// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2025 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
/*
 * Exception
 *
 *  Created on: 4 nov. 2014
 *      Author: gratienj
 */

#ifndef ARCGEOSIM_UTILS_EXCEPTION_H
#define ARCGEOSIM_UTILS_EXCEPTION_H


namespace ArcGeoSim
{
  namespace Exception
  {
    class BaseException : public std::exception
    {
    public:
        BaseException( const char * type, const char * msg, int line )
        {
            std::ostringstream oss;
            oss << "Error type "<<type<<" line " << line << " : "
                << msg;
            this->msg = oss.str();
        }

        virtual ~BaseException() throw()
        {

        }

        virtual const char * what() const throw()
        {
            return this->msg.c_str();
        }

    private:
        std::string msg;
    };

    class ErrorException : public BaseException
    {
    public :
      ErrorException(std::string const& msg,int line)
      : BaseException("Error",msg.c_str(),line)
      {}
    } ;

    class RuntimeException : public BaseException
    {
    public :
      RuntimeException(std::string const& msg,int line)
      : BaseException("Runtime",msg.c_str(),line)
      {}
    } ;


    class NumericException : public BaseException
    {
    public :
      NumericException(std::string const& msg,int line)
      : BaseException("Numeric",msg.c_str(),line)
      {}
    } ;
  }
}


#endif /*ARCGEOSIM_UTILS_EXCEPTION_H */
