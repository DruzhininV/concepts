#pragma once

#include <concepts>
#include <memory>
#include <fmt/core.h>

#include <iostream>


namespace app {


template<typename T>
concept Number = std::integral<T> or std::floating_point<T>;

void dump(Number auto val, std::ostream& out) {
    out << fmt::format("Number = {}\n", val);
}

template<typename T> concept AppState =
        std::copyable<T>
        and requires (T value, std::ostream& out) {
            dump(value, out);
};

class AppStateItem {
public:
    ~AppStateItem() = default;
    AppStateItem(AppStateItem&&) noexcept = default;
    AppStateItem& operator=(AppStateItem&&) noexcept = default;
    AppStateItem(AppStateItem const& obj) : model_{ obj.model_->copy() } {}
    AppStateItem& operator=(AppStateItem const& obj) {
        auto tmp{ obj };
        model_ = std::move(tmp.model_);
        return *this;
    }

    template<AppState T> explicit AppStateItem(T state)
        : model_{ std::make_unique<AppStateModel<T>>(std::move(state)) } {}

    friend void dump(AppStateItem const& val, std::ostream& out) {
        val.model_->dump_(out);
    }

private:

    struct AppStateConcept;
    using Concept = std::unique_ptr<AppStateConcept>;

    struct AppStateConcept {
        virtual ~AppStateConcept() = default;
        virtual Concept copy() const = 0;
        virtual void dump_(std::ostream&) = 0;
    };

    template<AppState T>
    struct AppStateModel final: AppStateConcept {
        explicit AppStateModel(T state) : data_{ std::move(state) } {}
        ~AppStateModel() final = default;

        [[nodiscard]] Concept copy() const final {
            return std::make_unique<AppStateModel>(*this);
        }

        void dump_(std::ostream& stream) final {
            dump(data_, stream);
        }

        T data_;
    };

    Concept model_;
};

static_assert(std::movable<AppStateItem>);
static_assert(std::copyable<AppStateItem>);
static_assert(AppState<AppStateItem>);


} //namespace app;