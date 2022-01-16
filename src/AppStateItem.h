#pragma once

#include <concepts>
#include <memory>
#include <vector>
#include <array>
#include <list>
#include <string>
#include <ranges>
#include <fmt/core.h>
#include <boost/circular_buffer.hpp>

#include <iostream>
#include <iomanip>


namespace app {

template <typename T>
concept Streamable = requires(T value, std::ostream out) {
    { out << value } -> std::convertible_to<std::ostream&>;
};

void dump(Streamable auto const& value) {
    std::cout << value << "\n";
}

template<typename T>
concept AppState =
    std::copyable<T>
    and requires (T value) {
        dump(value);
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

    template<typename T, typename DumpStrategy>
    requires std::invocable<DumpStrategy, T>
    AppStateItem(T state, DumpStrategy strategy)
        : model_{ std::make_unique<CustomizableModel<T, DumpStrategy>>(std::move(state), std::move(strategy))} {}

    friend void dump(AppStateItem const& val /*, Args*/) {
        val.model_->dump_(/*Args*/);
    }

private:

    struct AppStateConcept;
    using Concept = std::unique_ptr<AppStateConcept>;

    struct AppStateConcept {
        virtual ~AppStateConcept() = default;
        virtual Concept copy() const = 0;
        virtual void dump_(/*Args*/) = 0;
    };

    template<AppState T>
    struct AppStateModel final: AppStateConcept {
        explicit AppStateModel(T state) : data_{ std::move(state) } {}
        ~AppStateModel() final = default;

        [[nodiscard]] Concept copy() const final {
            return std::make_unique<AppStateModel>(*this);
        }

        void dump_(/*Args*/) final {
            dump(data_ /*, Args*/);
        }

        T data_;
    };

    template<AppState T, typename DumpStrategy>
    requires std::invocable<DumpStrategy, T>
    struct CustomizableModel final : AppStateConcept {
        CustomizableModel(T state, DumpStrategy strategy)
        : data_{ std::move(state) }, dumpStrategy_{ std::move(strategy) } {}
        ~CustomizableModel() final = default;

        [[nodiscard]] Concept copy() const final {
            return std::make_unique<CustomizableModel>(*this);
        }

        void dump_() final {
            dumpStrategy_(data_);
        }

        T data_;
        DumpStrategy dumpStrategy_;
    };

    Concept model_;
};

static_assert(std::movable<AppStateItem>);
static_assert(std::copyable<AppStateItem>);
static_assert(AppState<AppStateItem>);

template<typename T>
concept StdLikeContainer = std::copyable<T>
                           and requires (T container) {
    typename T::value_type;
//        typename T::allocator_type;
    typename T::size_type;
    typename T::difference_type;
    typename T::reference;
    typename T::const_reference;
    typename T::pointer;
    typename T::const_pointer;
    typename T::iterator;
    typename T::const_pointer;
    typename T::reverse_iterator;
    typename T::const_reverse_iterator;

    { std::begin(container) } -> std::same_as<typename T::iterator>;
    { std::cbegin(container) } -> std::same_as<typename T::const_iterator>;
    { std::end(container) } -> std::same_as<typename T::iterator>;
    { std::cend(container) } -> std::same_as<typename T::const_iterator>;
    { std::size(container) } noexcept -> std::same_as<typename T::size_type>;
    { container.empty() } noexcept -> std::same_as<bool>;
    { container.front() } -> std::same_as<typename T::reference>;
    { container.back() } -> std::same_as<typename T::reference>;
    requires requires (T const& constContainer) {
       { constContainer.front() } -> std::same_as<typename T::const_reference>;
       { constContainer.back() } -> std::same_as<typename T::const_reference>;
   };
};

static_assert(StdLikeContainer<std::vector<int>>);
static_assert(StdLikeContainer<std::list<int>>);
static_assert(StdLikeContainer<std::array<int,1>>);
static_assert(StdLikeContainer<boost::circular_buffer<int>>);

template<typename T>
concept AppStateContainer = StdLikeContainer<T>
                            and std::same_as<AppStateItem, typename T::value_type>
                            and AppState<typename T::value_type>;

void dump(AppStateContainer auto container) {
    std::cout << "Container begin\n";
    std::ranges::for_each(container, [&](AppState auto const& state) {
        dump(state);
    });
    std::cout << "Container end\n";
}


static_assert(AppStateContainer<std::vector<AppStateItem>>);
static_assert(not AppStateContainer<std::string>);

} //namespace app;