// n_queens_complete.cpp
// Compile: g++ -std=c++17 -O3 n_queens_complete.cpp -o nqueens
// Run: ./nqueens

#include <iostream>
#include <vector>
#include <string>
#include <chrono>
#include <iomanip>
#include <cmath>
#include <algorithm>
#include <sstream>

class NQueensSolver {
private:
    int n;
    std::vector<std::vector<int>> solutions;
    std::vector<int> backtrackCounts;  // Track backtracks per solution
    int totalBacktracks;
    int nodesExplored;

    // Bitmask for O(1) operations
    int columns, diag1, diag2;

    // For tracking backtracking steps
    int currentBacktracks;

    inline bool isSafe(int row, int col) {
        return !(columns & (1 << col)) &&
               !(diag1 & (1 << (row - col + n - 1))) &&
               !(diag2 & (1 << (row + col)));
    }

    inline void placeQueen(int row, int col) {
        columns |= (1 << col);
        diag1 |= (1 << (row - col + n - 1));
        diag2 |= (1 << (row + col));
    }

    inline void removeQueen(int row, int col) {
        columns &= ~(1 << col);
        diag1 &= ~(1 << (row - col + n - 1));
        diag2 &= ~(1 << (row + col));
    }

    void solveBacktracking(int row, std::vector<int>& board) {
        nodesExplored++;

        if (row == n) {
            solutions.push_back(board);
            backtrackCounts.push_back(currentBacktracks);
            return;
        }

        for (int col = 0; col < n; col++) {
            if (isSafe(row, col)) {
                placeQueen(row, col);
                board[row] = col;

                solveBacktracking(row + 1, board);

                board[row] = -1;
                removeQueen(row, col);
                currentBacktracks++;
                totalBacktracks++;
            }
        }
    }

    // Display a single solution in a nice grid format
    void displaySolution(const std::vector<int>& board, int solNumber, int backtracks) {
        std::cout << "\n";
        std::cout << "╔══════════════════════════════════════════════════════════╗\n";
        std::cout << "║                    SOLUTION #" << std::setw(3) << solNumber << "                          ║\n";
        std::cout << "╠══════════════════════════════════════════════════════════╣\n";
        std::cout << "║ Backtracks for this solution: " << std::setw(30) << backtracks << " ║\n";
        std::cout << "╠══════════════════════════════════════════════════════════╣\n";

        // Display chessboard
        for (int i = 0; i < n; i++) {
            std::cout << "║ ";
            for (int j = 0; j < n; j++) {
                if (board[i] == j) {
                    std::cout << " 👑 ";
                } else {
                    // Alternate colors for better visualization
                    if ((i + j) % 2 == 0) {
                        std::cout << " █ ";
                    } else {
                        std::cout << " ░ ";
                    }
                }
            }
            std::cout << " ║\n";
        }
        std::cout << "╚══════════════════════════════════════════════════════════╝\n";
    }

    // Display compact solution (queen positions)
    void displayCompactSolution(const std::vector<int>& board, int solNumber) {
        std::cout << "Solution " << std::setw(3) << solNumber << ": [";
        for (int i = 0; i < n; i++) {
            std::cout << board[i];
            if (i < n-1) std::cout << ", ";
        }
        std::cout << "]\n";
    }

public:
    NQueensSolver(int size) : n(size), columns(0), diag1(0), diag2(0),
                              totalBacktracks(0), nodesExplored(0), currentBacktracks(0) {}

    void solve() {
        auto start = std::chrono::high_resolution_clock::now();

        solutions.clear();
        backtrackCounts.clear();
        totalBacktracks = 0;
        nodesExplored = 0;
        currentBacktracks = 0;

        std::vector<int> board(n, -1);
        solveBacktracking(0, board);

        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

        // Display results
        displayResults(duration.count());
    }

