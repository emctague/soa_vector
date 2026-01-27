#include <catch2/catch_test_macros.hpp>
#include "soa_vector.h"
#include <iostream>

TEST_CASE("Tree", "[tests]") {
    enum class cols { parent, value, sum_with_parents };
    soa_vector<cols, int, int, int> tree;

    tree.emplace_back(0, 2, 0);
    tree.emplace_back(0, 6, 0);

    tree.at<cols::sum_with_parents>(0) = tree.at<cols::value>(0);

    for (size_t i = 1; i < tree.size(); ++i) {
        REQUIRE(tree.size() == 2);

        std::cerr << "ITER" << std::endl;

        const auto parent = tree.at<cols::parent>(i);
        const auto parent_sum = tree.at<cols::sum_with_parents>(parent);

        const auto value = tree.at<cols::value>(i) + parent_sum;
        tree.at<cols::sum_with_parents>(i) = value;
    }

    std::cerr << "DONE ITER" << std::endl;

    REQUIRE(tree.at<cols::sum_with_parents>(0) == 2);
    REQUIRE(tree.at<cols::sum_with_parents>(1) == 8);

    std::cerr << "yayyyy" << std::endl;
}
