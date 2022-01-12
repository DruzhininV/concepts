
#pragma once

#include <memory>
#include <ostream>

template<typename T>
concept Printable = requires(std::ostream& s, T val) {
    s << val;
};

//std::unique_ptr<Printable auto> Make_printable() {
//
//}