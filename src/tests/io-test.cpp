#include <cmath>
#include <cassert>

#include <iomanip>
#include <iostream>
#include <fstream>
#include <cstring>
#include <stdexcept>

//#include "spdlog/spdlog.h"

#include "io_tools/read_nrg.h"
#include "io_tools/write_nrg.h"

#include "bblock/system.h"

namespace {

static std::vector<bblock::System> systems;

}  // namespace

////////////////////////////////////////////////////////////////////////////////

int main(int argc, char** argv) {
    if (argc != 3) {
        std::cerr << "usage: " << argv[0] << " input.nrg output.nrg" << std::endl;
        return 0;
    }

    try {
        std::ifstream ifs(argv[1]);
        tools::ReadNrg(argv[1], systems);
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    const char* fname = argv[2];
    tools::WriteNrg(fname, systems);

    return 0;
}