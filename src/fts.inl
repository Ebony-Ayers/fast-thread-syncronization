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

namespace fts
{
	//=========================================SpinLock=========================================
	inline void SpinLock::lock()
	{
		while(true)
		{
			if(!this->m_isLocked.exchange(true, std::memory_order_acquire)) [[likely]] break;
			while(this->m_isLocked.load(std::memory_order_relaxed));
		}
	}
	inline void SpinLock::unlock()
	{
		this->m_isLocked.store(false, std::memory_order_release);
	}
	inline bool SpinLock::try_lock()
	{
		return ! this->m_isLocked.exchange(true, std::memory_order_acquire);
	}


	//=========================================AdaptiveLock========================================
	inline void AdaptiveLock::lock()
	{
		//platform: linux
		#ifdef FTS_PLATFORM_LINUX
			syscall(SYS_futex, &this->m_address, FUTEX_WAIT_PRIVATE, 1, nullptr);
			this->m_address = 1;
		//platform: windows
		#elif defined(FTS_PLATFORM_WINDOWS)
			std::cout << "herllo there I am being called from a degenerate operating system" << std::endl;
			auto value = 1;
			WaitOnAddress(reinterpret_cast<void*>(&this->m_address), &value, sizeof(value), INFINITE);
			this->m_address = 1;
		//platform: unknown
		#elif defined(FTS_PLATFORM_UNKNOWN)
			this->m_mutex.lock();
		#endif
	}
	inline void AdaptiveLock::unlock()
	{
		//platform: linux
		#ifdef FTS_PLATFORM_LINUX
			syscall(SYS_futex, &this->m_address, FUTEX_WAKE_PRIVATE, 1, nullptr);
			this->m_address = 0;
		//platform: windows
		#elif defined(FTS_PLATFORM_WINDOWS)
		std::cout << "yeet" << std::endl;
			this->m_address = 0;
			WakeByAddressSingle(reinterpret_cast<void*>(&this->m_address));
		//platform: unknown
		#elif defined(FTS_PLATFORM_UNKNOWN)
			this->m_mutex.unlock();
		#endif
	}
	inline bool AdaptiveLock::try_lock()
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
				this->m_address = 1;
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
	inline void SpinSemaphore::lock()
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
	inline void SpinSemaphore::unlock()
	{
		this->m_counter.fetch_add(1, std::memory_order_acquire);
	}
	inline bool SpinSemaphore::try_lock()
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

	void SpinSemaphore::unlockDestoryCounter()
	{
		return;
	}

	inline void SpinSemaphore::addCounter(int32_t n)
	{
		this->m_counter.fetch_add(n);
	}
	inline void SpinSemaphore::removeCounter(int32_t n)
	{
		this->m_counter.fetch_sub(n);
	}

	inline int32_t SpinSemaphore::numCounters() const
	{
		return this->m_counter.load();
	}


	//=========================================AdaptiveSemaphore=========================================
	inline void AdaptiveSemaphore::lock()
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
	inline void AdaptiveSemaphore::unlock()
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
	inline bool AdaptiveSemaphore::try_lock()
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

	inline void AdaptiveSemaphore::unlockDestoryCounter()
	{
		//platform: linux
		#ifdef FTS_PLATFORM_LINUX
			syscall(SYS_futex, &this->m_address, FUTEX_WAKE_PRIVATE, 1, nullptr);
		//platform: windows
		#elif defined(FTS_PLATFORM_WINDOWS)
			WakeByAddressSingle(reinterpret_cast<void*>(&m_address));
		//platform: unknown
		#elif defined(FTS_PLATFORM_UNKNOWN)
			this->m_mutex.unlock();
		#endif
	}

	inline void AdaptiveSemaphore::addCounter(int32_t n)
	{
		this->m_counter.fetch_add(n);
	}
	inline void AdaptiveSemaphore::removeCounter(int32_t n)
	{
		this->m_counter.fetch_sub(n);
	}

	inline int32_t AdaptiveSemaphore::numCounters() const
	{
		return this->m_counter.load();
	}

