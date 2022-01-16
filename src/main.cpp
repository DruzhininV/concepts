#include "AppStateItem.h"
using namespace app;
#include <vector>


struct MyType{};

void dump(MyType value, std::ostream& out) {
    out << "MyType\n";
}


int main()
{
    //////////////////////////////////////////////////

    AppStateItem object{42};
    AppStateItem copy{object};
    AppStateItem movedCopy{std::move(copy)};
    AppStateItem copyAssigned = object;
    AppStateItem moveAssigned = std::move(object);

    //////////////////////////////////////////////////



    std::vector<AppStateItem> appState{};

    appState.emplace_back(0);
    appState.emplace_back(1.1);
    appState.emplace_back("2");
    appState.emplace_back(appState);
    appState.emplace_back(std::string{"string"});
    appState.emplace_back(MyType{});


    dump(appState, std::cout);


    return 0;
}
