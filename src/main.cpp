#include "AppStateItem.h"
using namespace app;
#include <vector>


int main()
{

    AppStateItem object{42};
    AppStateItem copy{object};
    AppStateItem movedCopy{std::move(copy)};
    AppStateItem copyAssigned = object;
    AppStateItem moveAssigned = std::move(object);

    //////////////////////////////////////////////////


    AppStateItem o1{5};
    AppStateItem o2{3.141483647};
//    AppStateItem o3{"o2"};
//    AppStateItem o4{std::string("o2")};
//    AppStateItem o4{MyType{}};
    dump(o1, std::cout);
    dump(o2, std::cout);
//    dump(o3, std::cout);
//    dump(o4, std::cout);

    std::vector<AppStateItem> vec{};
//    vec.emplace_back(3.141483647);
//    vec.emplace_back("vec[1]");


    return 0;
}
