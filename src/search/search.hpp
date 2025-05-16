#pragma once

#include <vector>
#include <chrono>

struct PieceMove;

using namespace std;

struct Solution {
    vector<PieceMove> moves;
    chrono::steady_clock::time_point duration;
    int node;
    bool is_solved;
};

struct Search {
    virtual Solution search();
};