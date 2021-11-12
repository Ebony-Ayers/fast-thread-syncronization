# fast-thread-syncronization
A set of basic locks and other thread syncronization objects desined for high performance and simplicity

##Syncronisation primatives
Lock - a lock is used to restrict access to an area of code, like resources access, to one thread at a time by requireing locking the lock to procede
Semaphore - a semaphore allow a limited number of threads to access an area of code at a time. Once the limit it exhausted threads must wait.
Signal - wait untill another thread signals to continue. In many ways the oposite of a lock
Flag - check if another thread has flagged. A wrapper arround an atomic bool.

##Implementation
Lock, Semaphore, Signal all come in spin and adaptive variants. Spin variants simply loop untill they can continue. Adaptive variants use a call to the kernel to pause the thread. For short wait times spin variants will be faster and for long variants adaptive variants will be faster.
