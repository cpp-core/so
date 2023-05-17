// Copyright (C) 2022, 2023 by Mark Melton
//

#include <fmt/printf.h>
#include <fmt/format.h>
#include "core/util/tool.h"

struct SafeFile {
    SafeFile(const std::string& filename)
	: fp_(fopen(filename.c_str(), "w"))
	, nuncaught_(std::uncaught_exception()) {
	if (fp_ == nullptr)
	    throw std::runtime_error("Failed to open file");
    }
    
    ~SafeFile() throw() {
	fflush(fp_);
	if (ferror(fp_) and nuncaught_ == std::uncaught_exception())
	    throw std::runtime_error("Failed to flush data");
	fclose(fp_);
    }
    
    FILE *fp_{nullptr};
    int nuncaught_{};
};

int tool_main(int argc, const char *argv[]) {
    try {
	SafeFile fp("/dev/urandom");
    }
    catch ( const std::exception& ex )
    {
	cerr << "There was an exception" << endl;
        return EXIT_FAILURE;
    }
    
    return 0;
}
