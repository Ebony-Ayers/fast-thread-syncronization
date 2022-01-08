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
		
		//private:
			std::atomic_int32_t m_numReaders;
			std::atomic_bool m_writeRequest;
	};
}


//=========================================SpinLock=========================================
inline void fts::SpinLock::lock()
{
	while(true)
	{
		if(!this->m_isLocked.exchange(true, std::memory_order_acquire)) [[likely]] break;
		while(this->m_isLocked.load(std::memory_order_relaxed));
	}
}
inline void fts::SpinLock::unlock()
{
	this->m_isLocked.store(false, std::memory_order_release);
}
inline bool fts::SpinLock::try_lock()
{
	return ! this->m_isLocked.exchange(true, std::memory_order_acquire);
}


//=========================================AdaptiveLock========================================
inline void fts::AdaptiveLock::lock()
{
	//platform: linux
	#ifdef FTS_PLATFORM_LINUX
		syscall(SYS_futex, &this->m_address, FUTEX_WAIT_PRIVATE, 1, nullptr);
		this->m_address = 1;
	//platform: windows
	#elif defined(FTS_PLATFORM_WINDOWS)
		auto value = 1;
		WaitOnAddress(reinterpret_cast<void*>(&this->m_address), &value, sizeof(value), INFINITE);
	//platform: unknown
	#elif defined(FTS_PLATFORM_UNKNOWN)
		this->m_mutex.lock();
	#endif
}
inline void fts::AdaptiveLock::unlock()
{
	//platform: linux
	#ifdef FTS_PLATFORM_LINUX
		syscall(SYS_futex, &this->m_address, FUTEX_WAKE_PRIVATE, 1, nullptr);
		this->m_address = 0;
	//platform: windows
	#elif defined(FTS_PLATFORM_WINDOWS)
		this->m_address++;
		WakeByAddressSingle(reinterpret_cast<void*>(&this->m_address));
	//platform: unknown
	#elif defined(FTS_PLATFORM_UNKNOWN)
		this->m_mutex.unlock();
	#endif
}
inline bool fts::AdaptiveLock::try_lock()
{
	//platform: linux
	#ifdef FTS_PLATFORM_LINUX
		if(this->m_address == 0)
		{
			syscall(SYS_futex, &this->m_address, FUTEX_WAIT_PRIVATE, 1, nullptr);
			return true;
		}
		else return false;
	//platform: windows
	#elif defined(FTS_PLATFORM_WINDOWS)
		if (this->m_address == 0)
		{
			auto value = 1;
			WaitOnAddress(reinterpret_cast<void*>(&this->m_address), &value, sizeof(value), INFINITE);
			return true;
		}
		else return false;
	//platform: unknown
	#elif defined(FTS_PLATFORM_UNKNOWN)
		return this->m_mutex.try_lock();
	#endif

	return false;
}


//=========================================SpinSemaphore=========================================
inline void fts::SpinSemaphore::lock()
{
	while(true)
	{
		auto prev = this->m_counter.fetch_sub(1, std::memory_order_acquire);
		if(prev > 0) [[likely]] return;
		else
		{
			this->m_counter.fetch_add(1, std::memory_order_acquire);
			while(this->m_counter.load(std::memory_order_relaxed) < 1);
		}
	}
}
inline void fts::SpinSemaphore::unlock()
{
	this->m_counter.fetch_add(1, std::memory_order_acquire);
}
inline bool fts::SpinSemaphore::try_lock()
{
	while(true)
	{
		auto prev = this->m_counter.fetch_sub(1, std::memory_order_acquire);
		if(prev > 0) [[likely]] return true;
		else
		{
			this->m_counter.fetch_add(1, std::memory_order_acquire);
			return false;
		}
	}
}


