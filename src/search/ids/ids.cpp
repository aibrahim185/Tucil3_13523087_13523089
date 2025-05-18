#include "ids.hpp"

// helper
SearchNode dls(SearchNode initial_node, const Board& initial_board, int limit, int& nodes_total, set<string>& visited) {
    stack<SearchNode> s;
    s.push(initial_node);

    while (!s.empty()) {
        SearchNode current_node = s.top();
        s.pop();

        nodes_total++;

        if (current_node.piece_moved != ' ') {
            Coordinates new_position = {-1,-1}; 
            Coordinates original_position = current_node.original_position;

            for(const auto& p_state : current_node.pieces) {
                if (p_state.id == current_node.piece_moved) {
                    new_position = p_state.coordinates;
                    break;
                }
            }
        }

        if (Utils::is_exit(initial_board, current_node.pieces)) {
            return current_node;
        }

        if (current_node.val >= limit) {
            continue;
        }

        vector<SearchNode> next = Utils::generate_next(current_node, [](const Board&, const vector<Piece>&, const SearchNode& parent_node) {
            return parent_node.val + 1;
        });

        reverse(next.begin(), next.end());

        for (const auto& next_node : next) {
            string next_state = Utils::state_to_string(next_node.pieces);
            
            if (visited.find(next_state) == visited.end()) {
                visited.insert(next_state);
                s.push(next_node);
            }
        }
    }
    return SearchNode({}, initial_board, {}, -1);
}

Solution ids::search_ids(const Board& initial_board, const vector<Piece>& initial_pieces) {
    auto time_start = chrono::high_resolution_clock::now();
    Solution result;
    result.node = 0;

    const int MAX_DEPTH_LIMIT = 100000;

    for (int depth_limit = 0; depth_limit <= MAX_DEPTH_LIMIT; ++depth_limit) {        
        SearchNode initial_node(initial_pieces, initial_board, {}, 0);
        set<string> visited_dls;
        
        visited_dls.insert(Utils::state_to_string(initial_node.pieces));

        SearchNode found = dls(initial_node, initial_board, depth_limit, result.node, visited_dls);

        if (found.val != -1) {
            result.is_solved = true;
            result.moves = found.path;
            auto time_end = chrono::high_resolution_clock::now();
            result.duration = chrono::duration<double, milli>(time_end - time_start);
           
            return result;
        }
    }
    auto time_end = chrono::high_resolution_clock::now();
    result.duration = chrono::duration<double, milli>(time_end - time_start);
    result.is_solved = false;

    return result;
}