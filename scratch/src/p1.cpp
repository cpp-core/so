// Copyright (C) 2022, 2023 by Mark Melton
//

#include <signal.h>
#include <memory>
#include <thread>
#include <unistd.h>

void end_of_the_world(int) {
    write(1, "Ouch\n", 5);
}
    
int main(int argc, const char *argv[]) {

    struct sigaction sa;
    memset(&sa, 0, sizeof(struct sigaction));
    sigemptyset(&sa.sa_mask);
    sa.sa_handler = end_of_the_world;
    sa.sa_flags = 0;
    
    sigaction(SIGSEGV, &sa, nullptr);
    sigaction(SIGINT, &sa, nullptr);
    sigaction(SIGTERM, &sa, nullptr);
    sigaction(SIGQUIT, &sa, nullptr);

    while (true)
	std::this_thread::sleep_for(std::chrono::seconds{1});
    return 0;
}
