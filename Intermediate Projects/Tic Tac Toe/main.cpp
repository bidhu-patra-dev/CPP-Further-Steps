#include <iostream>
#include <vector>
#include <limits>
#include <string>

using namespace std;

#define COMPUTER 1
#define HUMAN 2
#define SIDE 3
#define COMPUTERMOVE 'O'
#define HUMANMOVE 'X'

// Show the board
void show_board(vector<vector<char>>& board) {
    for (int i = 0; i < SIDE; ++i) {
        cout << "\t";
        for (int j = 0; j < SIDE; ++j) {
            cout << board[i][j];
            if (j < SIDE - 1)
                cout << " | ";
        }
        cout << endl;
        if (i < SIDE - 1)
            cout << "\t" << string(9, '-') << endl;
    }
}

// Display instructions
void show_instructions() {
    cout << "\nChoose a cell numbered from 1 to 9 as below and play\n";
    cout << "\t 1 | 2 | 3 " << endl;
    cout << "\t-----------" << endl;
    cout << "\t 4 | 5 | 6 " << endl;
    cout << "\t-----------" << endl;
    cout << "\t 7 | 8 | 9\n" << endl;
}

// Initialize board with '*'
vector<vector<char>> initialize() {
    vector<vector<char>> board;
    for (int i = 0; i < SIDE; ++i) {
        vector<char> row(SIDE, '*');
        board.push_back(row);
    }
    return board;
}

// Declare winner
void declare_winner(int whose_turn) {
    if (whose_turn == COMPUTER)
        cout << "COMPUTER has won\n" << endl;
    else
        cout << "HUMAN has won\n" << endl;
}

// Row Check
bool row_crossed(vector<vector<char>>& board) {
    for (int i = 0; i < SIDE; ++i) {
        if (board[i][0] == board[i][1] && board[i][1] == board[i][2] && board[i][0] != '*')
            return true;
    }
    return false;
}

// Column Check
bool column_crossed(vector<vector<char>>& board) {
    for (int i = 0; i < SIDE; ++i) {
        if (board[0][i] == board[1][i] && board[1][i] == board[2][i] && board[0][i] != '*')
            return true;
    }
    return false;
}

// Diagonal Check
bool diagonal_crossed(vector<vector<char>>& board) {
    if (board[0][0] == board[1][1] && board[1][1] == board[2][2] && board[0][0] != '*')
        return true;

    if (board[0][2] == board[1][1] && board[1][1] == board[2][0] && board[0][2] != '*')
        return true;

    return false;
}

// Game state check
bool game_over(vector<vector<char>>& board) {
    return row_crossed(board) || column_crossed(board) || diagonal_crossed(board);
}

// Minimax function
int minimax(vector<vector<char>>& board, int depth, bool is_ai) {
    if (game_over(board)) return is_ai ? -10 : 10;
    
    if (depth == 9) return 0;

    int best_score = is_ai ? -9999 : 9999;

    for (int i = 0; i < SIDE; ++i) {
        for (int j = 0; j < SIDE; ++j) {
            if (board[i][j] == '*') {
                board[i][j] = is_ai ? COMPUTERMOVE : HUMANMOVE;
                int score = minimax(board, depth + 1, !is_ai);
                board[i][j] = '*';
                best_score = is_ai ? max(best_score, score) : min(best_score, score);
            }
        }
    }
    return best_score;
}

// Best move function
pair<int, int> best_move(vector<vector<char>>& board, int move_index) {
    int best_score = -9999;
    int x = -1, y = -1;

    for (int i = 0; i < SIDE; ++i) {
        for (int j = 0; j < SIDE; ++j) {
            if (board[i][j] == '*') {
                board[i][j] = COMPUTERMOVE;
                int score = minimax(board, move_index + 1, false);
                board[i][j] = '*';

                if (score > best_score) {
                    best_score = score;
                    x = i, y = j;
                }
            }
        }
    }

    return {x, y};
}

// Game logic − turn-by-turn play
void play_tic_tac_toe(int whose_turn) {
    vector<vector<char>> board = initialize();
    int move_index = 0;
    show_instructions();

    while (!game_over(board) && move_index < SIDE * SIDE) {
        if (whose_turn == COMPUTER) {
            pair<int, int> move = best_move(board, move_index);
            board[move.first][move.second] = COMPUTERMOVE;
            cout << "COMPUTER has put an " << COMPUTERMOVE
                 << " in cell " << move.first * 3 + move.second + 1 << endl << endl;
            show_board(board);
            whose_turn = HUMAN;
        } else {
            cout << "Available positions: ";
            for (int i = 0; i < SIDE; ++i) {
                for (int j = 0; j < SIDE; ++j) {
                    if (board[i][j] == '*') {
                        cout << (i * 3 + j + 1) << " ";
                    }
                }
            }
            cout << endl;

            cout << "Enter the position: ";
            int n; cin >> n;
            n--; // Adjust to 0-indexed
            int x = n / SIDE;
            int y = n % SIDE;

            if (n >= 0 && n < 9 && board[x][y] == '*') {
                board[x][y] = HUMANMOVE;
                cout << "\nHUMAN has put an " << HUMANMOVE << " in cell " << (n + 1) << endl << endl;
                show_board(board);
                whose_turn = COMPUTER;
            } else {
                cout << "Invalid or occupied position, try again.\n";
                continue;
            }
        }
        move_index++;
    }

    if (!game_over(board) && move_index == SIDE * SIDE) {
        cout << "It's a draw\n";
    } else {
        declare_winner(whose_turn == HUMAN ? COMPUTER : HUMAN);
    }
}

// Main function
int main() {
    cout << "\n-------------------- Tic-Tac-Toe --------------------\n" << endl;

    while (true) {
        cout << "Do you want to start first? (y/n): ";
        string choice;
        cin >> choice;

        if (choice == "y" || choice == "Y") {
            play_tic_tac_toe(HUMAN);
        } else if (choice == "n" || choice == "N") {
            play_tic_tac_toe(COMPUTER);
        } else {
            cout << "Invalid choice\n";
            continue;
        }

        cout << "Do you want to quit? (y/n): ";
        string again;
        cin >> again;
        if (again == "y" || again == "Y") {
            break;
        }
    }

    return 0;
}