#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <queue>

using namespace std;

pair<int, int> find_knight_position(vector<string> board) {
    for (int i = 0; i < board.size(); i++) {
        for (int j = 0; j < board[i].length(); j++) {
            if (board[i][j] == 'B') {
                return make_pair(i, j);
            }
        }
    }
    return make_pair(-1, -1);
};

void find_path(vector<string> board, pair<int, int> knight_position, int w, int h) {
    int possible_moves[8][2] = { {-2, 1}, {-1, 2}, {1, 2}, {2, 1}, {2, -1}, {1, -2}, {-1, -2}, {-2, -1} };
    
    // Создаем матрицу для хранения расстояний
    vector<vector<int>> distance(h, vector<int>(w, 0));
    distance[knight_position.first][knight_position.second] = 0;

    // Создается матрица посещаемых клеток
    vector<vector<bool>> visited(h, vector<bool>(w, false));
    visited[knight_position.first][knight_position.second] = true;

    pair<int, int> target_pos = { -1, -1 };
    int turns = 0;
    queue<pair<int, int>> q;
    bool is_path_found = false;

    q.push(knight_position);
    while (!q.empty()) {
        auto current = q.front();
        q.pop();
        int x = current.first;
        int y = current.second;
        // Если нашли цель
        if (board[x][y] == 'E') {
            target_pos = { x, y };
            is_path_found = true;
            break;
        }
        // Проверяем все возможные ходы рыцаря
        for (int i = 0; i < 8; i++) {
            int new_x = x + possible_moves[i][0];
            int new_y = y + possible_moves[i][1];

            // Проверяем границы доски
            if (new_x >= 0 && new_x < h && new_y >= 0 && new_y < w) {
                // Проверяем, можно ли встать на клетку (не препятствие)
                if (board[new_x][new_y] != '#' && !visited[new_x][new_y]) {

                    distance[new_x][new_y] = distance[x][y] + 1;
                    visited[new_x][new_y] = true;
                    q.push({ new_x, new_y });
                }
            }
        }
    }
    if (!is_path_found) {
        cout << "Impossible" << endl;
    }
    else {
        cout << distance[target_pos.first][target_pos.second] << endl;
    }
}


int main()
{
    int w;
    int h;
    vector<string> board;
    cin >> w >> h; cin.ignore();
    for (int i = 0; i < h; i++) {
        string row;
        cin >> row; cin.ignore();
        board.push_back(row);
    }

    pair<int, int> knight_position = find_knight_position(board);
    if (knight_position.first == -1 || knight_position.second == -1) {
        cout << "There is no knight on the board" << endl;
        return 0;
    }

    find_path(board, knight_position, w, h);
}