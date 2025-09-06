# Задачи с сайта codingame.com для Школы разработчиков

## Содержание

1.  [Easy](#1easy)
    * 1.1. [Table Of Contents](#11table-of-contents)
2.  [Medium](#2medium)
    * 2.1. [Telephone Numbers](#21telephone-numbers)
3.  [Задачи Hard](#задачи-hard)

---

### 1. Easy
В данном разделе описаны задачи с уровнем `Easy` с сайта codingame.com.
Их список представлен в соответствующем [разделе](https://github.com/WhoAmI1909/ProsoftSystemsClasses2025_2026/tree/main/Codingames/01_Easy).

#### 1.1. Table Of Contents
Решение представлено в файле [table_of_contents.cpp](https://github.com/WhoAmI1909/ProsoftSystemsClasses2025_2026/blob/main/Codingames/01_Easy/table_of_contents.cpp).

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


#### Название второй средней задачи

### Задачи Hard

#### Название первой сложной задачи

#### Название второй сложной задачи
