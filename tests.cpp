#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators_all.hpp>
#include <iostream>
#include <random>

#include "soa_vector.h"

TEST_CASE("Tree", "[tests]") {
    enum class cols { parent, value, sum_with_parents };
    eam::soa_vector<cols, int, int, int> tree;

    REQUIRE(tree.empty());

    tree.emplace_back(0, 2, 0);
    REQUIRE(!tree.empty());
    REQUIRE(tree.size() == 1);
    REQUIRE(tree.capacity() >= 1);

    tree.emplace_back(0, 6, 0);
    REQUIRE(tree.size() == 2);
    REQUIRE(tree.capacity() >= 2);
    REQUIRE(tree.at<cols::parent>(0) == 0);
    REQUIRE(tree.at<cols::parent>(1) == 0);
    REQUIRE(tree.at<cols::value>(0) == 2);
    REQUIRE(tree.at<cols::value>(1) == 6);
    REQUIRE(tree.at<cols::sum_with_parents>(0) == 0);
    REQUIRE(tree.at<cols::sum_with_parents>(1) == 0);

    // Compute a simple formula for every entry:
    // sum(i) = sum(parent(i)) + value(i)
    // sum(0) = value(0)

    tree.at<cols::sum_with_parents>(0) = tree.at<cols::value>(0);

    for (size_t i = 1; i < tree.size(); ++i) {
        const auto parent = tree.at<cols::parent>(i);
        const auto parent_sum = tree.at<cols::sum_with_parents>(parent);
        const auto value = tree.at<cols::value>(i) + parent_sum;
        tree.at<cols::sum_with_parents>(i) = value;
    }

    REQUIRE(tree.at<cols::sum_with_parents>(0) == 2);
    REQUIRE(tree.at<cols::sum_with_parents>(1) == 8);
}

TEST_CASE("Growth", "[tests]") {
    eam::soa_vector<int, int> vec;
    for (int i = 0; i < 1024; i++) {
        REQUIRE(vec.size() == i);
        REQUIRE(vec.capacity() >= i);
        vec.emplace_back(i);
    }

    // Validate that all entries were retained correctly.
    for (int i = 0; i < 1024; i++) {
        REQUIRE(vec.at<0>(i) == i);
    }
}

TEST_CASE("Bench: SoA vs AoS simple iteration with padding", "[!benchmark]") {
    // Generate a seed for populating our big list.
    const auto random_seed = GENERATE(take(10, random(0, 9999)));
    std::mt19937 gen(random_seed);

    struct person {
        const char *name;
        int age;
        int importance;
    };

    enum class field { name, age, importance };

    static constexpr std::array names {
        "Garry",
        "Joshua",
        "Steven"
    };

    eam::soa_vector<field, const char*, int, int> soa;
    std::vector<person> aos;

    // Populate both!
    for (int i = 0; i < 1000000; i++) {
        const auto age = std::uniform_int_distribution{1, 80}(gen);
        const auto importance = std::uniform_int_distribution{1, 5}(gen);
        const auto name_idx = std::uniform_int_distribution<>{0, names.size() - 1}(gen);
        soa.emplace_back(names[name_idx], age, importance);
        aos.emplace_back(names[name_idx], age, importance);
    }

    long total1{}, total2{};

    BENCHMARK("Non-SoA") {
        long sum1 = 0;
        for (int i = 0; i < aos.size(); i++) {
            sum1 += aos[i].age * aos[i].importance;
        }
        total1 = sum1;
    };

    BENCHMARK("SoA") {
        long sum2 = 0;
        for (int i = 0; i < soa.size(); i++) {
            sum2 += soa.at<field::age>(i) * soa.at<field::importance>(i);
        }
        total2 = sum2;
    };

    REQUIRE(total1 == total2);
}

TEST_CASE("Bench: SoA vs AoS tree structure", "[!benchmark]") {
    // Generate a seed for populating our big list.
    const auto random_seed = GENERATE(take(10, random(0, 9999)));
    std::mt19937 gen(random_seed);

    struct node {
        unsigned parent;
        unsigned value;
        unsigned sum;
    };

    enum class field { parent, value, sum };


    eam::soa_vector<field, unsigned, unsigned, unsigned> soa;
    std::vector<node> aos;

    // Populate both!
    for (int i = 0; i < 1000000; i++) {
        const unsigned parent = std::uniform_int_distribution{0, i - 1}(gen);
        const unsigned value = std::uniform_int_distribution{0, 2048}(gen);
        soa.emplace_back(parent, value, 0);
        aos.emplace_back(parent, value, 0);
    }

    BENCHMARK("Non-SoA") {
        aos[0].sum = aos[0].value;

        for (int i = 1; i < aos.size(); i++) {
            aos[i].sum = aos[aos[i].parent].sum + aos[i].value;
        }
    };

    BENCHMARK("SoA") {
        soa.at<field::sum>(0) = soa.at<field::value>(0);

        for (int i = 1; i < soa.size(); i++) {
            soa.at<field::sum>(i) = soa.at<field::sum>(soa.at<field::parent>(i)) + soa.at<field::value>(i);
        }
    };
}