#include "../../src/fts.hpp"
#include <chrono>
#include <thread>

void foo(fts::Signal* signal)
{
	std::cout << "thread start" << std::endl;
	std::this_thread::sleep_for(std::chrono::milliseconds(2000));
	std::cout << "thread end" << std::endl;
	signal->wake();
}

int main(int /*argc*/, const char** /*argv*/)
{
	fts::ReadWriteLock wrl;

	wrl.readLock();
	std::cout << "1" << std::endl;
	wrl.readLock();
	std::cout << "2" << std::endl;
	wrl.readLock();
	std::cout << "3" << std::endl;

	wrl.readUnlock();
	wrl.readUnlock();
	wrl.readUnlock();

	wrl.writeLock();
	wrl.writeUnlock();
	wrl.writeLock();
	wrl.writeUnlock();

	return 0;
}
