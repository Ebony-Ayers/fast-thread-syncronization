#include "fts.hpp"

fts::SpinLock::SpinLock()
: m_isLocked(false) {}


fts::AdaptiveLock::AdaptiveLock()
: m_address(0) {}


fts::SpinSemaphore::SpinSemaphore()
: m_counter(1) {}
fts::SpinSemaphore::SpinSemaphore(int32_t max)
: m_counter(max) {}


fts::AdaptiveSemaphore::AdaptiveSemaphore()
: m_counter(1), m_address(0) {}
fts::AdaptiveSemaphore::AdaptiveSemaphore(int32_t max)
: m_counter(max), m_address(0) {}


fts::Signal::Signal()
: m_address(0)
#ifdef FTS_PLATFORM_UNKNOWN
, m_unlocked(false), m_numWaiting(0)
#endif
{
	#ifdef FTS_PLATFORM_UNKNOWN
	m_mutex.lock();
	#endif
}

fts::SpinSignal::SpinSignal()
: m_isWaiting(false) {}

fts::Flag::Flag()
: m_isRaised(false) {}