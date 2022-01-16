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

    auto dbDumper{[](AppState auto const& state){
        std::cout << fmt::format("Dump into DataBase {}\n", state);
    }};

    appState.emplace_back(0, dbDumper);
    appState.emplace_back(1.1, dbDumper);
    appState.emplace_back("2", dbDumper);
    appState.emplace_back(appState);
    appState.emplace_back(std::string{"string"}, dbDumper);
    appState.emplace_back(MyType{});
    appState.emplace_back(42, [](AppState auto const& state){
        std::cout << fmt::format("Dump into DataBase: {}\n", state);
    });

    AppStateHistory history{10};


    dump(appState);


    return 0;
}
