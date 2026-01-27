#include <catch2/catch_test_macros.hpp>
#include "soa_vector.h"
#include <iostream>

TEST_CASE("Tree", "[tests]") {
    enum class cols { parent, value, sum_with_parents };
    soa_vector<cols, int, int, int> tree;

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
    soa_vector<int, int> vec;
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
