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
        return this->cost < other.cost;
    }

    void print() {
        cout << this->house_1 << " " << this->house_2 << " " << this->cost << endl;
    }
};

// Функция для поиска корня компоненты связности
int find_root(map<int, int>& parent, int x) {
    if (parent[x] != x) {
        parent[x] = find_root(parent, parent[x]);
    }
    return parent[x];
}

// Объединение двух компонент связности
void union_components(map<int, int>& parent, int x, int y) {
    int root_x = find_root(parent, x);
    int root_y = find_root(parent, y);
    if (root_x != root_y) {
        parent[root_y] = root_x;
    }
}

void get_minimal_links(vector<Link> links, map<int, int> initial_components) {
     // Создаем структуру для Union-Find
    map<int, int> parent;
    for (auto& comp : initial_components) {
        parent[comp.first] = comp.first; // Изначально каждый узел - корень сам себе
    }
    
    // Сортируем связи по стоимости (алгоритм Крускала)
    sort(links.begin(), links.end(), [](const Link& a, const Link& b) {
        return a.cost < b.cost;
    });
    
    int links_cost = 0;
    int links_count = 0;
    vector<Link> necessary_links;
    
    for (const Link& link : links) {
        int root1 = find_root(parent, link.house_1);
        int root2 = find_root(parent, link.house_2);
        
        // Если узлы в разных компонентах связности
        if (root1 != root2) {
            union_components(parent, link.house_1, link.house_2);
            links_cost += link.cost;
            links_count++;
            necessary_links.push_back(link);
        }
    }
    cout << links_count << ' ' << links_cost << endl;
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
    sort(links.begin(), links.end());
    get_minimal_links(links, connected_components);    
}