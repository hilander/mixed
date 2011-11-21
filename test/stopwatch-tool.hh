#ifndef __STOPWATCH_TOOL_HH__
#define __STOPWATCH_TOOL_HH__

#include <vector>
#include <algorithm>
#include <string>
#include <sys/time.h>
#include <tr1/functional>

namespace ___stopwatch_ns
{
  using namespace std;
  using namespace std::tr1;
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
        ::gettimeofday( &start_val, 0 );
        val_splits.clear();
      }

      void stop()
      {
        ::gettimeofday( &end_val, 0 );
      }

      void split()
      {
        ::timeval current_split;
        ::gettimeofday( &current_split, 0 );
        val_splits.push_back( current_split );
      }

      long get_time()
      {
        return end_stamp() - start_stamp();
      }

      vector< long > get_splits()
      {
        vector< long > res( val_splits.size() );
        transform( val_splits.begin(), val_splits.end()
                 , res.begin()
                 , bind( &stopwatch::split_stamp, this, placeholders::_1 ) );
        return res;
      }

      string str()
      {
        switch ( clock_resolution )
        {
          case SEC:
            return string( "seconds" );

          case MSEC:
            return string( "milliseconds" );

          case USEC:
            return string( "microseconds" );

          default:
            return string( "??" );
        }
      }

    private:
      long end_stamp()
      {
        return end_val.tv_sec * SEC/clock_resolution + end_val.tv_usec / clock_resolution;
      }

      long start_stamp()
      {
        return start_val.tv_sec * SEC/clock_resolution + start_val.tv_usec / clock_resolution;
      }

      long split_stamp( ::timeval split_val )
      {
        return split_val.tv_sec * SEC/clock_resolution + split_val.tv_usec / clock_resolution;
      }

    private:
      resolution clock_resolution;

      ::timeval start_val;
      ::timeval end_val;
      vector< ::timeval > val_splits;

  };

}

using namespace ___stopwatch_ns;

#endif
