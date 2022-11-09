#include <iostream>
#include <string>
#include <dlfcn.h>

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
    load_lib(lib_path);

    return 0;
}

