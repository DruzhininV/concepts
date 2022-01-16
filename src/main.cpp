#include "state.h"
using namespace Concept;
#include <vector>


int main()
{

    Object object{42};
    Object copy{object};
    Object movedCopy{std::move(copy)};
    Object copyAssigned = object;
    Object moveAssigned = std::move(object);

    //////////////////////////////////////////////////


    Object o1{5};
    Object o2{3.141483647};
//    Object o3{"o2"};
//    Object o4{std::string("o2")};
//    Object o4{MyType{}};
    dump(o1, std::cout);
    dump(o2, std::cout);
//    dump(o3, std::cout);
//    dump(o4, std::cout);

    std::vector<Object> vec{};
//    vec.emplace_back(3.141483647);
//    vec.emplace_back("vec[1]");


    return 0;
}
