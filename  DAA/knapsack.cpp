

#include <iostream>
#include <vector>
#include <algorithm>
#include <chrono>
#include <iomanip>

using namespace std;
using namespace chrono;

int main() {
    cout << "\n";
    cout << "╔════════════════════════════════════════════════════════════╗\n";
    cout << "║                   0/1 KNAPSACK SOLVER                      ║\n";
    cout << "╚════════════════════════════════════════════════════════════╝\n\n";

    int capacity, numItems;

    // Get inputs
    cout << "Knapsack capacity: ";
    cin >> capacity;
    cout << "Number of items: ";
    cin >> numItems;
    cin.ignore();

    vector<int> weights(numItems);
    vector<int> values(numItems);

    // Get items one by one
    cout << "\nEnter items (weight,value):\n";
    for (int i = 0; i < numItems; i++) {
        char comma;
        cout << "Item " << i+1 << ": ";
        cin >> weights[i] >> comma >> values[i];
    }

    // Display input
    cout << "\n┌──────────────────────────────────────────────────────────┐\n";
    cout << "│ INPUT                                                     │\n";
    cout << "├──────────────────────────────────────────────────────────┤\n";
    cout << "│ Capacity: " << capacity << ", Items: " << numItems << "                     │\n";
    cout << "└──────────────────────────────────────────────────────────┘\n";

    // Solve using DP
    auto start = high_resolution_clock::now();

    // DP Table
    vector<vector<int>> dp(numItems + 1, vector<int>(capacity + 1, 0));

    // Fill DP table
    for (int i = 1; i <= numItems; i++) {
        for (int w = 0; w <= capacity; w++) {
            if (weights[i-1] <= w) {
                dp[i][w] = max(values[i-1] + dp[i-1][w - weights[i-1]], dp[i-1][w]);
            } else {
                dp[i][w] = dp[i-1][w];
            }
        }
    }

    // Find selected items
    vector<int> selectedItems;
    int w = capacity;
    for (int i = numItems; i > 0; i--) {
        if (dp[i][w] != dp[i-1][w]) {
            selectedItems.push_back(i-1);
            w -= weights[i-1];
        }
    }
    reverse(selectedItems.begin(), selectedItems.end());

    auto end = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(end - start);

    // Display DP Table
    cout << "\n┌──────────────────────────────────────────────────────────┐\n";
    cout << "│ DP TABLE                                                  │\n";
    cout << "└──────────────────────────────────────────────────────────┘\n";

    // Print table header
    cout << "\n     ";
    for (int w = 0; w <= capacity; w++) {
        cout << setw(4) << w;
    }
    cout << "\n     ";
    for (int w = 0; w <= capacity; w++) {
        cout << "────";
    }
    cout << "\n";

    // Print table rows
    for (int i = 0; i <= numItems; i++) {
        if (i == 0) {
            cout << "0    ";
        } else {
            cout << "I" << i << "   ";
        }
        for (int w = 0; w <= capacity; w++) {
            cout << setw(4) << dp[i][w];
        }
        cout << "\n";
    }

    // Display results
    cout << "\n╔════════════════════════════════════════════════════════════╗\n";
    cout << "║                    SOLUTION                                 ║\n";
    cout << "╚════════════════════════════════════════════════════════════╝\n";

    cout << "\nMaximum Value: " << dp[numItems][capacity] << endl;
    cout << "Time: " << duration.count() / 1000.0 << " ms\n";

    cout << "\nSelected Items:\n";
    int totalWeight = 0;
    int totalValue = 0;

    for (int idx : selectedItems) {
        totalWeight += weights[idx];
        totalValue += values[idx];
        cout << "  Item " << idx+1 << ": weight=" << weights[idx]
             << ", value=" << values[idx] << endl;
    }

    cout << "\nTotal Weight: " << totalWeight << "/" << capacity << endl;
    cout << "Total Value: " << totalValue << endl;


    return 0;
}
