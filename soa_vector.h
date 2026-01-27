#pragma once
#include <memory>
#include <tuple>

/// A structure-of-arrays vector.
template<typename KeyEnum, typename ...Ts>
class soa_vector {
    size_t capacity_{};
    size_t size_{};
    std::tuple<Ts*...> pointers_ {};

    template<typename T>
    static void resize_in_place(T*& pointer, const size_t new_capacity) {
        pointer = static_cast<T*>(std::realloc(static_cast<void*>(pointer), new_capacity * sizeof(T)));
    }

    void resize_to_capacity() {
        std::apply([&](auto& ...x){(resize_in_place(x, capacity_), ...);} , pointers_);
    }

public:
    soa_vector() = default;
    soa_vector(const soa_vector& other) = delete;
    soa_vector(soa_vector&& other) noexcept {
        capacity_ = other.capacity_;
        size_ = other.size_;
        pointers_ = std::move(other.pointers_);
        other.capacity_ = 0;
        other.size_ = 0;
        std::apply([&](auto ...x){((x = nullptr), ...);}, other.pointers_);
    }

    /// Ensure the vector has space for at least `new_capacity` elements.
    void reserve(const size_t new_capacity) {
        capacity_ = std::max(capacity_, new_capacity);
        resize_to_capacity();
    }

    /// Append a new set of values to the end of the vector.
    size_t emplace_back(const Ts... values) {
        if (size_ >= capacity_) {
            capacity_ = (capacity_ + 1) * 3 / 2;
            resize_to_capacity();
        }

        const size_t idx = size_;
        size_++;

        std::apply([&](auto ...x){((x[idx] = values), ...);}, pointers_);

        return idx;
    }

    /// Get the current number of items in the vector.
    [[nodiscard]] size_t size() const { return size_; }

    /// Get the underlying capacity of the vector.
    [[nodiscard]] size_t capacity() const { return capacity_; }

    /// Check if the vector is empty.
    [[nodiscard]] bool empty() const { return size_ == 0; }

    /// Access the item at a particular index and key, mutable.
    template<KeyEnum Key>
    [[nodiscard]] auto& at(const size_t index) {
        return std::get<static_cast<size_t>(Key)>(pointers_)[index];
    }

    /// Access the item at a particular index and key, immutable.
    template<KeyEnum Key>
    [[nodiscard]] const auto& at(const size_t index) const {
        return std::get<static_cast<size_t>(Key)>(pointers_)[index];
    }

    /// Reset the size of the multi_vector to zero, without freeing anything.
    void clear() {
        size_ = 0;
    }

    ~soa_vector() {
        std::apply([](auto ...ptr){(std::free(ptr), ...);} , pointers_);
    }
};
