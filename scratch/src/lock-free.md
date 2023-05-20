
Lock-Free
---------

The term lock-free typically refers to data structures that allow
concurrent access without using a mutual exclusion mechanism for
synchronization. The more [technical
definition](https://en.wikipedia.org/wiki/Non-blocking_algorithm#Lock-freedom)
is a data structure that guaratees that at least one thread will make
progress. This contrasts with
[wait-free](https://en.wikipedia.org/wiki/Non-blocking_algorithm#Wait-freedom)
(all threads make progress) and
[obstruction-free](https://en.wikipedia.org/wiki/Non-blocking_algorithm#Obstruction-freedom)
(at least one thread will make progress if executed in isolation).

`std::atomic`
-------------
In modern `C++` lock-free data structures are implmented using
[`std::atomic`](https://en.cppreference.com/w/cpp/atomic/atomic)
variables. In the `C++` [memory
model](https://en.cppreference.com/w/cpp/language/memory_model), each
atomic operation is indivisible with respect to any other atomic
operation for the same object (even from a different thread) making them
free of data races.

A good rule-of-thumb for reading code is that operations on atomic
variables will occurr in the same order they appear in the source code
and other non-atomic reads and writes cannot be reordered across the
atomic operations boundaries.

Lock-Free, Single Producer, Single Consumer Queue
-------------------------------------------------
A lock-free, single producer, single consumer queue can be implemented
with a pair of atomic pointers we will call `head` (the next write
location) and `tail` (the current read location).
