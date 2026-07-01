# Quickstart: PowerFlowCalc

**Date**: 2026-07-01

## Предварительные требования

- Компилятор C++17 (GCC ≥ 8, Clang ≥ 7)
- CMake ≥ 3.15
- Git (для FetchContent загрузки зависимостей)

Сборка:

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

## Проверка работоспособности

### Сценарий 1: Простейшая сеть (2 узла)

**Входной файл**: `data/test_2node.json`

```json
{
  "base_voltage_kV": 110,
  "base_power_MVA": 100,
  "tolerance": 0.001,
  "max_iterations": 100,
  "nodes": [
    {"id": 1, "name": "Шина", "type": "slack", "voltage_kV": 115},
    {"id": 2, "name": "Нагрузка", "type": "load", "power_MW": 50}
  ],
  "branches": [
    {"id": 1, "name": "ЛЭП", "from_node_id": 1, "to_node_id": 2, "resistance_ohm": 10.0}
  ]
}
```

**Запуск**:
```bash
./build/powerflow_calc data/test_2node.json
```

**Ожидаемый результат**:
- Узел 1: U = 1.045 о.е. (115 кВ / 110 кВ)
- Узел 2: U ≈ 0.997 о.е.
- Поток P₁₂ ≈ 50 МВт, направление 1→2
- Баланс: генерация ≈ 50 МВт, нагрузка = 50 МВт, невязка ≈ 0
- Статус: Сошлось

### Сценарий 2: Сеть с 3 узлами и генератором

**Входной файл**: `data/test_3node.json`

```json
{
  "base_voltage_kV": 110,
  "base_power_MVA": 100,
  "tolerance": 0.001,
  "max_iterations": 100,
  "nodes": [
    {"id": 1, "type": "slack", "voltage_kV": 115},
    {"id": 2, "type": "load", "power_MW": 50},
    {"id": 3, "type": "generator", "power_MW": 30}
  ],
  "branches": [
    {"id": 1, "from_node_id": 1, "to_node_id": 2, "resistance_ohm": 10.0},
    {"id": 2, "from_node_id": 2, "to_node_id": 3, "resistance_ohm": 5.0},
    {"id": 3, "from_node_id": 1, "to_node_id": 3, "resistance_ohm": 20.0}
  ]
}
```

**Запуск**:
```bash
./build/powerflow_calc data/test_3node.json
```

**Ожидаемый результат**:
- Баланс: генерация = slack_генерация + 30 МВт (генератор) = 20 + 30 ≈ 50 МВт
- Потоки распределены с учетом сопротивлений
- Статус: Сошлось

### Сценарий 3: Два независимых островка

**Входной файл**: `data/test_two_islands.json`

```json
{
  "base_voltage_kV": 110,
  "base_power_MVA": 100,
  "tolerance": 0.001,
  "max_iterations": 100,
  "nodes": [
    {"id": 1, "type": "slack", "voltage_kV": 115},
    {"id": 2, "type": "load", "power_MW": 30},
    {"id": 3, "type": "slack", "voltage_kV": 220},
    {"id": 4, "type": "load", "power_MW": 40}
  ],
  "branches": [
    {"id": 1, "from_node_id": 1, "to_node_id": 2, "resistance_ohm": 5.0},
    {"id": 2, "from_node_id": 3, "to_node_id": 4, "resistance_ohm": 8.0}
  ]
}
```

**Запуск**:
```bash
./build/powerflow_calc data/test_two_islands.json
```

**Ожидаемый результат**:
- Островок 0: узлы 1, 2 — slack 115 кВ, нагрузка 30 МВт
- Островок 1: узлы 3, 4 — slack 220 кВ, нагрузка 40 МВт
- Две группы таблиц в выводе + сводная
- Статус: Сошлось

### Сценарий 4: Ошибка валидации (островок без slack)

**Входной файл**: `data/test_no_slack.json`

```json
{
  "base_voltage_kV": 110,
  "base_power_MVA": 100,
  "tolerance": 0.001,
  "max_iterations": 100,
  "nodes": [
    {"id": 1, "type": "load", "power_MW": 50},
    {"id": 2, "type": "load", "power_MW": 30}
  ],
  "branches": [
    {"id": 1, "from_node_id": 1, "to_node_id": 2, "resistance_ohm": 10.0}
  ]
}
```

**Запуск**:
```bash
./build/powerflow_calc data/test_no_slack.json
```

**Ожидаемый результат**:
- Код возврата: 1 (ни один островок не рассчитан — единственный островок без slack)
- Сообщение: "Предупреждение: островок 0 пропущен — нет узла типа 'slack'. Каждый связный компонент сети должен содержать ровно один балансирующий узел."

### Запуск тестов

```bash
cmake --build build && cd build && ctest --output-on-failure
```

Ожидаемый результат: все тесты проходят (зеленый набор).
