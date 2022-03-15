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

#pragma once



#if defined(__clang__)
	#define FTS_COMPILER_CLANG
#elif defined(__GNUC__) || defined(__GNUG__)
	#define FTS_COMPILER_GCC
#elif defined(_MSC_VER)
	#define FTS_COMPILER_MSVC
#else
	#define FTS_COMPILER_UNKNOWN
#endif

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__) || defined(WIN64) || defined(_WIN64) || defined(__WIN64__)
	#define FTS_PLATFORM_WINDOWS
#elif defined(__linux__)
	#define FTS_PLATFORM_LINUX
#elif defined(__APPLE__)
	#define FTS_PLATFORM_APPLE
#else
	#define FTS_PLATFORM_UNKNOWN
#endif


#include <atomic>
#ifdef FTS_PLATFORM_UNKNOWN
	#include <mutex>
#endif
#ifdef FTS_PLATFORM_LINUX
	#include <unistd.h>
	#include <sys/syscall.h>
	#include <linux/futex.h>
#endif
#if FTS_PLATFORM_WINDOWS
	#include <windows.h>
#endif
#include <iostream>
#include <thread>
#include <chrono>


namespace fts
{
	class SpinLock
	{
		public:
			inline void lock();
			inline void unlock();
			inline bool try_lock();

			SpinLock();
		
		private:
			std::atomic_bool m_isLocked;
	};
	class AdaptiveLock
	{
		public:
			inline void lock();
			inline void unlock();
			inline bool try_lock();

			AdaptiveLock();
		
		private:
			int32_t m_address;
			#ifdef FTS_PLATFORM_UNKNOWN
			std::mutex m_mutex;
			#endif
	};
	class SpinSemaphore
	{
		public:
			inline void lock();
			inline void unlock();
			inline bool try_lock();
			
			inline void unlockDestoryCounter();

			inline void addCounter(int32_t n = 1);
			inline void removeCounter(int32_t n = 1);

			inline int32_t numCounters() const;

			SpinSemaphore();
			SpinSemaphore(int32_t max);
		
		private:
			std::atomic_int32_t m_counter;
	};
	class AdaptiveSemaphore
	{
		public:
			inline void lock();
			inline void unlock();
			inline bool try_lock();

			inline void unlockDestoryCounter();

			inline void addCounter(int32_t n = 1);
			inline void removeCounter(int32_t n = 1);

			inline int32_t numCounters() const;

			AdaptiveSemaphore();
			AdaptiveSemaphore(int32_t max);
		
		private:
			std::atomic_int32_t m_counter;
			int32_t m_address;
			#ifdef FTS_PLATFORM_UNKNOWN
			std::mutex m_mutex;
			#endif
	};

	class Signal
	{
		public:
			inline void wait();
			inline void wake();
			inline void wakeAll();

			Signal();
		
		private:
			int32_t m_address;
			#ifdef FTS_PLATFORM_UNKNOWN
			std::mutex m_mutex;
			std::atomic_bool m_unlocked;
			std::atomic_int32_t m_numWaiting;
			#endif
	};
	class SpinSignal
	{
		public:
			inline void wait();
			inline void wake();
			inline void wakeAll();

			SpinSignal();
		
		private:
			std::atomic_char m_isWaiting;
	};

	class Flag
	{
		public:
			inline void raise();
			inline void lower();
			inline bool isRaised();

			Flag();
		
		private:
			std::atomic_bool m_isRaised;
	};

	class ReadWriteLock
	{
		public:
			inline void readLock();
			inline void writeLock();
			inline void readUnlock();
			inline void writeUnlock();
			inline bool readTryLock();
			inline bool writeTryLock();

			ReadWriteLock();
		
		private:
			std::atomic_int32_t m_numReaders;
			std::atomic_bool m_writeRequest;
	};



	template<typename LockT>
	class GenericLockGuard
	{
		public:
			inline GenericLockGuard(LockT& lock);
			explicit inline GenericLockGuard(LockT* lock);
			inline ~GenericLockGuard();
		private:
			LockT* m_lock;
	};

	template<typename SemaphoreT>
	class SemaphoreDestoryCounterLockGuard
	{
		public:
			inline SemaphoreDestoryCounterLockGuard(SemaphoreT& semaphore);
			explicit inline SemaphoreDestoryCounterLockGuard(SemaphoreT* semaphore);
			inline ~SemaphoreDestoryCounterLockGuard();
		private:
			SemaphoreT* m_semaphore;
	};

	class ReadWriteLockReadLockGuard
	{
		public:
			inline ReadWriteLockReadLockGuard(ReadWriteLock& readWriteLock);
			explicit inline ReadWriteLockReadLockGuard(ReadWriteLock* readWriteLock);
			inline ~ReadWriteLockReadLockGuard();
		private:
			ReadWriteLock* m_readWriteLock;
	};
	class ReadWriteLockWriteLockGuard
	{
		public:
			inline ReadWriteLockWriteLockGuard(ReadWriteLock& readWriteLock);
			explicit inline ReadWriteLockWriteLockGuard(ReadWriteLock* readWriteLock);
			inline ~ReadWriteLockWriteLockGuard();
		private:
			ReadWriteLock* m_readWriteLock;
	};
}

#include "fts.inl"

//if uses as a single header include an implemenation is nessesary
#ifdef FTS_SINGLE_HEADER_IMPLEMENTATION
	#include "fts.cpp"
#endif