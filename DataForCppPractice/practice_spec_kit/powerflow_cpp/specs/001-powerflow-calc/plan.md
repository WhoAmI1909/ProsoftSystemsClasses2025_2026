# Implementation Plan: Расчет установившегося режима электрической сети постоянного тока

**Branch**: `001-powerflow-calc` | **Date**: 2026-07-01 | **Spec**: [spec.md](./spec.md)

**Input**: Feature specification from `specs/001-powerflow-calc/spec.md`

**Note**: This template is filled in by the `/speckit.plan` command.

## Summary

Программа PowerFlowCalc выполняет расчет потокораспределения активной мощности
в электрической сети постоянного тока (DC power flow) с поддержкой островного
режима. На вход принимает JSON-файл с описанием узлов (slack/load/generator) и
ветвей (активные сопротивления), на выходе — ASCII-таблица с напряжениями,
потоками мощности и балансом через консоль и текстовый файл. Решение —
итерационный метод простой итерации на основе матрицы узловых проводимостей.

## Technical Context

**Language/Version**: C++17 (GCC ≥ 8, Clang ≥ 7, MSVC ≥ 19.14)

**Primary Dependencies**:
- Eigen 3.4+ (плотные матрицы, header-only, решение СЛАУ)
- nlohmann/json 3.11+ (header-only, парсинг и валидация JSON)
- Google Test (модульное тестирование)

**Storage**: Файловая система: чтение JSON-файла, запись report.txt (N/A для БД)

**Testing**: Google Test (предпочтительно) или Catch2, запуск через CTest

**Target Platform**: Linux (основная), macOS и Windows желательны (CMake кроссплатформенная сборка)

**Project Type**: CLI (консольное приложение)

**Performance Goals**: Расчет сети из 20 узлов < 1 секунды (без учета файлового I/O)

**Constraints**: Сеть ≤ 20 узлов, tolerance > 0, max_iterations > 0, связность проверяется по островкам

**Scale/Scope**: До 20 узлов в сети, до нескольких независимых островков

## Constitution Check

*GATE: Must pass before Phase 0 research. Re-check after Phase 1 design.*

| Принцип | Статус | Подтверждение |
|---------|--------|---------------|
| I. Modular Architecture | ✅ PASS | Конвейерная архитектура: DataLoader → NetworkBuilder → PowerFlowSolver → FlowCalculator → ReportGenerator. Каждый модуль с четким интерфейсом (заголовочный файл), независимо тестируем. |
| II. Modern C++ Standards | ✅ PASS | C++17, snake_case/PascalCase, умные указатели, STL, constexpr, исключения. |
| III. Correctness & Test-Driven Verification | ✅ PASS | План тестов: 2-3 узла с аналитической проверкой, баланс мощностей, граничные условия, островной режим. Тесты пишутся до/параллельно реализации. |
| IV. Domain Model Fidelity | ✅ PASS | Физическая модель DC без потерь, только продольные сопротивления, первый закон Кирхгофа. Итерационный метод простой итерации. Знаки мощностей: нагрузка - / генерация +. |
| V. Input/Output Contract & Observability | ✅ PASS | JSON-контракт с полной валидацией, ASCII-таблицы в консоль, текстовый файл отчета. Сообщения об ошибках на русском языке. |

**Gate Result**: PASS — все принципы соблюдены, нарушений нет.

## Project Structure

### Documentation (this feature)

```text
specs/001-powerflow-calc/
├── plan.md              # This file
├── research.md          # Phase 0 output
├── data-model.md        # Phase 1 output
├── quickstart.md        # Phase 1 output
├── contracts/           # Phase 1 output
│   └── cli-contract.md  # Контракт командной строки
└── tasks.md             # Phase 2 output (/speckit.tasks)
```

### Source Code (repository root)

```text
src/
├── main.cpp                 # Точка входа, парсинг аргументов CLI
├── data_loader.cpp          # Чтение и валидация JSON, выделение островков
├── network_builder.cpp      # Построение матрицы проводимостей Y для островка
├── power_flow_solver.cpp    # Итерационный решатель (простая итерация)
├── flow_calculator.cpp      # Расчет потоков мощности по ветвям
├── report_generator.cpp     # Формирование ASCII-отчета (консоль + файл)
└── utils.cpp                # Вспомогательные функции (форматирование, BFS/DFS)

include/
├── data_loader.hpp
├── network_builder.hpp
├── power_flow_solver.hpp
├── flow_calculator.hpp
├── report_generator.hpp
├── utils.hpp
└── types.hpp                 # Общие структуры данных (Node, Branch, Island, etc.)

tests/
├── test_data_loader.cpp      # Тесты валидации JSON и выделения островков
├── test_network_builder.cpp  # Тесты построения матрицы Y
├── test_solver.cpp           # Тесты итерационного решателя
├── test_flow_calculator.cpp  # Тесты расчета потоков мощности
├── test_report_generator.cpp # Тесты генерации отчета
└── test_utils.cpp            # Тесты вспомогательных функций

data/
├── input.json                # Пример входных данных (по умолчанию)
├── test_2node.json           # Тест: 2 узла (slack + нагрузка)
├── test_3node.json           # Тест: 3 узла (slack + нагрузка + генератор)
├── test_parallel.json        # Тест: параллельные линии
├── test_two_islands.json     # Тест: два независимых островка
├── test_no_slack.json        # Ошибочный: островок без slack
├── test_double_slack.json    # Ошибочный: островок с двумя slack
└── test_unbalanced.json      # Тест: дисбаланс мощностей

CMakeLists.txt
README.md
```

**Structure Decision**: Single project (Option 1) — консольное CLI-приложение с
модульной структурой в `src/` и `include/`. Тесты в `tests/`, тестовые данные
в `data/`. Соответствует конституционной структуре проекта.

## Complexity Tracking

> Нет нарушений конституции, секция не требуется.