	//=========================================Signal=========================================
	inline void Signal::wait()
	{
		//platform: linux
		#ifdef FTS_PLATFORM_LINUX
			this->m_numWaiting.fetch_add(1);
			syscall(SYS_futex, &this->m_address, FUTEX_WAIT_PRIVATE, 0, nullptr);
		//platform: windows
		#elif defined(FTS_PLATFORM_WINDOWS)
			this->m_numWaiting.fetch_add(1);
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
	inline void Signal::wake()
	{
		//platform: linux
		#ifdef FTS_PLATFORM_LINUX
			this->m_numWaiting.fetch_sub(1);
			syscall(SYS_futex, &this->m_address, FUTEX_WAKE_PRIVATE, 1, nullptr);
		//platform: windows
		#elif defined(FTS_PLATFORM_WINDOWS)
			this->m_numWaiting.fetch_sub(1);
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
	inline void Signal::wakeAll()
	{
		//platform: linux
		#ifdef FTS_PLATFORM_LINUX
			this->m_numWaiting.store(0);
			syscall(SYS_futex, &this->m_address, FUTEX_WAKE_PRIVATE, std::numeric_limits<int>::max(), nullptr);
		//platform: windows
		#elif defined(FTS_PLATFORM_WINDOWS)
			this->m_numWaiting.store(0);
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

	inline bool Signal::hasWaitingThread()
	{
		return this->m_numWaiting.load() > 0;
	}


	//=========================================SpinSignal=========================================
	inline void SpinSignal::wait()
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
	inline void SpinSignal::wake()
	{
		this->m_isWaiting.store(1, std::memory_order_release);
	}
	inline void SpinSignal::wakeAll()
	{
		this->m_isWaiting.store(0, std::memory_order_release);
	}

	inline bool SpinSignal::hasWaitingThread()
	{
		return this->m_isWaiting.load() > 0;
	}


	//=========================================flag=========================================
	inline void Flag::raise()
	{
		this->m_isRaised.store(true);
	}
	inline void Flag::lower()
	{
		this->m_isRaised.store(false);
	}
	inline bool Flag::isRaised()
	{
		return this->m_isRaised.load();
	}


	//=========================================ReadWriteLock=========================================

	inline void ReadWriteLock::readLock()
	{
		while(true)
		{
			if(!this->m_writeRequest.load(std::memory_order_relaxed)) [[likely]] break;
			while(this->m_writeRequest.load(std::memory_order_relaxed));
		}
		this->m_numReaders.fetch_add(1, std::memory_order_acquire);
	}
	inline void ReadWriteLock::writeLock()
	{
		while(true)
		{
			if((!this->m_writeRequest.exchange(true, std::memory_order_acquire)) && (this->m_numReaders.load(std::memory_order_relaxed) == 0)) [[likely]] break;
			while(this->m_writeRequest.load(std::memory_order_relaxed));
			while(this->m_numReaders.load(std::memory_order_relaxed) != 0);
		}
	}
	inline void ReadWriteLock::readUnlock()
	{
		this->m_numReaders.fetch_sub(1, std::memory_order_acquire);
	}
	inline void ReadWriteLock::writeUnlock()
	{
		this->m_writeRequest.store(false, std::memory_order_release);
	}
	inline bool ReadWriteLock::readTryLock()
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
	inline bool ReadWriteLock::writeTryLock()
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

	
	template<typename LockT>
	inline GenericLockGuard<LockT>::GenericLockGuard(LockT& lock)
	{
		this->m_lock = &lock;
		this->m_lock->lock();
	}
	template<typename LockT>
	inline GenericLockGuard<LockT>::GenericLockGuard(LockT* lock)
	{
		this->m_lock = lock;
		this->m_lock->lock();
	}
	template<typename LockT>
	inline GenericLockGuard<LockT>::~GenericLockGuard()
	{
		this->m_lock->unlock();
	}

	template<typename SemaphoreT>
	inline SemaphoreDestoryCounterLockGuard<SemaphoreT>::SemaphoreDestoryCounterLockGuard(SemaphoreT& semaphore)
	{
		this->m_semaphore = &semaphore;
		this->m_semaphore->lock();
	}
	template<typename SemaphoreT>
	inline SemaphoreDestoryCounterLockGuard<SemaphoreT>::SemaphoreDestoryCounterLockGuard(SemaphoreT* semaphore)
	{
		this->m_semaphore = semaphore;
		this->m_semaphore->lock();
	}
	template<typename SemaphoreT>
	inline SemaphoreDestoryCounterLockGuard<SemaphoreT>::~SemaphoreDestoryCounterLockGuard()
	{
		this->m_semaphore->unlockDestoryCounter();
	}

	inline ReadWriteLockReadLockGuard::ReadWriteLockReadLockGuard(ReadWriteLock& readWriteLock)
	{
		this->m_readWriteLock = &readWriteLock;
		this->m_readWriteLock->readLock();
	}
	inline ReadWriteLockReadLockGuard::ReadWriteLockReadLockGuard(ReadWriteLock* readWriteLock)
	{
		this->m_readWriteLock = readWriteLock;
		this->m_readWriteLock->readLock();
	}
	inline ReadWriteLockReadLockGuard::~ReadWriteLockReadLockGuard()
	{
		this->m_readWriteLock->readUnlock();
	}

	inline ReadWriteLockWriteLockGuard::ReadWriteLockWriteLockGuard(ReadWriteLock& readWriteLock)
	{
		this->m_readWriteLock = &readWriteLock;
		this->m_readWriteLock->writeLock();
	}
	inline ReadWriteLockWriteLockGuard::ReadWriteLockWriteLockGuard(ReadWriteLock* readWriteLock)
	{
		this->m_readWriteLock = readWriteLock;
		this->m_readWriteLock->writeLock();
	}
	inline ReadWriteLockWriteLockGuard::~ReadWriteLockWriteLockGuard()
	{
		this->m_readWriteLock->writeUnlock();
	}
}