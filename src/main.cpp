#include <iostream>
#include <type_traits>
#include "concepts.h"

#include "state.h"
#include <vector>

#include <unordered_map>

#include <fmt/core.h>



//template<typename T>
//concept HasMemberPushBack = requires(T type) {
//    type.push_back(auto);
//};

template<typename T>
concept Int = std::is_integral_v<T>;

template<typename T>
concept MyConcept = true;

void test(Int auto type) {
    std::cout << "Integral type: " << type << std::endl;
}

void test(auto type) {
    std::cout << "none Integral type: " << type << std::endl;
}

template<Int T>
void test2(T type) {
    std::cout << "Integral type: " << type << std::endl;
}

template<typename T>
void test2(T type) {
    std::cout << "none Integral type: " << type << std::endl;
}

void test3(auto type) requires std::is_integral_v<decltype(type)> {
    std::cout << "Integral type: " << type << std::endl;
}

void test3(auto type) {
    std::cout << "none Integral type" << type << std::endl;
}

MyConcept auto initMyConcept() {
    return 0;
}

template<typename T>
requires MyConcept<T>
void func(T type);


//template<typename T>
//concept Number = std::integral<T> or std::floating_point<T>;
//
//void dump(Number auto val, std::ostream& out) {
//    out << fmt::format("Number = {}\n", val);
//}



//template<typename T>
//concept String = requires (T str) {
//    std::string_view{str};
//};

//void dump(auto val, std::ostream& out) {
//    out << fmt::format("String = {}\n", val);
//}

//void dump(int val, std::ostream& out) {
//    out << fmt::format("Number = {}\n", val);
//}


int main()
{
//    std::unique_ptr<Printable auto> ptr = std::make_unique<int>(5);
    test(1);
    test("test");
    test('t');

    test2(1);
    test2("test");
    test2('t');

    test3(1);
    test3("test");
    test3('t');

    struct MyType{};
//    std::unordered_map<MyType, int> m;

    MyConcept auto c = initMyConcept();

    Int auto i = 5;
//    Int auto b = "r";
    std::cout << "Hello, World!" << std::endl;


    Object o1{5};
    Object o2{3.141483647};
    Object o3{"o2"};
    Object o4{std::string("o2")};
//    Object o4{MyType{}};
    dump(o1, std::cout);
    dump(o2, std::cout);
    dump(o3, std::cout);
    dump(o4, std::cout);

    std::vector<Object> vec{};
//    vec.emplace_back(3.141483647);
//    vec.emplace_back("vec[1]");


    return 0;
}
