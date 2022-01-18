#include "AppStateItem.h"
using namespace app;
#include <vector>
#include <numbers>
#include <thread>
#include <future>


struct MyType{
    MyType() = default;
    ~MyType() = default;
    MyType(MyType const&) {
        std::cout << "MyType copied\n";
    }
    MyType& operator=(MyType const&) {
        std::cout << "MyType assigned\n";
        return *this;
    }
    MyType(MyType&&)  noexcept {
        std::cout << "MyType moved\n";
    }
    MyType& operator=(MyType&&)  noexcept {
        std::cout << "MyType move assigned\n";
        return *this;
    }
};

void dump(MyType const& value) {
    std::cout << "MyType\n";
}

//using State = std::vector<AppStateItem<UniqueStoragePolicy>>;
using State = std::vector<AppStateItem<>>;
using History = boost::circular_buffer<State>;

History createHistory(size_t capacity) {
    History history{capacity};
    history.push_back({});
    return history;
}

void commit(History& history) {
    assert(!history.empty());
    history.push_back(history.back());
}

void undo(History& history) {
    assert(!history.empty());
    history.pop_back();
}

State& current(History& history) {
    assert(!history.empty());
    return history.back();
}

int main()
{
    auto dbDumper{[](app_state auto const& state){
        std::cout << fmt::format("Dump into DataBase {}\n", state);
    }};

    auto history{createHistory(10)};

    current(history).emplace_back(1);
    current(history).emplace_back(std::numbers::pi);
    current(history).emplace_back("string");

    std::cout << fmt::format("{:=^50}\n", "State 1");
    dump(current(history));

    commit(history);

    current(history)[0] = AppStateItem(42);
    current(history)[2] = AppStateItem("new string", dbDumper);
    current(history).emplace_back(current(history));
    current(history).emplace_back(MyType{});

    auto asyncDump{std::async([state = current(history)]{
        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::cout << fmt::format("{:=^50}\n", "Async dump");
        dump(state);
    })};

    std::cout << fmt::format("{:=^50}\n", "State 2");
    dump(current(history));

    std::cout << fmt::format("{:=^50}\n", "Undo");
    undo(history);
    dump(current(history));


    return 0;
}
