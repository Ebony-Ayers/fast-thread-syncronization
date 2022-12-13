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
#ifndef FTS_HPP_HEADER_GUARD
#define FTS_HPP_HEADER_GUARD


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
#ifdef FTS_PLATFORM_WINDOWS
	#include <windows.h>
#endif
#include <iostream>
#include <thread>
#include <chrono>

//duplicate macros available at the bottom of the file to allow multiple macros per scope
#define FTS_GENERIC_LOCKGUARD(l) fts::GenericLockGuard ftsMacroGenericLockGuardInstance(l);
#define FTS_SEMAPHORE_DESTORY_COUNTER_LOCKGUARD(l) fts::SemaphoreDestoryCounterLockGuard ftsMacroSemaphoreDestoryCounterLockGuardInstance(l);
#define FTS_READ_WRITE_READ_LOCKGUARD(l) fts::ReadWriteLockReadLockGuard ftsMacroReadWriteLockReadLockGuardInstance(l);
#define FTS_READ_WRITE_WRITE_LOCKGUARD(l) fts::ReadWriteLockWriteLockGuard ftsMacroReadWriteLockWriteLockGuardInstance(l);

namespace fts
{
	class SpinLock
	{
		public:
			inline void lock();
			inline void unlock();
			inline bool try_lock();

			SpinLock();
			SpinLock(const SpinLock&) = delete;
			SpinLock(SpinLock&&) = delete;
			
			SpinLock& operator=(const SpinLock&) = delete;
			SpinLock& operator=(SpinLock&&) = delete;
		
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
			AdaptiveLock(const AdaptiveLock&) = delete;
			AdaptiveLock(AdaptiveLock&&) = delete;

			AdaptiveLock& operator=(const AdaptiveLock&) = delete;
			AdaptiveLock& operator=(AdaptiveLock&&) = delete;
		
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
			SpinSemaphore(const SpinSemaphore&) = delete;
			SpinSemaphore(SpinSemaphore&&) = delete;

			SpinSemaphore& operator=(const SpinSemaphore&) = delete;
			SpinSemaphore& operator=(SpinSemaphore&&) = delete;
		
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
			AdaptiveSemaphore(const AdaptiveSemaphore&) = delete;
			AdaptiveSemaphore(AdaptiveSemaphore&&) = delete;

			AdaptiveSemaphore& operator=(const AdaptiveSemaphore&) = delete;
			AdaptiveSemaphore& operator=(AdaptiveSemaphore&&) = delete;
		
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

			inline bool hasWaitingThread();

			Signal();
			Signal(const Signal&) = delete;
			Signal(Signal&&) = delete;

			Signal& operator=(const Signal&) = delete;
			Signal& operator=(Signal&&) = delete;
		
		private:
			int32_t m_address;
			std::atomic_int32_t m_numWaiting;
			#ifdef FTS_PLATFORM_UNKNOWN
			std::mutex m_mutex;
			std::atomic_bool m_unlocked;
			#endif
	};
	class SpinSignal
	{
		public:
			inline void wait();
			inline void wake();
			inline void wakeAll();

			inline bool hasWaitingThread();

			SpinSignal();
			SpinSignal(const SpinSignal&) = delete;
			SpinSignal(SpinSignal&&) = delete;

			SpinSignal& operator=(const SpinSignal&) = delete;
			SpinSignal& operator=(SpinSignal&&) = delete;
		
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
			Flag(const Flag&) = delete;
			Flag(Flag&&) = delete;

			Flag& operator=(const Flag&) = delete;
			Flag& operator=(Flag&&) = delete;
		
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
			ReadWriteLock(const ReadWriteLock&) = delete;
			ReadWriteLock(ReadWriteLock&&) = delete;

			ReadWriteLock& operator=(const ReadWriteLock&) = delete;
			ReadWriteLock& operator=(ReadWriteLock&&) = delete;
		
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
			GenericLockGuard(const GenericLockGuard<LockT>&) = delete;
			GenericLockGuard(GenericLockGuard<LockT>&&) = delete;
			inline ~GenericLockGuard();

			GenericLockGuard<LockT>& operator=(const GenericLockGuard<LockT>&) = delete;
			GenericLockGuard<LockT>& operator=(GenericLockGuard<LockT>&&) = delete;
		private:
			LockT* m_lock;
	};

	template<typename SemaphoreT>
	class SemaphoreDestoryCounterLockGuard
	{
		public:
			inline SemaphoreDestoryCounterLockGuard(SemaphoreT& semaphore);
			explicit inline SemaphoreDestoryCounterLockGuard(SemaphoreT* semaphore);
			SemaphoreDestoryCounterLockGuard(const SemaphoreDestoryCounterLockGuard<SemaphoreT>&) = delete;
			SemaphoreDestoryCounterLockGuard(SemaphoreDestoryCounterLockGuard<SemaphoreT>&&) = delete;
			inline ~SemaphoreDestoryCounterLockGuard();

