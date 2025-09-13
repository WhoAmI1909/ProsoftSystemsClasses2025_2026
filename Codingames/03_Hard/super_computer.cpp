#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;

struct Operation {
    int start;
    int end;
    Operation(int s, int duration) : start(s) {
        end = s + duration - 1;
    }

    bool operator<(const Operation& other) const {
        return end < other.end;
    }
};

int findMaxNonOverlappingOperations(const vector<Operation>& operations) {
    if (operations.empty()) return 0;

    int count = 1;
    int lastEnd = operations[0].end;

    for (size_t i = 1; i < operations.size(); i++) {
        if (operations[i].start > lastEnd) {
            count++;
            lastEnd = operations[i].end;
        }
    }
    return count;
}
        
int main()
{
    int n;
    cin >> n; cin.ignore();
    vector<Operation> operations;
    for (int i = 0; i < n; i++) {
        int j, d;
        cin >> j >> d; cin.ignore();
        operations.push_back(Operation(j, d));
    }

    sort(operations.begin(), operations.end());
    cout << findMaxNonOverlappingOperations(operations) << endl;
}