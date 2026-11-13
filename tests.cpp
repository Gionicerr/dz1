#include <cassert>
#include <chrono>
#include <cstddef>
#include <iostream>
#include <utility>

#include "uniq.h"
#include "shared.h"

struct Tracker {
    static int alive;

    int value;

    Tracker() : value(0) {
        ++alive;
    }

    explicit Tracker(int value) : value(value) {
        ++alive;
    }

    ~Tracker() {
        --alive;
    }
};

int Tracker::alive = 0;

struct Base {
    virtual ~Base() = default;
    virtual int value() const = 0;
};

struct Derived : Base {
    int number;

    explicit Derived(int number) : number(number) {}

    int value() const override {
        return number;
    }
};

void test_unique_single_object() {
    {
        UniquePtr<int> pointer(new int(10));

        assert(pointer.get() != nullptr);
        assert(*pointer == 10);

        pointer.reset(new int(20));
        assert(*pointer == 20);

        pointer.reset();
        assert(pointer.get() == nullptr);
    }

    std::cout << "UniquePtr single object: OK\n";
}

void test_unique_move_constructor() {
    {
        UniquePtr<int> first(new int(42));
        UniquePtr<int> second(std::move(first));

        assert(first.get() == nullptr);
        assert(second.get() != nullptr);
        assert(*second == 42);
    }

    std::cout << "UniquePtr move constructor: OK\n";
}

void test_unique_move_assignment() {
    {
        UniquePtr<int> first(new int(1));
        UniquePtr<int> second(new int(2));

        second = std::move(first);

        assert(first.get() == nullptr);
        assert(second.get() != nullptr);
        assert(*second == 1);
    }

    std::cout << "UniquePtr move assignment: OK\n";
}

void test_unique_release() {
    int* raw = nullptr;

    {
        UniquePtr<int> pointer(new int(77));
        raw = pointer.release();

        assert(pointer.get() == nullptr);
        assert(raw != nullptr);
        assert(*raw == 77);
    }

    delete raw;

    std::cout << "UniquePtr release: OK\n";
}

void test_unique_array() {
    {
        UniquePtr<int[]> pointer(new int[3]);

        pointer[0] = 10;
        pointer[1] = 20;
        pointer[2] = 30;

        assert(pointer[0] == 10);
        assert(pointer[1] == 20);
        assert(pointer[2] == 30);

        pointer.reset(new int[2]);
        pointer[0] = 100;
        pointer[1] = 200;

        assert(pointer[0] == 100);
        assert(pointer[1] == 200);
    }

    std::cout << "UniquePtr array: OK\n";
}

void test_unique_array_move() {
    {
        UniquePtr<int[]> first(new int[2]);
        first[0] = 5;
        first[1] = 6;

        UniquePtr<int[]> second(std::move(first));

        assert(first.get() == nullptr);
        assert(second[0] == 5);
        assert(second[1] == 6);
    }

    std::cout << "UniquePtr array move: OK\n";
}

void test_unique_destruction() {
    assert(Tracker::alive == 0);

    {
        UniquePtr<Tracker> pointer(new Tracker(5));
        assert(Tracker::alive == 1);

        pointer.reset();
        assert(Tracker::alive == 0);
    }

    assert(Tracker::alive == 0);

    std::cout << "UniquePtr destruction: OK\n";
}

void test_shared_copying() {
    assert(Tracker::alive == 0);

    {
        SharedPtr<Tracker> first(new Tracker(10));
        assert(Tracker::alive == 1);

        {
            SharedPtr<Tracker> second(first);
            SharedPtr<Tracker> third(second);

            assert(Tracker::alive == 1);
            assert(first->value == 10);
            assert(second->value == 10);
            assert(third->value == 10);
        }

        assert(Tracker::alive == 1);
    }

    assert(Tracker::alive == 0);

    std::cout << "SharedPtr copying: OK\n";
}

void test_shared_assignment() {
    assert(Tracker::alive == 0);

    {
        SharedPtr<Tracker> first(new Tracker(1));
        SharedPtr<Tracker> second(new Tracker(2));

        assert(Tracker::alive == 2);

        second = first;

        // Старый объект второго SharedPtr должен быть удалён.
        assert(Tracker::alive == 1);
        assert(second->value == 1);

        // Самоприсваивание не должно менять владение.
        second = second;
        assert(Tracker::alive == 1);
        assert(second->value == 1);
    }

    assert(Tracker::alive == 0);

    std::cout << "SharedPtr copy assignment: OK\n";
}

void test_shared_reset() {
    assert(Tracker::alive == 0);

    {
        SharedPtr<Tracker> pointer(new Tracker(1));
        assert(Tracker::alive == 1);

        pointer.reset(new Tracker(2));
        assert(Tracker::alive == 1);
        assert(pointer->value == 2);

        pointer.reset();
        assert(pointer.get() == nullptr);
        assert(Tracker::alive == 0);
    }

    assert(Tracker::alive == 0);

    std::cout << "SharedPtr reset: OK\n";
}

