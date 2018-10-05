
namespace impl {

// #include <chrono>
// #include <thread>

// using std::this_thread::sleep_for;     // sleep_for, sleep_until
// using std::this_thread::sleep_until;     // sleep_for, sleep_until
// using std::chrono_literals::s;
// using std::chrono_literals::ms; // ns, us, ms, s, h, etc.
// using std::chrono_literals::ns;

// using std::chrono::system_clock;

// //sleep_for(10ns);
// //sleep_until(system_clock::now() + 1s);



#if defined(__WIN32__) || defined(_WIN32) || defined(WIN32) || defined(__WINDOWS__) || defined(__TOS_WIN__)

#include <windows.h>

inline void delayMS( unsigned long ms ) {
  Sleep( ms );
}

#else  /* presume POSIX */

#include <unistd.h>

inline void delayMS( unsigned long ms ) {
  usleep( ms * 1000 );
}

#endif

} // impl
