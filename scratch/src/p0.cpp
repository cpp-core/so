// Copyright (C) 2022, 2023 by Mark Melton
//

#include <iostream>

struct SafeFile {
    SafeFile(const std::string& filename)
	: fp_(fopen(filename.c_str(), "w"))
	, nuncaught_(std::uncaught_exception()) {
	if (fp_ == nullptr)
	    throw std::runtime_error("Failed to open file");
    }
    
    ~SafeFile() noexcept(false) {
	fflush(fp_);
	if (ferror(fp_) and nuncaught_ == std::uncaught_exception())
	    throw std::runtime_error("Failed to flush data");
	fclose(fp_);
    }

    auto operator*() {
	return fp_;
    }
    
    FILE *fp_{nullptr};
    int nuncaught_{};
};

int main()
{
    try {
	SafeFile fp("/dev/urandom");
        fprintf(*fp, "Hello, world!");
    }
    catch ( const std::exception& ex )
    {
	std::cout << "Caught the exception" << std::endl;
        return EXIT_FAILURE;
    }
}