			SemaphoreDestoryCounterLockGuard<SemaphoreT>& operator=(const SemaphoreDestoryCounterLockGuard<SemaphoreT>&) = delete;
			SemaphoreDestoryCounterLockGuard<SemaphoreT>& operator=(SemaphoreDestoryCounterLockGuard<SemaphoreT>&&) = delete;
		private:
			SemaphoreT* m_semaphore;
	};

	class ReadWriteLockReadLockGuard
	{
		public:
			inline ReadWriteLockReadLockGuard(ReadWriteLock& readWriteLock);
			explicit inline ReadWriteLockReadLockGuard(ReadWriteLock* readWriteLock);
			ReadWriteLockReadLockGuard(const ReadWriteLockReadLockGuard&) = delete;
			ReadWriteLockReadLockGuard(ReadWriteLockReadLockGuard&&) = delete;
			inline ~ReadWriteLockReadLockGuard();

			ReadWriteLockReadLockGuard& operator=(const ReadWriteLockReadLockGuard&) = delete;
			ReadWriteLockReadLockGuard& operator=(ReadWriteLockReadLockGuard&&) = delete;
		private:
			ReadWriteLock* m_readWriteLock;
	};
	class ReadWriteLockWriteLockGuard
	{
		public:
			inline ReadWriteLockWriteLockGuard(ReadWriteLock& readWriteLock);
			explicit inline ReadWriteLockWriteLockGuard(ReadWriteLock* readWriteLock);
			ReadWriteLockWriteLockGuard(const ReadWriteLockWriteLockGuard&) = delete;
			ReadWriteLockWriteLockGuard(ReadWriteLockWriteLockGuard&&) = delete;
			inline ~ReadWriteLockWriteLockGuard();

			ReadWriteLockWriteLockGuard& operator=(const ReadWriteLockWriteLockGuard&) = delete;
			ReadWriteLockWriteLockGuard& operator=(ReadWriteLockWriteLockGuard&&) = delete;
		private:
			ReadWriteLock* m_readWriteLock;
	};
}

#include "fts.inl"

//if uses as a single header include an implemenation is nessesary
#ifdef FTS_SINGLE_HEADER_IMPLEMENTATION
	#include "fts.cpp"
#endif

