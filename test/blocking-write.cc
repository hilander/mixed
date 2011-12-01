#include <sstream>
#include <iostream>
using namespace std;
#include <cstdlib>
#include <sys/time.h>
#include <fcntl.h>

#include <gtest/gtest.h>

#include "stopwatch-tool.hh"

#include "getopt.hh"

int main( int argc, char* argv[] )
{
  getopts g;
  g.set( 'a' );
  g.set( 'B' );
  g.parse( argc, argv );
  cout << "For 'a' we have: " << g.get< int >( 'a' ) << endl;
  cout << "For 'B' we have: " << g.get< string >( 'B' ) << endl;
  stopwatch sw( stopwatch::USEC );
  stringstream buffer;
  /*
  for ( int i = 0; i < 1000; i++ )
  {
    buffer << "." << endl;
  }
  */
  sw.reset();
  ::write( STDOUT_FILENO, (void*)buffer.str().c_str(), buffer.str().size() );
  sw.stop();
  cout << sw.get_time() << endl;
  return EXIT_SUCCESS;
}