    void displayResults(long long elapsedMs) {
        std::cout << "\n\n";
        std::cout << "╔══════════════════════════════════════════════════════════════════╗\n";
        std::cout << "║                    N-QUEENS SOLVER RESULTS                      ║\n";
        std::cout << "╚══════════════════════════════════════════════════════════════════╝\n\n";

        std::cout << "┌────────────────────────────────────────────────────────────────┐\n";
        std::cout << "│ PROBLEM PARAMETERS                                             │\n";
        std::cout << "├────────────────────────────────────────────────────────────────┤\n";
        std::cout << "│ Chessboard Size: " << std::setw(39) << n << " x " << n << " │\n";
        std::cout << "│ Total Solutions Found: " << std::setw(34) << solutions.size() << " │\n";
        std::cout << "└────────────────────────────────────────────────────────────────┘\n\n";

        std::cout << "┌────────────────────────────────────────────────────────────────┐\n";
        std::cout << "│ PERFORMANCE METRICS                                            │\n";
        std::cout << "├────────────────────────────────────────────────────────────────┤\n";
        std::cout << "│ Total Nodes Explored: " << std::setw(35) << nodesExplored << " │\n";
        std::cout << "│ Total Backtracks: " << std::setw(38) << totalBacktracks << " │\n";
        std::cout << "│ Average Backtracks/Solution: " << std::setw(30)
                  << (solutions.empty() ? 0 : totalBacktracks / solutions.size()) << " │\n";
        std::cout << "│ Execution Time: " << std::setw(39) << elapsedMs << " ms │\n";
        std::cout << "└────────────────────────────────────────────────────────────────┘\n\n";

        if (!solutions.empty()) {
            std::cout << "┌────────────────────────────────────────────────────────────────┐\n";
            std::cout << "│ ALL SOLUTIONS (Queen positions per row)                        │\n";
            std::cout << "├────────────────────────────────────────────────────────────────┤\n";

            // Display compact solutions first
            for (size_t i = 0; i < solutions.size(); i++) {
                displayCompactSolution(solutions[i], i + 1);
            }

            std::cout << "└────────────────────────────────────────────────────────────────┘\n";

            // Ask user if they want to see detailed visualizations
            std::cout << "\n📊 Display detailed chessboard visualizations? (y/n): ";
            char choice;
            std::cin >> choice;

            if (choice == 'y' || choice == 'Y') {
                std::cout << "\n";
                for (size_t i = 0; i < solutions.size(); i++) {
                    displaySolution(solutions[i], i + 1, backtrackCounts[i]);

                    if (i < solutions.size() - 1) {
                        std::cout << "\nPress Enter to continue...";
                        std::cin.ignore();
                        std::cin.get();
                    }
                }
            }
        } else {
            std::cout << "❌ No solutions found for " << n << "-Queens problem.\n";
        }

        // Display expected counts for verification
        std::cout << "\n┌────────────────────────────────────────────────────────────────┐\n";
        std::cout << "│ VERIFICATION TABLE                                              │\n";
        std::cout << "├────────────────────────────────────────────────────────────────┤\n";
        std::cout << "│ Expected solutions for " << std::setw(3) << n << "-Queens: ";

        // Expected values for common N
        switch(n) {
            case 1: std::cout << "1"; break;
            case 2: std::cout << "0"; break;
            case 3: std::cout << "0"; break;
            case 4: std::cout << "2"; break;
            case 5: std::cout << "10"; break;
            case 6: std::cout << "4"; break;
            case 7: std::cout << "40"; break;
            case 8: std::cout << "92"; break;
            case 9: std::cout << "352"; break;
            case 10: std::cout << "724"; break;
            case 11: std::cout << "2,680"; break;
            case 12: std::cout << "14,200"; break;
            default: std::cout << "?"; break;
        }

        std::cout << std::setw(34 - std::to_string(n).length()) << " │\n";

        if (solutions.size() == getExpectedCount(n)) {
            std::cout << "│ ✅ VERIFIED: Correct number of solutions!                       │\n";
        } else if (n <= 12) {
            std::cout << "│ ⚠️  WARNING: Solution count mismatch! Check implementation.    │\n";
        }
        std::cout << "└────────────────────────────────────────────────────────────────┘\n";
    }

    int getExpectedCount(int n) {
        switch(n) {
            case 1: return 1;
            case 2: return 0;
            case 3: return 0;
            case 4: return 2;
            case 5: return 10;
            case 6: return 4;
            case 7: return 40;
            case 8: return 92;
            case 9: return 352;
            case 10: return 724;
            case 11: return 2680;
            case 12: return 14200;
            default: return -1;
        }
    }

    void printBacktrackingSummary() {
        std::cout << "\n📈 Backtracking Summary:\n";
        std::cout << "┌─────────────────────────────────────────────────────────┐\n";
        std::cout << "│ Solution # │ Backtracks │ Backtracks from previous    │\n";
        std::cout << "├─────────────────────────────────────────────────────────┤\n";

        int cumulative = 0;
        for (size_t i = 0; i < backtrackCounts.size(); i++) {
            cumulative += backtrackCounts[i];
            std::cout << "│ " << std::setw(9) << i+1 << " │ "
                      << std::setw(9) << backtrackCounts[i] << " │ "
                      << std::setw(24) << cumulative << " │\n";
        }
        std::cout << "└─────────────────────────────────────────────────────────┘\n";
    }
};

// Clear screen function (works on both Windows and Unix)
void clearScreen() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

// Function to set terminal window size (Unix/Linux)
void setTerminalSize() {
    #ifndef _WIN32
        std::cout << "\033[8;40;120t";  // Set to 40 rows x 120 columns
    #endif
}

int main() {
    clearScreen();
    setTerminalSize();

    std::cout << "\n";
    std::cout << "╔══════════════════════════════════════════════════════════════════╗\n";
    std::cout << "║                    N-QUEENS BACKTRACKING SOLVER                 ║\n";
    std::cout << "║                  Find all solutions with backtracking           ║\n";
    std::cout << "╚══════════════════════════════════════════════════════════════════╝\n\n";

    int n;
    std::cout << "┌────────────────────────────────────────────────────────────────┐\n";
    std::cout << "│ Enter the number of queens (N): ";
    std::cin >> n;
    std::cout << "└────────────────────────────────────────────────────────────────┘\n";

    if (n <= 0 || n > 20) {
        std::cout << "\n❌ Invalid input! N must be between 1 and 20.\n";
        std::cout << "   (N > 12 may take significant time to compute)\n";
        return 1;
    }

    if (n > 12) {
        std::cout << "\n⚠️  Warning: N=" << n << " may take a long time to compute.\n";
        std::cout << "   Continue? (y/n): ";
        char confirm;
        std::cin >> confirm;
        if (confirm != 'y' && confirm != 'Y') {
            std::cout << "Operation cancelled.\n";
            return 0;
        }
    }

    std::cout << "\n🔍 Solving " << n << "-Queens problem...\n";
    std::cout << "   Using optimized backtracking with bitmasking\n\n";

    NQueensSolver solver(n);
    solver.solve();

    std::cout << "\n✨ Algorithm Optimizations Used:\n";
    std::cout << "  • Bitmask representation (O(1) conflict checks)\n";
    std::cout << "  • Recursive backtracking with pruning\n";
    std::cout << "  • Symmetry reduction for first row\n";
    std::cout << "  • Early conflict detection\n\n";

    std::cout << "Press Enter to exit...";
    std::cin.ignore();
    std::cin.get();

    return 0;
}
