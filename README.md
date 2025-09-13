# Задачи с сайта codingame.com для Школы разработчиков

## Содержание

1.  [Easy](#1-easy)
    * 1.1. [Table Of Contents](#11-table-of-contents)
    * 1.2. [Logic Gates](#12-logic-gates)
2.  [Medium](#2-medium)
    * 2.1. [Telephone Numbers](#21-telephone-numbers)
    * 2.2. [Pachinko Jackpot](#22-pachinko-jackpot)
    * 2.3. [Don't Panic, Episode 1](#23-dont-panic-episode-1)
3.  [Hard](#3-hard)
    * 3.1. [Don't Panic, Episode 2](#31-dont-panic-episode-2)
    * 3.2. [Death First Search, Episode 2](#32-death-first-search-episode-2)
    * 3.3. [Chess Cavalry](#33-chess-cavalry)
    * 3.4. [Super Computer](#34-super-computer)
    * 3.5. [Electrical Grid](#35-electrical-grid)

---

Ссылка на профиль codingame: [WhoAmI1909](https://www.codingame.com/profile/fbd8bf22b913d7403a1e33d97ab9d01a2675793).

### 1. Easy
В данном разделе описаны задачи с уровнем `Easy` с сайта codingame.com.
Их список представлен в соответствующем [разделе](https://github.com/WhoAmI1909/ProsoftSystemsClasses2025_2026/tree/main/Codingames/01_Easy).

#### 1.1. Table Of Contents
Решение представлено в файле [table_of_contents.cpp](https://github.com/WhoAmI1909/ProsoftSystemsClasses2025_2026/blob/main/Codingames/01_Easy/table_of_contents.cpp).

Ссылка на саму задачу: [Table Of Contents](https://www.codingame.com/training/easy/table-of-contents).

Задача заключалась в том, чтобы получить вид содержания/оглавления для входных данных.
Создается структура Entry, которая хранит в себе всю необходимую информацию для корректного вывода:
```cpp
struct Entry {
    string title;   // Заголовок
    string page;    // Страница
    int level;      // Уровень вложенности
    int number;     // Порядковый номер заголовка во вложенности
};
```
С помощью `vector` создается вектор `entries` со всеми входными заголовками.
Далее создается вектор `level_counters` размером 10 с нулевыми значениями.
Вектор `level_counters` используется для подсчета уровней вложенности.

В основном цикле функции `main` вызывается функция `formatter`, которая форматирует входные данные в нужный вид.
Итоговая строка собирается из нескольких составляющих:
* отступ `indent` для уровня вложенности
* уровень вложенности `level`
* заголовок `title`
* пробелы `space`
* страница `page`
Сама функция `formatter`:
```cpp
string formatter(Entry entry, int lengthofline) {
    string title = exchange_tab_to_space(entry.title);
    string level = to_string(entry.level) + " ";
    string indent = string((entry.level - 1) * 4, ' ');
    string space = string(lengthofline - indent.length() - level.length() - title.length() - entry.page.length(), '.');
    return indent + level + title + space + entry.page + "\n";
}
```
Функция `exchange_tab_to_space` заменяет входные символы `>` на пустую строку.

### 1.2. Logic Gates
Решение представлено в файле [logic_signals.cpp](https://github.com/WhoAmI1909/ProsoftSystemsClasses2025_2026/blob/main/Codingames/01_Easy/logic_signals.cpp).

Ссылка на саму задачу: [Logic Gates](https://www.codingame.com/training/easy/logic-gates).

Решение основано на поэлементном разборе входящих сигналов, их преобразовании в логическую величину с последующей обработкой логической операции. По имени определяется последовательность входного сигнала. 

### 2. Medium
В данном разделе описаны задачи с уровнем `Medium` с сайта codingame.com.
Их список представлен в соответствующем [разделе](https://github.com/WhoAmI1909/ProsoftSystemsClasses2025_2026/tree/main/Codingames/02_Medium).

#### 2.1. Telephone Numbers
Решение представлено в файле [telephone_numbers.cpp](https://github.com/WhoAmI1909/ProsoftSystemsClasses2025_2026/blob/main/Codingames/02_Medium/telephone_numbers.cpp).

Ссылка на саму задачу: [Telephone Numbers](https://www.codingame.com/training/medium/telephone-numbers).

Задача заключалась в том, чтобы сохранить все введенные номера, при этом избежать дублирования каких-либо цифр из начала номера. Это позволит сократить объем памяти, затрачиваемый на хранение всех введенных номеров.
На выходе программы ожидалось количество, условно, ячеек памяти, требующихся для хранения всех номеров.
Для этого были созданы две структуры данных:
* `Node` - узел дерева, которое хранит указатели на дочерние узлы, соответствующие различным цифрам;
```cpp
struct Node {
    Node* children[10];
    Node() {
        for(int i = 0; i < 10; i++) {
            this->children[i] = nullptr;
        }
    }
};
```
В конструкторе `Node` создается массив указателей на дочерние узлы, которые будут храниться в дереве.

* `TelephoneNumbers` - дерево, которое хранит указатели на корень дерева.
```cpp
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
```
В конструкторе `TelephoneNumbers` создается корень дерева, в котором хранятся указатели на дочерние узлы.
У структуры `TelephoneNumbers` есть метод `insert_phone_number`, который выполняет запись нового номера.
Принцип работы:
1. Создается указатель `node` на корень дерева.
2. Перебираются все цифры в номере, используя `for` цикл.
3. Проверяется, есть ли узел в дереве, соответствующий данной цифре.
4. Если узел отсутствует, то он создается и добавляется в дерево.
5. Перемещается указатель `node` на узел, соответствующий данной цифре.
6. Количество узлов в дереве увеличивается на 1.

В основной функции `main` создается объект `TelephoneNumbers`, в котором хранятся все введенные номера.
После каждого введенного номера выполняется его обработка и сохранения в дереве.
В конце основной функции `main` выводится количество узлов в дереве.

#### 2.2. Pachinko Jackpot
Решение представлено в файле [pachinko_jackpot.cpp](https://github.com/WhoAmI1909/ProsoftSystemsClasses2025_2026/blob/main/Codingames/02_Medium/pachinko_jackpot.cpp).

Ссылка на саму задачу: [Pachinko Jackpot](https://www.codingame.com/training/medium/pachinko-jackpot).

Задача заключалась в подсчете максимально возможного выигрыша в игре пачинко (Патинко).

Решение задачи основано на рекурсии, используя динамическое программирование.
Для этого была создана одна функция `max_prize`:
```cpp
int max_prize(int **matrix, int dimensions_size, int currRow, int currCol, int sum_increment) {
    // Если дошли до нижней строчки с выигрышем
    if (currRow == dimensions_size - 1) {
        return sum_increment * matrix[currRow][currCol];
    }
    sum_increment = sum_increment + matrix[currRow][currCol];
    
    // Рекурсивно идем в обе возможные клетки следующей строки
    int left_prize = max_prize(matrix, dimensions_size, currRow + 1, currCol, sum_increment);
    int right_prize = max_prize(matrix, dimensions_size, currRow + 1, currCol + 1, sum_increment);
    
    // Возвращаем максимальный результат
    return max(left_prize, right_prize);
}
```
Известно, что из каждой точки можно двигаться в двух направлениях. На этом основан рекурсивный алгоритм. Каждый раз выполняется переход к ячейке ниже, вплоть до выигрыша. При этом выполняется суммирование множителя выигрыша в каждой ячейке по пути до конца. При достижении выигрыша, выполняется расчет значения и выход из функции, постепенно выбирая максимальное значение из всех найденных.

Проблема данного подхода в том, что с увеличением количества слоев, возрастает время, затрачиваемое функцией для определения максимального выигрыша.

#### 2.3. Don't Panic, Episode 1
Решение представлено в файле [dont_panic_ep1.cpp](https://github.com/WhoAmI1909/ProsoftSystemsClasses2025_2026/blob/main/Codingames/02_Medium/dont_panic_ep1.cpp).

Ссылка на саму задачу: [Don't Panic, Episode 1](https://www.codingame.com/training/medium/don't-panic-episode-1).

Решение основано на постоянной проверки текущего расположения клона относительно его цели: лифта или выхода.
В зависимости от этого, выполняется переход в нужную ячейку.

Для этой цели была написана функция:
```cpp
void AnalyzeClonePos(int pos, int clone_pos, string direction) {
    if (((clone_pos > pos) && (direction == "RIGHT")) ||
        ((clone_pos < pos) && (direction == "LEFT"))) {
        cout << "BLOCK" << endl;
    }
    else { cout << "WAIT" << endl; }	
}
```
Она выполняет анализ местоположения объектов на этаже и текущего направления движения клона.

### 3. Hard

#### 3.1. Don't Panic, Episode 2
Решение представлено в файле [dont_panic_ep2.cpp](https://github.com/WhoAmI1909/ProsoftSystemsClasses2025_2026/blob/main/Codingames/03_Hard/dont_panic_ep2.cpp).

Ссылка на саму задачу: [Don't Panic, Episode 2](https://www.codingame.com/training/hard/don't-panic-episode-2).

Решение имеет в основе тот же подход, что и в задаче `Don't Panic, Episode 1`.
Так как лифта может не оказаться на этаже, была создана функция, которая выполняет поиск лифта. Она возвращает индекс лифта.

```cpp
int SearchElevator(
    map<int, vector<int>>* elevatorsMap,
    int clone_pos,
    int clone_floor
) {
    int idx_closest_elevator = -1;
    if ((*elevatorsMap).find(clone_floor) != (*elevatorsMap).end()) {
        vector<int> elevators = (*elevatorsMap)[clone_floor];
        int distance_to_closest_elevator = 100;
        for (int i = 0; i < elevators.size(); i++) {
            if (abs(elevators[i] - clone_pos) < distance_to_closest_elevator) {
                distance_to_closest_elevator = abs(elevators[i] - clone_pos);
                idx_closest_elevator = i;
            }
        }
    }
    return idx_closest_elevator;
}
```

Так как в задаче присутствует возможность возведения дополнительных точек подъема на следующий этаж, необходимо вести учет количество лифтов, которые можно использовать для более оптимального подъема по этажам. Для этого введена функция:
```cpp
int define_extra_elevators(int floors, int nb_additional_elevators, map<int, vector<int>> elevatorsMap) {
    int neccesary_elevators = 0;
    for (int i = 0; i < floors - 1; i++) {
        if (elevatorsMap.find(i) == elevatorsMap.end()) {
            neccesary_elevators++;
        }
    }
    return nb_additional_elevators - neccesary_elevators;
}
```

Также дополнительно добавлена функция, которая обрабатывает случаи с лифтами.
* Если лифта нет, то на месте клона возводится новый лифт, который далее вносится в общий список, чтобы корректно учитывать существующие лифты.
* Если несколько лифтов на этаже, то берется ближайший к клону.
* Также выполняется анализ расстояния до ближайшего лифта. Если он больше, чем расстояние до ближайшей границе карты, то при избытке дополнительных лифтов возводится новый на месте лидирующего клона.

```cpp
void AnalyzeElevatorPos(
    map<int, vector<int>> *elevatorsMap,
    int elevator_idx,
    int clone_pos,
    int clone_floor,
    string direction,
    int* nb_additional_elevators,
    int* extra_elevators,
    int width
) {
    if (elevator_idx == -1) {
        cout << "ELEVATOR" << endl;
        (*nb_additional_elevators)--;
        (*elevatorsMap)[clone_floor].push_back(clone_pos);
    }
    else {
        int closest_distance_to_board = clone_pos < width - clone_pos ? clone_pos : width - clone_pos;
        if (((*extra_elevators) > 0) && (closest_distance_to_board < abs((*elevatorsMap)[clone_floor][elevator_idx] - clone_pos))) {
            cout << "ELEVATOR" << endl;
            (*extra_elevators)--;
            (*elevatorsMap)[clone_floor].push_back(clone_pos);
            return;
        }
        AnalyzeClonePos((*elevatorsMap)[clone_floor][elevator_idx], clone_pos, direction);
    }
}
```

Из минусов решения, не выполняется поиск оптимального пути до выхода, ввиду чего не проходят все тесты.


#### 3.2. Death First Search, Episode 2
Решение представлено в файле [death_first_search_ep2.cpp](https://github.com/WhoAmI1909/ProsoftSystemsClasses2025_2026/blob/main/Codingames/03_Hard/death_first_search_ep2.cpp).

Ссылка на саму задачу: [Death First Search, Episode 2](https://www.codingame.com/ide/puzzle/death-first-search-episode-2).

Задача решается с использованием поиска в ширину.

Структура каждого узла представляет собой множество индексов связанных узлов: как между простыми, так и между выходами.
```cpp
struct Node {
    set<int> links;
    set<int> links_to_gateways;
};
```

Основной функцией является `pair<int, int> BFS(vector<Node> &graph, int BobNet_node_id)`, которая выполняет поиск в ширину и возвращает пару индексов узлов, связь между которыми необходимо отключить.
```cpp
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
```

Приоритет отдается узлам, у которых есть связь с несколькими выходами. Они изолируются в первую очередь.

Поиск заканчивается тогда, когда был найден узел, у которого есть связь с выходом.

#### 3.3 Chess Cavalry
Решение представлено в файле [chess_cavalry.cpp](https://github.com/WhoAmI1909/ProsoftSystemsClasses2025_2026/blob/main/Codingames/03_Hard/chess_cavalry.cpp).

Ссылка на саму задачу: [Chess Cavalry](https://www.codingame.com/training/hard/chess-cavalry).

Задача заключается в поиске кратчайшего пути до точки назначения, учитывая возможные препятствия на шахматной доске.
Были написаны две функции.
Первая выполняет поиск стартовой позиции `Knight`.
```cpp
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
```
Вторая функция реализует непосредственно сам алгоритм поиска. В его основе лежит поиск в ширину.
```cpp
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
```

Были созданы две вспомогательные таблицы данных: `distance` и `visited`.
* `distance` хранит расстояние от начальной позиции до каждой клетки доски.
* `visited` хранит информацию о том, была ли клетка посещена.

Поочередно рассматриваются все возможные ходы рыцаря и проверяется, можно ли встать на клетку (не препятствие).
Если да, то клетка помечается как посещенная и расстояние до нее увеличивается на 1.
Если клетка является целью, то расстояние до нее вычисляется и выводится на экран.
Если цель не найдена, выводится сообщение об этом.

#### 3.4. Super Computer
Решение представлено в файле [super_computer.cpp](https://github.com/WhoAmI1909/ProsoftSystemsClasses2025_2026/blob/main/Codingames/03_Hard/super_computer.cpp).

Ссылка на саму задачу: [Super Computer](https://www.codingame.com/training/hard/super-computer).

Задача заключается в поиске максимального количества операций, кооторые можно выполнить друг за другом.

Решение основано на применении "жадного" алгоритма.
Для удобства была создана структура `Operation` для хранения информации о каждой операции.

```cpp
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
```
В конструкторе определяется начало и конец операции.
Был также переопределен оператор `<` для сравнения операции по концу.
Это требуется для выполнения сортировке по дате окончания операции.

Основная функция выполнения подсчета максимального количества операций:
```cpp
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
```

В функции подсчитываются операции, которые начинаются и заканчиваются максимально близко друг к другу.
Так как пересечения расписаний недопускаются, необходимо выполнить наибольшее количество операций.
#### 3.5. Electrical Grid
Решение представлено в файле [electrical_grid.cpp](https://github.com/WhoAmI1909/ProsoftSystemsClasses2025_2026/blob/main/Codingames/03_Hard/electrical_grid.cpp).

Ссылка на саму задачу: [Chess Cavalry](https://www.codingame.com/training/hard/electrical-grid).

Данный паззл представляет собой задачу поиска остовного дерева в графах - найти подграф без циклов с минимальным весом ребер.

Для удобства описания и использования данных о связях была создана структура `Link`
```cpp
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
```

В ней реализован метод вывода информации в консоль так, как этого требует задание.
Также переопределен метод сравнения, который используется только для итогового вывода в отсортированном виде.

Основная функция, которая выполняет поиск путей и вывод подграфа в консоль в требуемом виде:
```cpp
void get_minimal_links(vector<Link> links, map<int, int>& initial_components) {    
    int links_cost = 0;
    int links_count = 0;
    vector<Link> necessary_links;
    
    for (const Link& link : links) {
        int root1 = find_root(initial_components, link.house_1);
        int root2 = find_root(initial_components, link.house_2);
        
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
```
На вход приходит отсортированный список ребер по их весу в порядке возрастания.

Инициализация вспомогательных данных:
  - общая стоимость ребер;
  - количество ребер;

Определяется "корень" для каждого узла, это аналог компоненты связности.
Для этого написана функция `find_root`:
```cpp
int find_root(map<int, int>& initial_components, int x) {
    if (initial_components[x] != x) {
        initial_components[x] = find_root(initial_components, initial_components[x]);
    }
    return initial_components[x];
}
```
В представленном словаре она ище исходный узел, с которого начались все связи.

Если узлы отличаются, то необходимо объединить узлы, то есть добавить связь.

В конце выполняется вывод суммы весов ребер и их общее количество, а также вывод связей в подграфе в отсортированном виде.