void test_shared_same_pointer_reset() {
    assert(Tracker::alive == 0);

    Tracker* raw = new Tracker(42);

    {
        SharedPtr<Tracker> pointer(raw);

        // Не должен удалять объект и менять владение.
        pointer.reset(raw);

        assert(pointer.get() == raw);
        assert(pointer->value == 42);
        assert(Tracker::alive == 1);
    }

    assert(Tracker::alive == 0);

    std::cout << "SharedPtr same-pointer reset: OK\n";
}

void test_shared_move_constructor() {
    {
        SharedPtr<int> first(new int(42));
        SharedPtr<int> second(std::move(first));

        assert(first.get() == nullptr);
        assert(second.get() != nullptr);
        assert(*second == 42);
    }

    std::cout << "SharedPtr move constructor: OK\n";
}

void test_shared_move_assignment() {
    assert(Tracker::alive == 0);

    {
        SharedPtr<Tracker> first(new Tracker(10));
        SharedPtr<Tracker> second(new Tracker(20));

        assert(Tracker::alive == 2);

        second = std::move(first);

        assert(first.get() == nullptr);
        assert(second.get() != nullptr);
        assert(second->value == 10);
        assert(Tracker::alive == 1);
    }

    assert(Tracker::alive == 0);

    std::cout << "SharedPtr move assignment: OK\n";
}

void test_shared_array() {
    assert(Tracker::alive == 0);

    {
        SharedPtr<Tracker[]> pointer(new Tracker[3]);

        assert(Tracker::alive == 3);

        pointer[0].value = 10;
        pointer[1].value = 20;
        pointer[2].value = 30;

        {
            SharedPtr<Tracker[]> copy(pointer);

            assert(copy[0].value == 10);
            assert(copy[1].value == 20);
            assert(copy[2].value == 30);
            assert(Tracker::alive == 3);
        }

        assert(Tracker::alive == 3);
    }

    assert(Tracker::alive == 0);

    std::cout << "SharedPtr array: OK\n";
}

void test_shared_array_move() {
    {
        SharedPtr<int[]> first(new int[3]);
        first[0] = 10;
        first[1] = 20;
        first[2] = 30;

        SharedPtr<int[]> second(std::move(first));

        assert(first.get() == nullptr);
        assert(second[0] == 10);
        assert(second[1] == 20);
        assert(second[2] == 30);
    }

    std::cout << "SharedPtr array move: OK\n";
}

void benchmark_raw(std::size_t count) {
    volatile long long checksum = 0;

    const auto begin = std::chrono::steady_clock::now();

    for (std::size_t i = 0; i < count; ++i) {
        int* pointer = new int(static_cast<int>(i));
        checksum += *pointer;
        delete pointer;
    }

    const auto end = std::chrono::steady_clock::now();
    const auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
        end - begin
    ).count();

    std::cout << "Raw pointer: " << elapsed
              << " ms, checksum = " << checksum << '\n';
}

void benchmark_unique(std::size_t count) {
    volatile long long checksum = 0;

    const auto begin = std::chrono::steady_clock::now();

    for (std::size_t i = 0; i < count; ++i) {
        UniquePtr<int> pointer(new int(static_cast<int>(i)));
        checksum += *pointer;
    }

    const auto end = std::chrono::steady_clock::now();
    const auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
        end - begin
    ).count();

    std::cout << "UniquePtr:    " << elapsed
              << " ms, checksum = " << checksum << '\n';
}

void benchmark_shared(std::size_t count) {
    volatile long long checksum = 0;

    const auto begin = std::chrono::steady_clock::now();

    for (std::size_t i = 0; i < count; ++i) {
        SharedPtr<int> pointer(new int(static_cast<int>(i)));
        checksum += *pointer;
    }

    const auto end = std::chrono::steady_clock::now();
    const auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
        end - begin
    ).count();

    std::cout << "SharedPtr:    " << elapsed
              << " ms, checksum = " << checksum << '\n';
}

void run_benchmarks() {
    const std::size_t counts[] = {1000, 10000, 100000};

    std::cout << "\nBenchmarks:\n";

    for (std::size_t count : counts) {
        std::cout << "Objects: " << count << '\n';
        benchmark_raw(count);
        benchmark_unique(count);
        benchmark_shared(count);
    }
}

int main() {
    test_unique_single_object();
    test_unique_move_constructor();
    test_unique_move_assignment();
    test_unique_release();
    test_unique_array();
    test_unique_array_move();
    test_unique_destruction();

    test_shared_copying();
    test_shared_assignment();
    test_shared_reset();
    test_shared_same_pointer_reset();
    test_shared_move_constructor();
    test_shared_move_assignment();
    test_shared_array();
    test_shared_array_move();

    assert(Tracker::alive == 0);

    run_benchmarks();

    std::cout << "\nAll tests passed\n";
}
