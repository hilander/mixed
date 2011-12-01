#ifndef __GETOPT_HH__
#define __GETOPT_HH__

#include <unistd.h>
#include <map>
#include <algorithm>
#include <tr1/memory>
#include <tr1/functional>
#include <string>
#include <sstream>

namespace ___getopt_ns
{
  using namespace std;
  using namespace std::tr1;

  class getopts : public enable_shared_from_this< getopts >
  {
    public:
      getopts()
      {
        for ( char c = 'a'; c <= 'z'; c++ )
        {
          opt_substrings.insert( make_pair( c, false ) );
          option_vals.insert( make_pair( c, string( "" ) ) );
        }

        for ( char c = 'A'; c <= 'Z'; c++ )
        {
          opt_substrings.insert( make_pair( c, false ) );
          option_vals.insert( make_pair( c, string( "" ) ) );
        }
      }
      
      virtual ~getopts()
      {
      }

      void update( pair< char, bool > p )
      {
        char ct[2];
        ct[ 0 ] = p.first;
        ct[ 1 ] = '\0';
        string oo( string( ct ) 
            + (p.second ? string( ":" ) : string( "" ) ) );
        o = o + oo;
      }

      void unset( char c )
      {
        opt_substrings[ c ] = false;
      }

      void set( char c )
      {
        opt_substrings[ c ] = true;
      }

      /*
      string get( char c )
      {
        return option_vals[ c ];
      }
      */

      void parse( int argc, char* const argv[] )
      {
        for_each( opt_substrings.begin()
                , opt_substrings.end()
                , bind( &getopts::update, this, placeholders::_1 ) );
        int foundopt;
        while ( ( foundopt = ::getopt( argc, argv, o.c_str() ) )  > 0 )
        {
          char optc = (char)foundopt;
          if ( opt_substrings[ optc ] )
          {
            option_vals[ optc ] = optarg;
          }
        }
      }

      template< typename outclass > outclass get( char c )
      {
        stringstream ss;
        ss << option_vals[ c ];
        outclass rv;
        ss >> rv;
        return rv;
      }

    private:
      map< char, bool > opt_substrings;
      map< char, string > option_vals;
      string            o;
  };
}

using namespace ___getopt_ns;

#endif
