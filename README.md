# Задачи с сайта codingame.com для Школы разработчиков

## Содержание

1.  [Easy](#1-easy)
    * 1.1. [Table Of Contents](#11-table-of-contents)
2.  [Medium](#2-medium)
    * 2.1. [Telephone Numbers](#21-telephone-numbers)
    * 2.2. [Pachinko Jackpot](#22-pachinko-jackpot)
    * 2.3. [Don't Panic, Episode 1](#23-dont-panic-episode-1)
3.  [Задачи Hard](#3-hard)
    * 3.1. [Don't Panic, Episode 2](#31-dont-panic-episode-2)
    * 3.2. [Death First Search, Episode 2](#32-death-first-search-episode-2)

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

`TODO`


### 3. Hard

#### 3.1. Don't Panic, Episode 2
Решение представлено в файле [dont_panic_ep2.cpp](https://github.com/WhoAmI1909/ProsoftSystemsClasses2025_2026/blob/main/Codingames/03_Hard/dont_panic_ep2.cpp).

Ссылка на саму задачу: [Don't Panic, Episode 2](https://www.codingame.com/training/hard/don't-panic-episode-2).

`TODO`

#### 3.2. Death First Search, Episode 2
Решение представлено в файле [death_first_search_ep2.cpp](https://github.com/WhoAmI1909/ProsoftSystemsClasses2025_2026/blob/main/Codingames/03_Hard/death_first_search_ep2.cpp).

Ссылка на саму задачу: [Death First Search, Episode 2](https://www.codingame.com/ide/puzzle/death-first-search-episode-2).

`TODO`