//duplicate macros to allow multiple macros in the same scope
#define FTS_GENERIC_LOCKGUARD_1(l) fts::GenericLockGuard ftsMacroGenericLockGuardInstance1(l);
#define FTS_GENERIC_LOCKGUARD_2(l) fts::GenericLockGuard ftsMacroGenericLockGuardInstance2(l);
#define FTS_GENERIC_LOCKGUARD_3(l) fts::GenericLockGuard ftsMacroGenericLockGuardInstance3(l);
#define FTS_GENERIC_LOCKGUARD_4(l) fts::GenericLockGuard ftsMacroGenericLockGuardInstance4(l);
#define FTS_GENERIC_LOCKGUARD_5(l) fts::GenericLockGuard ftsMacroGenericLockGuardInstance5(l);
#define FTS_GENERIC_LOCKGUARD_6(l) fts::GenericLockGuard ftsMacroGenericLockGuardInstance6(l);
#define FTS_GENERIC_LOCKGUARD_7(l) fts::GenericLockGuard ftsMacroGenericLockGuardInstance7(l);
#define FTS_GENERIC_LOCKGUARD_8(l) fts::GenericLockGuard ftsMacroGenericLockGuardInstance8(l);
#define FTS_GENERIC_LOCKGUARD_9(l) fts::GenericLockGuard ftsMacroGenericLockGuardInstance9(l);
#define FTS_SEMAPHORE_DESTORY_COUNTER_LOCKGUARD_1(l) fts::SemaphoreDestoryCounterLockGuard ftsMacroSemaphoreDestoryCounterLockGuardInstance1(l);
#define FTS_SEMAPHORE_DESTORY_COUNTER_LOCKGUARD_2(l) fts::SemaphoreDestoryCounterLockGuard ftsMacroSemaphoreDestoryCounterLockGuardInstance2(l);
#define FTS_SEMAPHORE_DESTORY_COUNTER_LOCKGUARD_3(l) fts::SemaphoreDestoryCounterLockGuard ftsMacroSemaphoreDestoryCounterLockGuardInstance3(l);
#define FTS_SEMAPHORE_DESTORY_COUNTER_LOCKGUARD_4(l) fts::SemaphoreDestoryCounterLockGuard ftsMacroSemaphoreDestoryCounterLockGuardInstance4(l);
#define FTS_SEMAPHORE_DESTORY_COUNTER_LOCKGUARD_5(l) fts::SemaphoreDestoryCounterLockGuard ftsMacroSemaphoreDestoryCounterLockGuardInstance5(l);
#define FTS_SEMAPHORE_DESTORY_COUNTER_LOCKGUARD_6(l) fts::SemaphoreDestoryCounterLockGuard ftsMacroSemaphoreDestoryCounterLockGuardInstance6(l);
#define FTS_SEMAPHORE_DESTORY_COUNTER_LOCKGUARD_7(l) fts::SemaphoreDestoryCounterLockGuard ftsMacroSemaphoreDestoryCounterLockGuardInstance7(l);
#define FTS_SEMAPHORE_DESTORY_COUNTER_LOCKGUARD_8(l) fts::SemaphoreDestoryCounterLockGuard ftsMacroSemaphoreDestoryCounterLockGuardInstance8(l);
#define FTS_SEMAPHORE_DESTORY_COUNTER_LOCKGUARD_9(l) fts::SemaphoreDestoryCounterLockGuard ftsMacroSemaphoreDestoryCounterLockGuardInstance9(l);
#define FTS_READ_WRITE_READ_LOCKGUARD_1(l) fts::ReadWriteLockReadLockGuard ftsMacroReadWriteLockReadLockGuardInstance1(l);
#define FTS_READ_WRITE_READ_LOCKGUARD_2(l) fts::ReadWriteLockReadLockGuard ftsMacroReadWriteLockReadLockGuardInstance2(l);
#define FTS_READ_WRITE_READ_LOCKGUARD_3(l) fts::ReadWriteLockReadLockGuard ftsMacroReadWriteLockReadLockGuardInstance3(l);
#define FTS_READ_WRITE_READ_LOCKGUARD_4(l) fts::ReadWriteLockReadLockGuard ftsMacroReadWriteLockReadLockGuardInstance4(l);
#define FTS_READ_WRITE_READ_LOCKGUARD_5(l) fts::ReadWriteLockReadLockGuard ftsMacroReadWriteLockReadLockGuardInstance5(l);
#define FTS_READ_WRITE_READ_LOCKGUARD_6(l) fts::ReadWriteLockReadLockGuard ftsMacroReadWriteLockReadLockGuardInstance6(l);
#define FTS_READ_WRITE_READ_LOCKGUARD_7(l) fts::ReadWriteLockReadLockGuard ftsMacroReadWriteLockReadLockGuardInstance7(l);
#define FTS_READ_WRITE_READ_LOCKGUARD_8(l) fts::ReadWriteLockReadLockGuard ftsMacroReadWriteLockReadLockGuardInstance8(l);
#define FTS_READ_WRITE_READ_LOCKGUARD_9(l) fts::ReadWriteLockReadLockGuard ftsMacroReadWriteLockReadLockGuardInstance9(l);
#define FTS_READ_WRITE_WRITE_LOCKGUARD_1(l) fts::ReadWriteLockWriteLockGuard ftsMacroReadWriteLockWriteLockGuardInstance1(l);
#define FTS_READ_WRITE_WRITE_LOCKGUARD_2(l) fts::ReadWriteLockWriteLockGuard ftsMacroReadWriteLockWriteLockGuardInstance2(l);
#define FTS_READ_WRITE_WRITE_LOCKGUARD_3(l) fts::ReadWriteLockWriteLockGuard ftsMacroReadWriteLockWriteLockGuardInstance3(l);
#define FTS_READ_WRITE_WRITE_LOCKGUARD_4(l) fts::ReadWriteLockWriteLockGuard ftsMacroReadWriteLockWriteLockGuardInstance4(l);
#define FTS_READ_WRITE_WRITE_LOCKGUARD_5(l) fts::ReadWriteLockWriteLockGuard ftsMacroReadWriteLockWriteLockGuardInstance5(l);
#define FTS_READ_WRITE_WRITE_LOCKGUARD_6(l) fts::ReadWriteLockWriteLockGuard ftsMacroReadWriteLockWriteLockGuardInstance6(l);
#define FTS_READ_WRITE_WRITE_LOCKGUARD_7(l) fts::ReadWriteLockWriteLockGuard ftsMacroReadWriteLockWriteLockGuardInstance7(l);
#define FTS_READ_WRITE_WRITE_LOCKGUARD_8(l) fts::ReadWriteLockWriteLockGuard ftsMacroReadWriteLockWriteLockGuardInstance8(l);
#define FTS_READ_WRITE_WRITE_LOCKGUARD_9(l) fts::ReadWriteLockWriteLockGuard ftsMacroReadWriteLockWriteLockGuardInstance9(l);

#endif //#ifndef FTS_HPP_HEADER_GUARD