//=========================================AdaptiveSemaphore=========================================
inline void fts::AdaptiveSemaphore::lock()
{
	//platform: linux
	#ifdef FTS_PLATFORM_LINUX
		while(true)
		{
			auto prev = this->m_counter.fetch_sub(1, std::memory_order_acquire);
			if(prev > 0) [[likely]] return;
			else
			{
				this->m_counter.fetch_add(1, std::memory_order_acquire);
				syscall(SYS_futex, &this->m_address, FUTEX_WAIT_PRIVATE, 0, nullptr);
			}
		}
	//platform: windows
	#elif defined(FTS_PLATFORM_WINDOWS)
		while (true)
		{
			auto prev = this->m_counter.fetch_sub(1, std::memory_order_acquire);
			if(prev > 0) [[likely]] return;
			else
			{
				this->m_counter.fetch_add(1, std::memory_order_acquire);
				auto value = 1;
				WaitOnAddress(reinterpret_cast<void*>(&this->m_address), &value, sizeof(value), INFINITE);
			}
		}
	//platform: unknown
	#elif defined(FTS_PLATFORM_UNKNOWN)
		while(true)
		{
			auto prev = this->m_counter.fetch_sub(1, std::memory_order_acquire);
			if(prev > 0) [[likely]] return;
			else
			{
				this->m_counter.fetch_add(1, std::memory_order_acquire);
				this->m_mutex.lock();
			}
		}
	#endif
}
inline void fts::AdaptiveSemaphore::unlock()
{
	//platform: linux
	#ifdef FTS_PLATFORM_LINUX
		this->m_counter.fetch_add(1, std::memory_order_acquire);
		syscall(SYS_futex, &this->m_address, FUTEX_WAKE_PRIVATE, 1, nullptr);
	//platform: windows
	#elif defined(FTS_PLATFORM_WINDOWS)
		this->m_counter.fetch_add(1, std::memory_order_acquire);
		WakeByAddressSingle(reinterpret_cast<void*>(&m_address));
	//platform: unknown
	#elif defined(FTS_PLATFORM_UNKNOWN)
		this->m_counter.fetch_add(1, std::memory_order_acquire);
		this->m_mutex.unlock();
	#endif
}
inline bool fts::AdaptiveSemaphore::try_lock()
{
	//platform: linux
	#ifdef FTS_PLATFORM_LINUX
		while(true)
		{
			auto prev = this->m_counter.fetch_sub(1, std::memory_order_acquire);
			if(prev > 0) [[likely]] return true;
			else
			{
				this->m_counter.fetch_add(1, std::memory_order_acquire);
				return false;
			}
		}
	//platform: windows
	#elif defined(FTS_PLATFORM_WINDOWS)
		while (true)
		{
			auto prev = this->m_counter.fetch_sub(1, std::memory_order_acquire);
			if (prev > 0) [[likely]] return true;
			else
			{
				this->m_counter.fetch_add(1, std::memory_order_acquire);
				return false;
			}
		}
	//platform: unknown
	#elif defined(FTS_PLATFORM_UNKNOWN)
		while(true)
		{
			auto prev = this->m_counter.fetch_sub(1, std::memory_order_acquire);
			if(prev > 0) [[likely]] return true;
			else
			{
				this->m_counter.fetch_add(1, std::memory_order_acquire);
				return false;
			}
		}
	#endif

	return false;
}


