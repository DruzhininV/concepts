#include "AppStateItem.h"
using namespace app;
#include <vector>


struct MyType{};

void dump(MyType value) {
    std::cout << "MyType\n";
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
    using AppStateHistory = boost::circular_buffer<AppStateItem>;

    appState.emplace_back(0);
    appState.emplace_back(1.1);
    appState.emplace_back("2");
    appState.emplace_back(appState);
    appState.emplace_back(std::string{"string"});
    appState.emplace_back(MyType{});

    AppStateHistory history{10};


    dump(appState);


    return 0;
}
