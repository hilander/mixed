#ifndef __STOPWATCH_TOOL_HH__
#define __STOPWATCH_TOOL_HH__

#include <vector>
#include <algorithm>
#include <string>
#include <sys/time.h>

namespace ___stopwatch_ns
{
  using namespace std;
  class stopwatch
  {
    public:
      enum resolution
      {
        SEC = 1000000, // seconds
        MSEC =   1000, // milliseconds
        USEC =      1  // microseconds
      };

    public:
      stopwatch( resolution r = MSEC ) : clock_resolution( r ) {}
      virtual ~stopwatch() {}

      void reset()
      {
      }

      void stop()
      {
      }

      void split()
      {
      }

      long get_time()
      {
        return end_stamp() - start_stamp();
      }

      void get_splits( vector< long >& splits )
      {
      }

      template< int > string str();

    private:
      long end_stamp()
      {
        return 0;
      }

      long start_stamp()
      {
        return 0;
      }

    private:
      resolution clock_resolution;

  };

      template<> string stopwatch::str< stopwatch::SEC >()
      {
        return string( "seconds" );
      }

      template<> string stopwatch::str< stopwatch::MSEC >()
      {
        return string( "milliseconds" );
      }

      template<> string stopwatch::str< stopwatch::USEC >()
      {
        return string( "microseconds" );
      }

}

using namespace ___stopwatch_ns;

#endif