//=========================================Signal=========================================
inline void fts::Signal::wait()
{
	//platform: linux
	#ifdef FTS_PLATFORM_LINUX
		syscall(SYS_futex, &this->m_address, FUTEX_WAIT_PRIVATE, 0, nullptr);
	//platform: windows
	#elif defined(FTS_PLATFORM_WINDOWS)
		auto value = 1;
		WaitOnAddress(reinterpret_cast<void*>(&this->m_address), &value, sizeof(value), INFINITE);
	//platform: unknown
	#elif defined(FTS_PLATFORM_UNKNOWN)
		this->m_numWaiting.fetch_add(1);
		this->m_mutex.lock();
		this->m_unlocked.store(true);
		this->m_mutex.unlock();
	#endif
}
inline void fts::Signal::wake()
{
	//platform: linux
	#ifdef FTS_PLATFORM_LINUX
		syscall(SYS_futex, &this->m_address, FUTEX_WAKE_PRIVATE, 1, nullptr);
	//platform: windows
	#elif defined(FTS_PLATFORM_WINDOWS)
		WakeByAddressSingle(reinterpret_cast<void*>(&this->m_address));
	//platform: unknown
	#elif defined(FTS_PLATFORM_UNKNOWN)
		this->m_unlocked.store(false);
		this->m_mutex.unlock();
		while(!this->m_unlocked.load());
		this->m_mutex.lock();
		this->m_numWaiting.fetch_sub(1);
	#endif
}
inline void fts::Signal::wakeAll()
{
	//platform: linux
	#ifdef FTS_PLATFORM_LINUX
		syscall(SYS_futex, &this->m_address, FUTEX_WAKE_PRIVATE, std::numeric_limits<int>::max(), nullptr);
	//platform: windows
	#elif defined(FTS_PLATFORM_WINDOWS)
		WakeByAddressAll(reinterpret_cast<void*>(&this->m_address));
	//platform: unknown
	#elif defined(FTS_PLATFORM_UNKNOWN)
		for(int32_t i = this->m_numWaiting.load(); i > 0; i--)
		{
			this->m_unlocked.store(false);
			this->m_mutex.unlock();
			while(!m_unlocked.load());
			this->m_mutex.lock();
		}
		this->m_numWaiting.store(0);
	#endif
}


//=========================================SpinSignal=========================================
inline void fts::SpinSignal::wait()
{
	this->m_isWaiting.store(2, std::memory_order_release);
	while(true)
	{
		if(this->m_isWaiting.load(std::memory_order_relaxed) == 1)
		{
			this->m_isWaiting.fetch_add(1, std::memory_order_acquire);
			return;
		}
		else if(this->m_isWaiting.load(std::memory_order_relaxed) == 0)
		{
			return;
		}
	}
}
inline void fts::SpinSignal::wake()
{
	this->m_isWaiting.store(1, std::memory_order_release);
}
inline void fts::SpinSignal::wakeAll()
{
	this->m_isWaiting.store(0, std::memory_order_release);
}



//=========================================flag=========================================
inline void fts::Flag::raise()
{
	this->m_isRaised.store(true);
}
inline void fts::Flag::lower()
{
	this->m_isRaised.store(false);
}
inline bool fts::Flag::isRaised()
{
	return this->m_isRaised.load();
}


//=========================================ReadWriteLock=========================================

inline void fts::ReadWriteLock::readLock()
{
	while(true)
	{
		if(!this->m_writeRequest.load(std::memory_order_relaxed)) [[likely]] break;
		while(this->m_writeRequest.load(std::memory_order_relaxed));
	}
	this->m_numReaders.fetch_add(1, std::memory_order_acquire);
}
inline void fts::ReadWriteLock::writeLock()
{
	while(true)
	{
		if((!this->m_writeRequest.exchange(true, std::memory_order_acquire)) && (this->m_numReaders.load(std::memory_order_relaxed) == 0)) [[likely]] break;
		while(this->m_writeRequest.load(std::memory_order_relaxed));
		while(this->m_numReaders.load(std::memory_order_relaxed) != 0);
	}
}
inline void fts::ReadWriteLock::readUnlock()
{
	this->m_numReaders.fetch_sub(1, std::memory_order_acquire);
}
inline void fts::ReadWriteLock::writeUnlock()
{
	this->m_writeRequest.store(false, std::memory_order_release);
}
inline bool fts::ReadWriteLock::readTryLock()
{
	if(!this->m_writeRequest.load(std::memory_order_relaxed)) [[likely]]
	{
		this->m_numReaders.fetch_add(1, std::memory_order_acquire);
		return true;
	}
	else
	{
		return false;
	}
}
inline bool fts::ReadWriteLock::writeTryLock()
{
	if((!this->m_writeRequest.exchange(true, std::memory_order_acquire)) && (this->m_numReaders.load(std::memory_order_relaxed) == 0)) [[likely]]
	{
		return true;
	}
	else
	{
		return false;
	}
}