#include <iostream>
#include <vector>
#include <string>
#include <unordered_set>
#include <algorithm>

using namespace std;

class SudokuSolver {
public:
    vector<char> board;

    SudokuSolver(const string& board_string) {
        board = vector<char>(board_string.begin(), board_string.end());
    }

    string solve() {
        if (board_is_invalid()) {
            return "";
        }
        return recursive_solve();
    }

    string recursive_solve() {
        if (board_is_solved()) {
            return string(board.begin(), board.end());  // Join board into single string
        }

        pair<int, vector<char>> result = get_next_cell_and_possibilities();
        int index = result.first;
        vector<char> possibilities = result.second;

        if (index == -1) {
            return "";  // No solution
        }

        for (char num : possibilities) {
            board[index] = num;
            string solved_board = recursive_solve();
            if (!solved_board.empty()) {
                return solved_board;
            }
        }

        board[index] = '-';  // Reset if no solution found
        return "";
    }

    bool board_is_invalid() {
        return !board_is_valid();
    }

    bool board_is_valid() {
        return all_rows_valid() && all_columns_valid() && all_boxes_valid();
    }

    bool board_is_solved() {
        return find(board.begin(), board.end(), '-') == board.end();
    }

    pair<int, vector<char>> get_next_cell_and_possibilities() {
        for (int i = 0; i < board.size(); ++i) {
            if (board[i] == '-') {
                vector<char> existing_values = get_all_intersections(i);
                vector<char> possibilities;

                for (int n = 1; n <= 9; ++n) {
                    char digit = '0' + n;
                    if (find(existing_values.begin(), existing_values.end(), digit) == existing_values.end()) {
                        possibilities.push_back(digit);
                    }
                }
                return make_pair(i, possibilities);
            }
        }
        return make_pair(-1, vector<char>());  // No empty cell found
    }

    vector<char> get_all_intersections(int i) {
        vector<char> result = get_row(i);
        vector<char> col = get_column(i);
        vector<char> box = get_box(i);

        result.insert(result.end(), col.begin(), col.end());
        result.insert(result.end(), box.begin(), box.end());
        return result;
    }

    vector<char> get_row(int i) {
        int row_start = (i / 9) * 9;
        return vector<char>(board.begin() + row_start, board.begin() + row_start + 9);
    }

    vector<char> get_column(int i) {
        int col_index = i % 9;
        vector<char> column;
        for (int j = col_index; j < 81; j += 9) {
            column.push_back(board[j]);
        }
        return column;
    }

    vector<char> get_box(int i) {
        int box_col = (i % 9) / 3;
        int box_row = i / 27;
        int start_index = box_col * 3 + box_row * 27;

        vector<int> offsets = {0, 1, 2, 9, 10, 11, 18, 19, 20};
        vector<char> box;
        for (int offset : offsets) {
            box.push_back(board[start_index + offset]);
        }
        return box;
    }

    bool collection_is_valid(const vector<char>& collection) {
        unordered_set<char> seen;
        for (char num : collection) {
            if (num == '-') continue;
            if (seen.count(num)) return false;
            seen.insert(num);
        }
        return true;
    }

    bool all_rows_valid() {
        for (int i = 0; i < 9; ++i) {
            if (!collection_is_valid(get_row(i * 9))) {
                return false;
            }
        }
        return true;
    }

    bool all_columns_valid() {
        for (int i = 0; i < 9; ++i) {
            if (!collection_is_valid(get_column(i))) {
                return false;
            }
        }
        return true;
    }

    bool all_boxes_valid() {
        vector<int> box_indices = {0, 3, 6, 27, 30, 33, 54, 57, 60};
        for (int idx : box_indices) {
            if (!collection_is_valid(get_box(idx))) {
                return false;
            }
        }
        return true;
    }
};

// Function to format and print board string
void print_board(const string& board) {
    for (int i = 0; i < 9; ++i) {
        if (i % 3 == 0 && i != 0) {
            cout << "---------------------" << endl;
        }

        string row = board.substr(i * 9, 9);
        cout << row[0] << " " << row[1] << " " << row[2] << " | "
             << row[3] << " " << row[4] << " " << row[5] << " | "
             << row[6] << " " << row[7] << " " << row[8] << endl;
    }
}
