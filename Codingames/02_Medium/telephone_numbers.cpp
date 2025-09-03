#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <set>

using namespace std;

struct Node {
    Node *children[10];
    Node() {
        for(int i = 0; i < 10; i++) {
            this->children[i] = nullptr;
        }
    }
};

struct TelephoneNumbers {
    Node *root;
    int count = 0;

    TelephoneNumbers() {
        this->root = new Node();
        this->count = 0;
    }

    void insert_phone_number(string number) {
        Node *node = this->root;

        for (char c: number) {
            int digit = atoi(&c);
            if (node->children[digit] == nullptr) {
                node->children[digit] = new Node();
                count++;
            }

            node = node->children[digit];
        }
    }
};

int main()
{
    int n;
    cin >> n; cin.ignore();
    TelephoneNumbers numbers = TelephoneNumbers();
    int all_digits = 0;
    for (int i = 0; i < n; i++) {
        string telephone;
        cin >> telephone; cin.ignore();
        numbers.insert_phone_number(telephone);
    }
    cout << numbers.count << endl;
}
