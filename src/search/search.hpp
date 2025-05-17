// #pragma once

// #include <vector>
// #include <chrono>

// struct PieceMove;

// using namespace std;

// struct Solution {
//     vector<PieceMove> moves;
//     chrono::steady_clock::time_point duration;
//     int node;
//     bool is_solved;
// };

// struct Search {
//     virtual Solution search();
// };

// src/search/search.hpp

#ifndef SEARCH_HPP
#define SEARCH_HPP

#include <vector>
#include <chrono>
#include "../main_scene/main_scene.hpp"

struct Solution {
    vector<PieceMove> moves;
    chrono::duration<double, milli> duration;
    int node;
    bool is_solved;
    Solution() : node(0), is_solved(false) {}
};

class Search {
public:
    Search() = default;
    virtual ~Search() = default;

    virtual Solution search() = 0;
};

#endif