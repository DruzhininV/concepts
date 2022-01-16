#pragma once

#include <concepts>
#include <memory>
#include <fmt/core.h>

#include <iostream>


namespace Concept
{


template<typename T>
concept Number = std::integral<T> or std::floating_point<T>;

void dump(Number auto val, std::ostream& out) {
    out << fmt::format("Number = {}\n", val);
}

template<typename T> concept State =
        std::copyable<T>
        and requires (T value, std::ostream& out) {
            dump(value, out);
};

class Object
{
public:

    ~Object() = default;
    Object(Object&&) noexcept = default;
    Object& operator=(Object&&) noexcept = default;
    Object(Object const& obj) : state_{obj.state_->copy() } {}
    Object& operator=(Object const& obj) {
        auto tmp{ obj };
        state_ = std::move(tmp.state_);
        return *this;
    }

    template<State T> explicit Object(T state)
        : state_{std::make_unique<Model < T>>(std::move(state)) } {}

    friend void dump(Object const& val, std::ostream& out) {
        val.state_->dump_(out);
    }

private:

    struct ConceptInterface;
    using Concept = std::unique_ptr<ConceptInterface>;

    struct ConceptInterface {
        virtual ~ConceptInterface() = default;
        virtual Concept copy() const = 0;
        virtual void dump_(std::ostream&) = 0;
    };

    template<State T> struct Model final: ConceptInterface {
        explicit Model(T state) : data_{std::move(state) } {}
        ~Model() final = default;

        [[nodiscard]] Concept copy() const final {
            return std::make_unique<Model>(*this);
        }

        void dump_(std::ostream& stream) final {
            dump(data_, stream);
        }

        T data_;
    };

    Concept state_;
};

static_assert(std::movable<Object>);
static_assert(std::copyable<Object>);
static_assert(State<Object>);


} //namespace Concept;