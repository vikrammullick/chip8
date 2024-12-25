#include "interpreter.hpp"

#include <fstream>

using namespace std;

int main(int argc, char *argv[]) {
    if (argc != 2) {
        cerr << "unexpected cli args" << endl;
        return 1;
    }

    vector<char> rom_bytes;

    ifstream rom_ifstream(argv[1], ios::binary);
    if (!rom_ifstream) {
        cerr << "rom not found" << endl;
        return 1;
    } else {
        rom_bytes = vector<char>(istreambuf_iterator<char>(rom_ifstream),
                                 istreambuf_iterator<char>());
        rom_ifstream.close();
    }

    interpreter_t interpreter(rom_bytes);
    interpreter.run();

    return 0;
}
