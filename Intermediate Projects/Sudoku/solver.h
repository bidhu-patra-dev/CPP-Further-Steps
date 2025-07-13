#ifndef SUDOKU_SOLVER_H
#define SUDOKU_SOLVER_H

#include <string>
#include <vector>
#include <unordered_set>
#include <algorithm>

class SudokuSolver {
public:
    std::vector<char> board;

    SudokuSolver(const std::string& board_string) {
        board = std::vector<char>(board_string.begin(), board_string.end());
    }

    std::string solve() {
        if (board_is_invalid()) {
            return "";
        }
        return recursive_solve();
    }

    virtual std::string recursive_solve() {
        if (board_is_solved()) {
            return std::string(board.begin(), board.end());
        }

        std::pair<int, std::vector<char>> result = get_next_cell_and_possibilities();
        int index = result.first;
        std::vector<char> possibilities = result.second;

        if (index == -1) {
            return "";
        }

        for (char num : possibilities) {
            board[index] = num;
            std::string solved_board = recursive_solve();
            if (!solved_board.empty()) {
                return solved_board;
            }
        }

        board[index] = '-';
        return "";
    }

    virtual std::pair<int, std::vector<char>> get_next_cell_and_possibilities() const {
        for (int i = 0; i < board.size(); ++i) {
            if (board[i] == '-') {
                std::vector<char> existing_values = get_all_intersections(i);
                std::vector<char> possibilities;

                for (int n = 1; n <= 9; ++n) {
                    char digit = '0' + n;
                    if (std::find(existing_values.begin(), existing_values.end(), digit) == existing_values.end()) {
                        possibilities.push_back(digit);
                    }
                }
                return std::make_pair(i, possibilities);
            }
        }
        return std::make_pair(-1, std::vector<char>());
    }

    virtual std::vector<char> get_all_intersections(int i) const {
        std::vector<char> result = get_row(i);
        std::vector<char> col = get_column(i);
        std::vector<char> box = get_box(i);

        result.insert(result.end(), col.begin(), col.end());
        result.insert(result.end(), box.begin(), box.end());
        return result;
    }

    std::vector<char> get_row(int i) const {
        int row_start = (i / 9) * 9;
        return std::vector<char>(board.begin() + row_start, board.begin() + row_start + 9);
    }

    std::vector<char> get_column(int i) const {
        int col_index = i % 9;
        std::vector<char> column;
        for (int j = col_index; j < 81; j += 9) {
            column.push_back(board[j]);
        }
        return column;
    }

    std::vector<char> get_box(int i) const {
        int box_col = (i % 9) / 3;
        int box_row = i / 27;
        int start_index = box_col * 3 + box_row * 27;

        std::vector<int> offsets = {0, 1, 2, 9, 10, 11,18, 19, 20};
        std::vector<char> box;
        for (int offset : offsets) {
            box.push_back(board[start_index + offset]);
        }
        return box;
    }

    bool collection_is_valid(const std::vector<char>& collection) const {
        std::unordered_set<char> seen;
        for (char num : collection) {
            if (num == '-') continue;
            if (seen.count(num)) return false;
            seen.insert(num);
        }
        return true;
    }

    bool all_rows_valid() const {
        for (int i = 0; i < 9; ++i) {
            if (!collection_is_valid(get_row(i * 9))) {
                return false;
            }
        }
        return true;
    }

    bool all_columns_valid() const {
        for (int i = 0; i < 9; ++i) {
            if (!collection_is_valid(get_column(i))) {
                return false;
            }
        }
        return true;
    }

    bool all_boxes_valid() const {
        std::vector<int> box_indices = {0, 3, 6, 27, 30, 33, 54, 57, 60};
        for (int idx : box_indices) {
            if (!collection_is_valid(get_box(idx))) {
                return false;
            }
        }
        return true;
    }

    bool board_is_valid() const {
        return all_rows_valid() && all_columns_valid() && all_boxes_valid();
    }

    bool board_is_invalid() const {
        return !board_is_valid();
    }

    bool board_is_solved() const {
        return std::find(board.begin(), board.end(), '-') == board.end();
    }
};

#endif // SUDOKU_SOLVER_H