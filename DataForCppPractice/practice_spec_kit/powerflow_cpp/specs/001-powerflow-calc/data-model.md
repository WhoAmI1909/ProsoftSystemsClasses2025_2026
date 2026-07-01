# Data Model: PowerFlowCalc

**Date**: 2026-07-01

## Entity Relationship

```
NetworkData (1) ──── (1..N) Island
Island       (1) ──── (1..N) Node
Island       (1) ──── (0..N) Branch
Island       (N) ──── (N)    IslandResult (расчет)

Node         (1) ──── (0..N) Branch (from_node_id)
Node         (1) ──── (0..N) Branch (to_node_id)
```

## Entities

### Node (Узел)

Точка подключения в электрической сети.

| Поле | Тип | Обязательное | Описание |
|------|-----|------------|----------|
| id | int | Да | Уникальный числовой идентификатор (>0) |
| name | string | Опционально | Человекочитаемое название (может быть пустым) |
| type | enum: slack, load, generator | Да | Тип узла |
| voltage_kV | double | slack only | Заданное напряжение в кВ (>0) |
| power_MW | double | load/generator only | Заданная мощность в МВт (≥0) |

**Правила валидации**:
- id уникален в пределах сети.
- voltage_kV обязательно только для slack, должно быть > 0.
- power_MW обязательно для load и generator, должно быть ≥ 0.
- power_MW для load с нулевым значением допустим (транзитный узел).
- В JSON power_MW всегда указан положительным; знак определяется типом узла.

**Внутреннее представление мощностей**:
- load: P_sign = −power_MW (потребление)
- generator: P_sign = +power_MW (генерация)
- slack: мощность не задана, вычисляется из баланса

### Branch (Ветвь)

Линия электропередачи между двумя узлами.

| Поле | Тип | Обязательное | Описание |
|------|-----|------------|----------|
| id | int | Да | Уникальный числовой идентификатор (>0) |
| name | string | Опционально | Человекочитаемое название |
| from_node_id | int | Да | ID начального узла |
| to_node_id | int | Да | ID конечного узла |
| resistance_ohm | double | Да | Продольное активное сопротивление, Ом (>0) |

**Правила валидации**:
- id уникален в пределах сети.
- from_node_id и to_node_id ссылаются на существующие узлы.
- resistance_ohm > 0.
- Допускаются параллельные ветви (одинаковые from_node_id, to_node_id).
- Направление from→to влияет на знак потока при выводе.

### Island (Островок)

Связный компонент графа сети. Каждый островок содержит ровно один slack-узел
и рассчитывается независимо.

| Поле | Тип | Описание |
|------|-----|----------|
| island_id | int | Порядковый номер островка (0-based) |
| nodes | vector\<Node\> | Узлы, принадлежащие островку |
| branches | vector\<Branch\> | Ветви, обе конечные точки которых в островке |
| slack_node_id | int | ID slack-узла в островке |
| node_index_map | map<int,int> | Отображение глобального ID узла → локальный индекс (0..M-1) |

**Инварианты**:
- Островок содержит ровно один slack-узел. Нарушение → ошибка валидации.
- Все узлы островка достижимы от slack-узла через цепочку ветвей.
- Баланс мощностей в островке: |ΣP_gen − ΣP_load| / max(ΣP_gen, ΣP_load) ≤ 5%.

### NetworkData (Сетевые данные)

Полная загруженная модель сети после валидации.

| Поле | Тип | Описание |
|------|-----|----------|
| base_voltage_kV | double | Базисное напряжение (>0) |
| base_power_MVA | double | Базисная мощность (>0) |
| tolerance | double | Допустимая невязка для сходимости (>0) |
| max_iterations | int | Максимальное число итераций (>0) |
| islands | vector\<Island\> | Список островков (минимум 1) |

### IslandResult (Результат расчета островка)

| Поле | Тип | Описание |
|------|-----|----------|
| island_id | int | Порядковый номер островка |
| voltage_pu | vector\<double\> | Напряжения узлов, о.е. (U_base = base_voltage_kV) |
| voltage_kV | vector\<double\> | Напряжения узлов, кВ |
| flow_MW | vector\<double\> | Потоки мощности по каждой ветви, МВт |
| flow_direction | vector\<int\> | Направление: +1 = from→to, −1 = to→from |
| total_generation_MW | double | Суммарная генерация в островке |
| total_load_MW | double | Суммарная нагрузка в островке |
| imbalance_MW | double | Невязка (|генерация − нагрузка|) |
| converged | bool | Статус сходимости |
| iterations | int | Число выполненных итераций |
| final_residual | double | Финальная невязка (max|ΔU|) |

### PowerFlowResult (Общие результаты)

| Поле | Тип | Описание |
|------|-----|----------|
| island_results | vector\<IslandResult\> | Результаты по каждому островку |
| total_generation_MW | double | Суммарная генерация по всем островкам |
| total_load_MW | double | Суммарная нагрузка по всем островкам |
| total_imbalance_MW | double | Суммарная невязка по всем островкам |

## Поток данных (Data Flow)

```
JSON-файл
  │
  ▼
DataLoader::load()
  ├── Парсинг JSON (nlohmann/json)
  ├── Валидация полей и типов
  ├── Построение графа (adjacency list)
  ├── BFS: выделение островков (связных компонент)
  ├── Проверка: ровно 1 slack на островок
  ├── Проверка: баланс мощностей в островке (≤5%)
  └── Возврат NetworkData
        │
        ▼  (для каждого Island)
NetworkBuilder::build_admittance_matrix(island)
  ├── Построение локальной нумерации узлов (0..M-1)
  ├── Y_ii = -Σ(g_ij) для всех ветвей узла i
  ├── Y_ij = +g_ij для связанных узлов
  └── Возврат MatrixXd Y, VectorXd P
        │
        ▼  (для каждого Island)
PowerFlowSolver::solve(Y, P, tolerance, max_iterations)
  ├── U_init = 1.0 для не-slack узлов, U_slack = 1.0
  ├── Цикл итераций:
  │   ├── I_i = P_i / U_i (для каждого узла кроме slack)
  │   ├── U_i_new = (I_i - Σ_{j≠i} Y(i,j)*U_j) / Y(i,i)
  │   ├── residual = max(|U_new - U_old|)
  │   └── Проверка: residual < tolerance?
  └── Возврат IslandResult
        │
        ▼  (для каждого Island)
FlowCalculator::calculate_flows(island, U)
  ├── Для каждой ветви (i,j):
  │   ├── P_ij = U_i * (U_i - U_j) / R_ij  (в о.е.)
  │   ├── P_ij_MW = P_ij * base_power_MVA
  │   └── Направление: from→to если P_ij > 0
  └── Возврат векторов flow_MW, flow_direction
        │
        ▼
ReportGenerator::generate(network_data, power_flow_result)
  ├── ASCII-таблицы в std::cout (по островкам + сводная)
  ├── Файл report.txt
  └── Коды завершения: 0 = успех
```
