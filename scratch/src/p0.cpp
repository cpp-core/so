// Copyright (C) 2022, 2023 by Mark Melton
//

#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <filesystem>
#include <thread>
#include <iostream>
#include <chrono>

class SpinLock {
public:
    void lock() {
	while (locked_.test_and_set(std::memory_order_acquire))
	    std::this_thread::yield();
    }

    void unlock() {
	locked_.clear(std::memory_order_release);
    }
    
private:
    std::atomic_flag locked_ = ATOMIC_FLAG_INIT;
};

class SpinLockGuard {
public:
    SpinLockGuard(SpinLock& lock)
	: lock_(lock) {
	lock_.lock();
    }
    
    ~SpinLockGuard() {
	lock_.unlock();
    }

private:
    SpinLock& lock_;
};

class MemoryFile {
public:
    enum Mode { CREATE, OPEN, CREATE_OR_OPEN };

    struct Header {
	size_t bytes_written{}, bytes_mapped{};
    };

    static constexpr size_t DataOffset = sizeof(Header);
    
    MemoryFile(const std::string& filename, Mode mode) {
	switch (mode) {
	case CREATE:
	    create_chunk(filename);
	    break;
	case OPEN:
	    open_chunk(filename);
	    break;
	case CREATE_OR_OPEN:
	    if (std::filesystem::exists(filename)) open_chunk(filename);
	    else create_chunk(filename);
	    break;
	}
    }
    
    ~MemoryFile() {
	if (base_)
	    munmap(base_, *ptr_bytes_mapped_);
	if (fd_ != -1)
	    close(fd_);
    }

    void commit_message(std::string_view msg) {
	// SpinLockGuard guard(lock_);
	std::lock_guard guard(mutex_);
	ensure_room_for_message(msg.size());
	std::copy(msg.begin(), msg.end(), base_ + *ptr_bytes_written_);
	*ptr_bytes_written_ += msg.size();
    }
    
private:
    size_t grow_file(size_t new_size) {
	auto rc = lseek(fd_, new_size - 1, SEEK_SET);
	if (rc + 1 != new_size)
	    throw std::runtime_error("error: lseek failed trying to grow file");
	
	rc = write(fd_, "", 1);
	if (rc != 1)
	    throw std::runtime_error("error: write to extend file failed");
	
	return new_size;
    }

    void map_memory(size_t nbytes) {
	base_ = (char*)mmap(nullptr, nbytes, PROT_READ | PROT_WRITE, MAP_SHARED, fd_, 0);
	if (base_ == nullptr)
	    throw std::runtime_error("error: failed to map memory");

	Header *header = (Header*)base_;
	ptr_bytes_written_ = &(header->bytes_written);
	ptr_bytes_mapped_ = &(header->bytes_mapped);
	*ptr_bytes_mapped_ = nbytes;
    }
    
    void create_chunk(const std::string& filename) {
	namespace fs = std::filesystem;
	if (fs::exists(fs::path(filename)))
	    throw std::runtime_error("error: chunk already exists");
	
	fd_ = open(filename.c_str(), O_CREAT | O_RDWR, 0777);
	if (fd_ == -1)
	    throw std::runtime_error("error: failed to create chunk");
	
	auto nbytes = grow_file(1024 * 1024);
	map_memory(nbytes);
	*ptr_bytes_written_ = sizeof(Header);
    }
    
    void open_chunk(const std::string& filename) {
	namespace fs = std::filesystem;
	if (not fs::exists(fs::path(filename)))
	    throw std::runtime_error("error: chunk does not exist");
	
	fd_ = open(filename.c_str(), O_RDWR, 0777);
	if (fd_ == -1)
	    throw std::runtime_error("error: failed to open chunk");
	
	auto nbytes = fs::file_size(fs::path(filename));
	map_memory(nbytes);
    }
    
    void ensure_room_for_message(size_t n) {
	auto nwritten = *ptr_bytes_written_, nmapped = *ptr_bytes_mapped_;
	if (nwritten + n >= nmapped) {
	    munmap(base_, nmapped);
	    base_ = nullptr;
	    
	    auto new_size = std::max(2 * nmapped, nwritten + n + 1);
	    auto nbytes = grow_file(new_size);
	    map_memory(nbytes);
	}
    }
    
    // Pointer to the number of bytes written.
    volatile size_t *ptr_bytes_written_{nullptr};

    // Pointer to the number of bytes in the mapped region.
    volatile size_t *ptr_bytes_mapped_{nullptr};
    
    // Pointer to the mapped region.
    char *base_{nullptr};

    // File descriptor of the mapped region.
    int fd_{-1};

    // Use simple spin-lock for thread synchronization.
    // SpinLock lock_;

    // Use a mutex for thread synchronization.
    std::mutex mutex_;
};

template<class Clock = std::chrono::high_resolution_clock>
class StopWatch
{
public:
    StopWatch()
	: start_tp_(Clock::now())
	, last_tp_(start_tp_)
    { }

    auto now() const
    {
	std::atomic_thread_fence(std::memory_order_relaxed);
	auto current_tp = Clock::now();
	std::atomic_thread_fence(std::memory_order_relaxed);
	return current_tp;
    }

    auto mark()
    {
	auto current_tp = now();
	auto elapsed = current_tp - last_tp_;
	last_tp_ = current_tp;
	return elapsed;
    }

    template<class Units = typename Clock::duration>
    auto elapsed_duration()
    {
	auto elapsed = mark();
	return std::chrono::duration_cast<Units>(elapsed);
    }
    
    template<class Units = typename Clock::duration>
    auto elapsed_time()
    {
	auto elapsed = mark();
	return std::chrono::duration_cast<Units>(elapsed).count();
    }
    
private:
    typename Clock::time_point start_tp_;
    typename Clock::time_point last_tp_;
};

int main(int argc, const char *argv[]) {
    const auto filename = "/tmp/x.out";
    size_t nth = argc >= 2 ? atoi(argv[1]) : 1;
    size_t nmsg = argc >= 3 ? atoi(argv[2]) : 1'000'000;
    size_t msg_per_thread = nmsg / nth;

    std::filesystem::remove(std::filesystem::path(filename));
    MemoryFile log(filename, MemoryFile::CREATE);
    
    StopWatch timer;
    timer.mark();
    std::vector<std::thread> threads;
    for (auto i = 0; i < nth; ++i)
	threads.emplace_back([&]() {
	    for (auto i = 0; i < msg_per_thread; ++i)
		log.commit_message("This is a log message\n");
	});

    for (auto& thread : threads)
	thread.join();
    
    auto ms = timer.elapsed_duration<std::chrono::milliseconds>().count();
    std::cout << ms << " ms" << std::endl;
    
    return 0;
}
