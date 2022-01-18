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
concept Streamable = requires(T const& value, std::ostream out) {
    { out << value } -> std::convertible_to<std::ostream&>;
};

void dump(Streamable auto const& value) {
    std::cout << value << "\n";
}

template<typename T>
concept app_state = std::copyable<T>
    and requires (T value) {
        dump(value);
};

struct UniqueStoragePolicy {
    template<typename Concept>
    using storage_type = std::unique_ptr<Concept>;

    template<typename Model>
    static auto copy_model(Model const& model) {
        return std::make_unique<Model>(model);
    }

    template<typename Model, app_state Value, typename... Args>
    requires std::constructible_from<Model, Value, Args...>
    static auto create_model(Value value, Args... args) {
        return std::make_unique<Model>(std::move(value), std::forward<Args...>(args)...);
    }
};

template<typename T>
concept storage_policy = requires(int value) {
    typename T::template storage_type<int>;
    { T::template create_model<int>(value) } -> std::same_as<typename T::template storage_type<int>>;
    { T::template copy_model<int>(value) } -> std::same_as<typename T::template storage_type<int>>;
};

static_assert(storage_policy<UniqueStoragePolicy>);


template<storage_policy StoragePolicy = UniqueStoragePolicy>
class AppStateItem : private StoragePolicy {
public:
    friend void dump(AppStateItem const& val) {
        val.model_->dump_();
    }

    ~AppStateItem() = default;
    AppStateItem(AppStateItem&&) noexcept = default;
    AppStateItem& operator=(AppStateItem&&) noexcept = default;
    AppStateItem(AppStateItem const& obj) : model_{ obj.model_->copy() } {}
    AppStateItem& operator=(AppStateItem const& obj) {
        auto tmp{ obj };
        model_ = std::move(tmp.model_);
        return *this;
    }

    template<app_state T>
    requires (not std::same_as<T, AppStateItem>)
    explicit AppStateItem(T state)
        : model_{ create_model<AppStateModel<T>>(std::move(state)) } {}

    template<typename T, typename DumpStrategy>
    requires std::invocable<DumpStrategy, T> and (not std::same_as<T, AppStateItem>)
    AppStateItem(T state, DumpStrategy strategy)
        : model_{ create_model<CustomizableModel<T, DumpStrategy>>(std::move(state), std::move(strategy))} {}

private:

    struct AppStateConcept;
    using Concept = typename StoragePolicy::template storage_type<AppStateConcept>;

    struct AppStateConcept {
        virtual ~AppStateConcept() = default;
        virtual Concept copy()  const = 0;
        virtual void dump_() const = 0;
    };

    template<typename T>
struct AppStateModel final: AppStateConcept {
        explicit AppStateModel(T state) : data_{ std::move(state) } {}
        ~AppStateModel() final = default;

        [[nodiscard]] Concept copy() const final {
            return copy_model<AppStateModel>(*this);
        }

        void dump_() const final {
            dump(data_);
        }

        T data_;
    };

    template<app_state T, typename DumpStrategy>
    requires std::invocable<DumpStrategy, T>
    struct CustomizableModel final : AppStateConcept {
        CustomizableModel(T state, DumpStrategy strategy)
        : data_{ std::move(state) }, dumpStrategy_{ std::move(strategy) } {}
        ~CustomizableModel() final = default;

        [[nodiscard]] Concept copy() const final {
            return copy_model<CustomizableModel>(*this);
        }

        void dump_() const final {
            dumpStrategy_(data_);
        }

        T data_;
        DumpStrategy dumpStrategy_;
    };

    Concept model_;
};

static_assert(std::movable<AppStateItem<>>);
static_assert(std::copyable<AppStateItem<>>);
static_assert(app_state<AppStateItem<>>);

template<typename T>
concept std_like_container = std::copyable<T>
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

    { container.begin() } -> std::same_as<typename T::iterator>;
    { container.end() } -> std::same_as<typename T::iterator>;
    { container.cbegin() } -> std::same_as<typename T::const_iterator>;
    { container.cend() } -> std::same_as<typename T::const_iterator>;
    { container.size() } noexcept -> std::same_as<typename T::size_type>;
    { container.empty() } noexcept -> std::same_as<bool>;
    { container.front() } -> std::same_as<typename T::reference>;
    { container.back() } -> std::same_as<typename T::reference>;
    requires requires (T const& constContainer) {
        { constContainer.begin() } -> std::same_as<typename T::const_iterator>;
        { constContainer.end() } -> std::same_as<typename T::const_iterator>;
        { constContainer.front() } -> std::same_as<typename T::const_reference>;
        { constContainer.back() } -> std::same_as<typename T::const_reference>;
   };
};

static_assert(std_like_container<std::vector<int>>);
static_assert(std_like_container<std::list<int>>);
static_assert(std_like_container<std::array<int,1>>);
static_assert(std_like_container<boost::circular_buffer<int>>);

template<typename T>
concept app_state_container = std_like_container<T>
                              and std::same_as<AppStateItem<>, typename T::value_type>
                              and app_state<typename T::value_type>;

void dump(app_state_container auto const& container) {
    std::cout << "Container begin\n";
    std::ranges::for_each(container, [&](app_state auto const& state) {
        dump(state);
    });
    std::cout << "Container end\n";
}


//static_assert(app_state_container<std::vector<AppStateItem>>);
//static_assert(not app_state_container<std::string>);

} //namespace app;