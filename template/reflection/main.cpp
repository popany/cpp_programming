#include <iostream>
#include <memory>
#include <string>

enum class DataTypeEnum : int {
   FOO,
   BAR,
   BAZ,
};

class Foo {
public:
   void print_foo() {
       std::cout << "Foo" << std::endl;
   }
};

class Bar {
public:
   void print_bar() {
       std::cout << "Bar" << std::endl;
   }
};

class Baz {
public:
   void print_baz() {
       std::cout << "Baz" << std::endl;
   }
};

void print(std::shared_ptr<Foo> foo) {
   foo->print_foo();
}

void print(std::shared_ptr<Bar> bar) {
   bar->print_bar();
}

void print(std::shared_ptr<Baz> baz) {
   baz->print_baz();
}

template <typename DataType>
void print(DataType*, std::shared_ptr<void> data) {
   print(std::static_pointer_cast<DataType>(data));
}

template <typename Func>
void process_data(const DataTypeEnum type_enum, Func func) {
   switch (type_enum) {
   case DataTypeEnum::FOO: {
           func(static_cast<Foo*>(nullptr));
           break;
       }
   case DataTypeEnum::BAR: {
           func(static_cast<Bar*>(nullptr));
           break;
       }
   case DataTypeEnum::BAZ: {
           func(static_cast<Baz*>(nullptr));
           break;
       }
   }
}

int main() {
   {
       std::shared_ptr<void> data = std::make_shared<Foo>();
       process_data(DataTypeEnum::FOO, [data](auto* p) {
           print(p, data);
       });
   }
   {
       std::shared_ptr<void> data = std::make_shared<Bar>();
       process_data(DataTypeEnum::BAR, [data](auto* p) {
           print(p, data);
       });
   }
   {
       std::shared_ptr<void> data = std::make_shared<Baz>();
       process_data(DataTypeEnum::BAZ, [data](auto* p) {
           print(p, data);
       });
   }

   return 0;
}

