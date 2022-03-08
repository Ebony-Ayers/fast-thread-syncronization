//MIT License
//
//Copyright (c) 2021 Ebony Ayers
//
//Permission is hereby granted, free of charge, to any person obtaining a copy
//of this software and associated documentation files (the "Software"), to deal
//in the Software without restriction, including without limitation the rights
//to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//copies of the Software, and to permit persons to whom the Software is
//furnished to do so, subject to the following conditions:
//
//The above copyright notice and this permission notice shall be included in all
//copies or substantial portions of the Software.
//
//THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
//SOFTWARE.

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

fts::ReadWriteLock::ReadWriteLock()
: m_numReaders(0), m_writeRequest(false) {}