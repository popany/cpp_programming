#include "common.h"
#include <iostream>
#include <string>
#include <dlfcn.h>

std::vector<B*>& m() {
    static std::vector<B*> _;
    return _;
}

void add_new(B* p) {
    m().push_back(p);
}

void print_all() {
    std::cout << "================================" << std::endl;
    for (const auto& p : m()) {
        std::cout << p->name() << std::endl;
    }
    std::cout << "--------------------------------" << std::endl;
}

class D2 : public B {
public:
    D2() {
        std::cout << "construct: " << name() << std::endl;
        add_new(this);
    }
    ~D2() {
        std::cout << "deconstruct: " << name() << std::endl;
    }

    std::string name() override  final {
        return "D2";
    }
};

static D2 d2;

void load_lib(const std::string& lib_path) {
    void* handle = dlopen (lib_path.c_str(), RTLD_LAZY | RTLD_GLOBAL);
    if (!handle) {
        const std::string error_msg{dlerror()};
        std::cout << "failed to load: " << lib_path << ", error msg: " << error_msg << std::endl;
    }
    std::cout << "lib " << lib_path << " loaded" << std::endl;
}

int main(int argc, char* argv[]) {
    if (argc == 1) {
        std::cout << "must give a so path" << std::endl;
    }

    const std::string lib_path{argv[1]};
    print_all();
    load_lib(lib_path);
    print_all();

    return 0;
}

