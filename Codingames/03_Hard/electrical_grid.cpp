#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <map>

using namespace std;

struct Link {
    int house_1;
    int house_2;
    int cost;

    bool operator<(const Link& other) {
    if (this->house_1 != other.house_1) {
        return this->house_1 < other.house_1;
    }
        return this->house_2 < other.house_2;
    }

    void print() {
        cout << this->house_1 << " " << this->house_2 << " " << this->cost << endl;
    }
};

// Функция для поиска корня компоненты связности
int find_root(map<int, int>& initial_components, int x) {
    if (initial_components[x] != x) {
        initial_components[x] = find_root(initial_components, initial_components[x]);
    }
    return initial_components[x];
}

void get_minimal_links(vector<Link> links, map<int, int>& initial_components) {    
    int links_cost = 0;
    int links_count = 0;
    vector<Link> necessary_links;
    
    for (const Link& link : links) {
        int root1 = find_root(initial_components, link.house_1);
        int root2 = find_root(initial_components, link.house_2);
        
        // Если узлы в разных компонентах связности
        if (root1 != root2) {
            initial_components[root2] = root1;
            links_cost += link.cost;
            links_count++;
            necessary_links.push_back(link);
        }
    }
    cout << links_count << ' ' << links_cost << endl;
    sort(necessary_links.begin(), necessary_links.end());
    for(Link link: necessary_links) {
        link.print();
    }
}


int main()
{
    int n;
    int m;
    cin >> n >> m; cin.ignore();
    map<int, int> connected_components;
    for(int i = 1; i <= n; i++) {
        connected_components[i] = i;
    }
    vector<Link> links;
    for (int i = 0; i < m; i++) {
        int house_1;
        int house_2;
        int cost;
        cin >> house_1 >> house_2 >> cost; cin.ignore();
        links.push_back({house_1, house_2, cost});
    }

    // Сортировка по "Весу" электрической связи
    sort(links.begin(), links.end(), [](const Link& a, const Link& b) {
        return a.cost < b.cost;
    });
    get_minimal_links(links, connected_components);    
}