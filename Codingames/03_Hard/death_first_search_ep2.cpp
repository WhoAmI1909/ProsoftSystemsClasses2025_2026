#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <set>
#include <queue>

using namespace std;

struct Node {
    set<int> links;
    set<int> links_to_gateways;
};

pair<int, int> BFS(vector<Node> &graph, int BobNet_node_id) {
    vector<bool> visited_nodes(graph.size(), false);
    queue<int> q;
    q.push(BobNet_node_id);

    visited_nodes[BobNet_node_id] = true;
    int selectedNode = -1;

    while (!q.empty()) {
        int node_id = q.front();
        q.pop();
        Node node = graph[node_id];
        visited_nodes[node_id] = true;

        const auto push_neighbours = [&node, &q, &visited_nodes]() { 
            for (const int& node_id : node.links) {
                if (!visited_nodes[node_id]) q.push(node_id);
            }
        };

        if (node.links_to_gateways.size() > 1) {
            selectedNode = node_id;
            break;
        }
        else if (node.links_to_gateways.size() == 1) {
            if (selectedNode == -1) {
                selectedNode = node_id;
                if (node_id == BobNet_node_id) break;
            }
            push_neighbours();
        }
        else if(selectedNode == -1) push_neighbours();
    }
    return make_pair(selectedNode, *(graph[selectedNode].links_to_gateways.begin()));
}

int main()
{
    int n; // the total number of nodes in the level, including the gateways
    int l; // the number of links
    int e; // the number of exit gateways
    cin >> n >> l >> e; cin.ignore();

    vector<Node> graph(n, {set<int>(), set<int>()});

    for (int i = 0; i < l; i++) {
        int n1; // N1 and N2 defines a link between these nodes
        int n2;
        cin >> n1 >> n2; cin.ignore();
        graph[n1].links.insert(n2);
        graph[n2].links.insert(n1);
    }
    for (int i = 0; i < e; i++) {
        int ei; // the index of a gateway node
        cin >> ei; cin.ignore();
        for (const int& node_id : graph[ei].links) 
            graph[node_id].links_to_gateways.insert(ei);
    }

    // game loop
    while (1) {
        int si; // The index of the node on which the Bobnet agent is positioned this turn
        cin >> si; cin.ignore();

        pair<int, int> link = BFS(graph, si);
        cout << link.first << ' ' << link.second << endl;
        // Cut link:
        graph[link.first].links_to_gateways.erase(link.second);
        graph[link.first].links.erase(link.second);
        graph[link.second].links.erase(link.first);
    }
}