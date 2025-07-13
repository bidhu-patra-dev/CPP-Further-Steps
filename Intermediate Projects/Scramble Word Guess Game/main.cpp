#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>
#include <algorithm>

using namespace std;

// Function to load words from file
vector<string> load_words(const string& filename) {
    vector<string> words;
    ifstream file(filename);
    string word;
    while (file >> word) {
        words.push_back(word);
    }
    return words;
}

// Function to scramble a word
string scramble_word(string word) {
    random_shuffle(word.begin(), word.end());
    return word;
}

// Game play logic
string play_game(string original_word, int attempts) {
    string guess;
    cout << "Guess the word (or type 'exit' to quit): ";
    cin >> guess;

    if (guess == "exit") {
        cout << "Thanks for playing! Goodbye!" << endl;
        return guess;
    }

    if (guess == original_word)
        cout << "Correct! You guessed it!" << endl;
    else
        cout << "Wrong guess. Attempts left: " << attempts << endl;

    return guess;
}

int main() {
    srand(time(0));

    vector<string> word_list = load_words("/Users/bidhubhusanpatra/Desktop/CPP Small Projects/Scramble Word Guess Game/words.txt");

    if (word_list.empty()) {
        cout << "Word list is empty or file not found!" << endl;
        return 1;
    }

    while (true) {
        string original_word = word_list[rand() % word_list.size()];
        string scrambled = scramble_word(original_word);

        cout << "\nWelcome to the 5-digit Word Scramble Game!" << endl;
        cout << "Scrambled word: " << scrambled << endl;

        int attempts = 5;

        while (attempts--) {
            string guess = play_game(original_word, attempts);
            if (guess == original_word || guess == "exit") break;
        }

        cout << "The correct word was: " << original_word << "\n" << endl;

        string play_again;
        cout << "Do you want to play again? (yes/no): ";
        cin >> play_again;

        if (play_again != "yes") {
            cout << "Thanks for playing! Goodbye!" << endl;
            break;
        }
    }

    return 0;
}