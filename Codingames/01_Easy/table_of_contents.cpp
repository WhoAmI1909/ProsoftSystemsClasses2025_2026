#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>

using namespace std;

/**
 * Auto-generated code below aims at helping you parse
 * the standard input according to the problem statement.
 **/
struct Entry {
    string title;   // Заголовок
    string page;    // Страница
    int level;      // Уровень вложенности
    int number;     // Номер во вложенности
};

string exchange_tab_to_space(const string& str) {
    string result = str;
    size_t idx = 0;
    while ((idx = result.find('>', idx)) != string::npos) {
        result.replace(idx, 1, "");
        idx += 1;
    }
    return result;
}

int define_level(const string& str) {
    int level = 1;
    for (char c : str) {
        if (c == '>') {
            level++;
        }
    }
    return level;
}

string formatter(Entry entry, int lengthofline) {
    string title = exchange_tab_to_space(entry.title);
    string level = to_string(entry.level) + " ";
    string indent = string((entry.level - 1) * 4, ' ');
    string space = string(lengthofline - indent.length() - level.length() - title.length() - entry.page.length(), '.');
    return indent + level + title + space + entry.page + "\n";
}

int main()
{
    vector<Entry> entries;
    int lengthofline;
    cin >> lengthofline; cin.ignore();
    int n;
    cin >> n; cin.ignore();
    for (int i = 0; i < n; i++) {
        Entry title;
        string entry;
        getline(cin, entry);

        stringstream ss(entry);
        ss >> title.title;
        ss >> title.page;
        title.level = define_level(title.title);
        entries.push_back(title);
    }

    vector<int> level_counters(10, 0);
    for (int i = 0; i < n; i++) {
        int level = entries[i].level;
        level_counters[level]++;

        for (int j = level + 1; j < level_counters.size(); j++) {
            level_counters[j] = 0;
        }
        entries[i].number = level_counters[level];

        cout << formatter(entries[i], lengthofline);
    }
}