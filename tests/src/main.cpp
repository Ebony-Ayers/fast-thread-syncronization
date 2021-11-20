#include "../../src/fts.hpp"

int main(int /*argc*/, const char** /*argv*/)
{
	fts::SpinLock s;
	
	s.lock();
	s.unlock();
	
	return 0;
}
