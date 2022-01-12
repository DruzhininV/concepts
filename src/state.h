#pragma once

#include <concepts>
#include <memory>
//#include <any>
//#include <functional>
#include <fmt/core.h>

#include <iostream>

template<typename T>
concept State = std::regular<T>;
//concept State = true;

template<typename T>
concept Number = std::integral<T> or std::floating_point<T>;

template<typename T>
concept String = requires (T str) {
    std::string_view{str};
};
static_assert(String<std::string>);
static_assert(String<std::string_view>);
static_assert(String<char*>);
static_assert(not String<char>);

//void dump(Number auto val, std::ostream& out) {
//    out << fmt::format("Number = {}\n", val);
//}

void dump(String auto val, std::ostream& out) {
    out << fmt::format("String = \"{}\"\n", val);
}

void dump(auto const& val, std::ostream& out) {
    out << fmt::format("{} = {}\n", typeid(val).name(), val);
}

class Object {
    public:

    ~Object() = default;

    template<State T>
    explicit Object(T val)
    : value_{std::make_unique<Model<T>>(std::move(val))}
    {}

    friend void dump(Object const& val, std::ostream& out) {
        val.value_->dump_(out);
    }

    private:

    struct ConceptInterface;
    using Concept = std::unique_ptr<ConceptInterface>;

    struct ConceptInterface {
        virtual ~ConceptInterface() = default;
        virtual Concept copy() const = 0;
        virtual void dump_(std::ostream&) = 0;
    };

    template <State T>
    struct Model final : ConceptInterface {
        explicit Model(T value) : data{std::move(value)} {}
        ~Model() final = default;
        [[nodiscard]] Concept copy() const final {
            return std::make_unique<Model>(*this);
        }
        void dump_(std::ostream& stream) final {
            dump(data, stream);
        }

        T data;
    };

    std::unique_ptr<ConceptInterface> value_;
};

//static_assert(State<Object>);