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
  g.set( 'b' );
  g.parse( argc, argv );
  int how_many = g.get< int >( 'a' );
  int how_long = g.get< int >( 'b' );
  stopwatch sw( stopwatch::USEC );
  stringstream buffer;

  for ( int j = 0; j < how_long; j++ )
  {
      buffer << "." ;
  }
  buffer << endl;

  sw.reset();
  for ( int i = 0; i < how_many; i++ )
  {
      ::write( STDERR_FILENO, (void*)buffer.str().c_str(), buffer.str().size() );
  }
  sw.stop();
  cout << sw.get_time() << endl;
  return EXIT_SUCCESS;
}
