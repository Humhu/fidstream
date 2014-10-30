#ifndef _FIDUCIALSTREAM_TYPES_H_
#define _FIDUCIALSTREAM_TYPES_H_

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/thread.hpp>
#include <boost/thread/locks.hpp>

namespace fidstr {

	/*! \typedef Boost Posix time is the standard representation for time in the system.
	 * It should be used for timestamping and synchronization. */
	typedef boost::posix_time::ptime Time;
	typedef boost::posix_time::time_duration TimeDuration;

	typedef boost::shared_mutex Mutex;
	typedef boost::unique_lock<Mutex> WriteLock;
	typedef boost::shared_lock<Mutex> ReadLock;
	
}

#endif