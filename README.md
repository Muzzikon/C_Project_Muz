# C_Project_Muz

## О проекте

`C_Project_Muz` — учебный проект на языке **C** по теме **пространственных структур данных**.

В основе проекта лежит **3D k-d tree** для работы с наборами точек из CSV.  
Проект ориентирован на задачи, близкие к обработке пространственных данных и робототехнике.

Реализованы:
- загрузка 3D-точек из CSV;
- построение 3D `KD-Tree`;
- вставка новой точки;
- поиск ближайшего соседа;
- диапазонный поиск;
- сравнение `KD-Tree` и `Brute force` по корректности и времени;
- кластеризация `DBSCAN`;
- сохранение результатов в отдельные CSV-файлы.

На текущем этапе режим `-cmeans` распознаётся в CLI, но алгоритм **ещё не реализован**.

---

## Что уже реализовано

В проекте есть:

- загрузка точек из CSV в память;
- полноценное **3D KD-дерево**;
- режимы:
  - `-kd_insert`
  - `-kd_nearest`
  - `-kd_range`
  - `-dbscan`
- проверка корректности `nearest` и `range` через **brute force**;
- сравнение времени работы `KD-Tree` и `Brute force`;
- `DBSCAN` для 3D-точек;
- ускорение `DBSCAN` через **пространственную сетку (`grid index`)**;
- сохранение результатов `range query`, `DBSCAN` и вставки в отдельные CSV-файлы;
- 2D-визуализация результата `DBSCAN` через Python-скрипт.

---

## Структура проекта

```text
C_Project_Muz/
├── include/
│   ├── bruteforce_utils.h
│   ├── cli_handlers.h
│   ├── dbscan.h
│   ├── grid_index.h
│   ├── io_utils.h
│   └── kd_tree.h
├── src/
│   ├── bruteforce_utils.c
│   ├── cli_handlers.c
│   ├── dbscan.c
│   ├── grid_index.c
│   ├── io_utils.c
│   ├── kd_tree.c
│   └── main.c
├── scripts/
│   ├── plot_dbscan_2D.py
│   ├── plot_nearest_2D.py
│   ├── plot_points_2D.py
│   └── plot_range_2D.py
├── Tests/
│   ├── Circle_xyz.csv
│   ├── Random1_xyz.csv
│   ├── Random2_xyz.csv
│   ├── Still_xyz.csv
│   ├── Zigzag_xyz.csv
│   └── дополнительные исходные *_cloud.csv
├── README.md
└── .gitignore
```

---

## Архитектура

### `src/main.c`

Главный файл программы.

Отвечает за:
- проверку аргументов командной строки;
- загрузку массива точек;
- построение `KD-Tree`;
- выбор нужного CLI-режима;
- освобождение памяти перед завершением программы.

### `src/kd_tree.c` / `include/kd_tree.h`

Модуль 3D `KD-Tree`.

Реализованы:
- `insert`
- `nearest_neighbor`
- `range_query`
- `range_query_indices`
- `free_tree`

Узел дерева хранит:
- точку `Point`;
- индекс точки в исходном массиве.

### `src/dbscan.c` / `include/dbscan.h`

Модуль `DBSCAN`.

Алгоритм работает **по массиву точек**, без зависимости от `KD-Tree`.

Реализованы:
- `dbscan(...)`
- `free_dbscan_result(...)`
- `print_dbscan_summary(...)`
- `save_dbscan_result_csv(...)`

### `src/grid_index.c` / `include/grid_index.h`

Модуль пространственной сетки.

Используется для ускорения поиска соседей внутри `DBSCAN`.

### `src/io_utils.c` / `include/io_utils.h`

Модуль ввода-вывода и разбора точек.

Содержит:
- загрузку точек из CSV;
- разбор точки из CLI;
- генерацию имён выходных файлов;
- сохранение точек в CSV.

### `src/bruteforce_utils.c` / `include/bruteforce_utils.h`

Модуль эталонных brute force-проверок.

Содержит:
- brute force nearest neighbor;
- brute force range query;
- сортировку точек;
- сравнение результатов `KD-Tree` и `Brute force`.

### `src/cli_handlers.c` / `include/cli_handlers.h`

Модуль обработчиков CLI-команд.

Содержит обработку режимов:
- `-kd_insert`
- `-kd_nearest`
- `-kd_range`
- `-dbscan`

Здесь находится логика пользовательских команд, вывода результатов и сохранения выходных CSV-файлов.

---

## Сборка проекта

Для сборки используется `gcc`.

```bash
gcc -Wall -Wextra -Werror -std=c11 src/main.c src/kd_tree.c src/dbscan.c src/grid_index.c src/io_utils.c src/bruteforce_utils.c src/cli_handlers.c -Iinclude -lm -o robot_spatial
```

После успешной сборки появится исполняемый файл:

```bash
./robot_spatial
```

---

## Формат входных данных

Основной ожидаемый формат — CSV, где каждая строка содержит одну 3D-точку:

```text
x,y,z
```

Пример:

```text
0,0,0
1,2,3
-1.5,4.2,0.8
```

### Важно

Если исходный CSV содержит дополнительные столбцы, его нужно предварительно преобразовать к формату `x,y,z`.

---

## Поддерживаемые режимы

Для аргументов командной строки точки можно передавать как в формате `x,y,z`,
так и в формате `x,y`. Во втором случае координата `z` автоматически считается равной `0`.

### 1. Вставка новой точки

```bash
./robot_spatial <file.csv> -kd_insert x,y,z
```

Пример:

```bash
./robot_spatial Tests/Still_xyz.csv -kd_insert 1234.5,2345.5,3456.5
```

Что делает:
- загружает точки из CSV;
- строит `KD-Tree`;
- добавляет новую точку в массив и дерево;
- сохраняет обновлённый набор точек в отдельный CSV-файл.

Примеры имён выходных файлов:

```text
Results/CSV/Still_xyz_inserted.csv
Results/CSV/Circle_xyz_inserted.csv
Results/CSV/Random1_xyz_inserted.csv
```

Каталог `Results/CSV` создаётся автоматически при первом сохранении.
Если файл уже существует, создаётся новый с нумерацией.

---

### 2. Поиск ближайшего соседа

```bash
./robot_spatial <file.csv> -kd_nearest x,y,z
```

Пример:

```bash
./robot_spatial Tests/Random1_xyz.csv -kd_nearest 0,0,0
```

Что делает:
- ищет ближайшую точку через `KD-Tree`;
- ищет ближайшую точку через `Brute force`;
- сравнивает результаты;
- измеряет время работы обоих способов.

---

### 3. Диапазонный поиск

```bash
./robot_spatial <file.csv> -kd_range x1,y1,z1 x2,y2,z2
```

Пример:

```bash
./robot_spatial Tests/Zigzag_xyz.csv -kd_range -5,-5,-5 5,5,5
```

Что делает:
- выполняет диапазонный поиск через `KD-Tree`;
- выполняет такой же поиск через `Brute force`;
- сравнивает количество найденных точек;
- сортирует и сравнивает результаты;
- сохраняет найденные точки в отдельный CSV-файл.

Примеры имён выходных файлов:

```text
Results/CSV/Still_xyz_range.csv
Results/CSV/Circle_xyz_range.csv
Results/CSV/Random1_xyz_range.csv
```

Если файл уже существует, создаётся новый с нумерацией.

---

### 4. DBSCAN

```bash
./robot_spatial <file.csv> -dbscan <eps> <min_pts>
```

Пример:

```bash
./robot_spatial Tests/Random2_xyz.csv -dbscan 0.5 5
```

Что делает:
- запускает `DBSCAN` для 3D-точек;
- ищет соседей через `grid index`;
- выводит число кластеров;
- выводит число шумовых точек;
- измеряет время работы;
- сохраняет результат в CSV.

Формат выходного CSV:

```text
x,y,z,cluster
```

Примеры имён выходных файлов:

```text
Results/CSV/Still_xyz_dbscan.csv
Results/CSV/Random2_xyz_dbscan.csv
Results/CSV/Circle_xyz_dbscan.csv
```

Если такой файл уже существует, создаётся новый с нумерацией.

---

### 5. Fuzzy C-means

```bash
./robot_spatial <file.csv> -cmeans ...
```

На текущем этапе алгоритм **не реализован**.

Режим распознаётся, но выводит сообщение о том, что реализация будет добавлена позже.

---

## Проверенные сценарии

Основные тестовые CSV для проверки сценариев находятся в папке `Tests/`.
Часть более крупных или вспомогательных файлов может использоваться только локально.

- `Tests/Circle_xyz.csv`
- `Tests/Random1_xyz.csv`
- `Tests/Random2_xyz.csv`
- `Tests/Still_xyz.csv`
- `Tests/Zigzag_xyz.csv`

Для этих файлов были проверены режимы:
- `-kd_nearest`
- `-kd_range`
- `-dbscan`
- `-kd_insert`

Тестовые CSV использовались локально при проверке и не обязательно входят в состав самого репозитория.

Программа принимает либо полный путь к CSV-файлу, либо короткое имя файла.
Если передано только имя без пути, программа сначала ищет файл по указанному имени,
а затем автоматически пробует открыть его из папки `Tests/`.
---

## Подтверждённые результаты

### `KD-Tree`: nearest neighbor

Для каждого из пяти файлов в качестве точки-запроса использовалась первая точка из файла.

Во всех случаях:
- `KD-Tree` и `Brute force` вернули одну и ту же точку;
- программа отработала корректно;
- `KD-Tree` оказался быстрее `Brute force`.

Это подтверждает корректность поиска ближайшего соседа.

### `KD-Tree`: range query

Для каждого из пяти файлов использовался диапазон вида:

```text
[q] ... [q]
```

где `q` — первая точка из файла.

Полученные результаты:

| Файл | Найдено точек |
|------|--------------:|
| Circle_xyz.csv | 1 |
| Random1_xyz.csv | 3 |
| Random2_xyz.csv | 6 |
| Still_xyz.csv | 86 |
| Zigzag_xyz.csv | 1 |

Во всех случаях:
- количество точек у `KD-Tree` и `Brute force` совпало;
- после сортировки результаты совпали полностью;
- результат диапазонного поиска был сохранён в отдельный CSV-файл.

Это подтверждает корректность `range query`, включая случаи с дубликатами точек.

### `DBSCAN`

Для всех пяти файлов использовались параметры:

```text
eps = 0.5
min_pts = 5
```

Полученные результаты:

| Файл | Точек | Кластеров | Шумовых точек | Время |
|------|------:|----------:|--------------:|------:|
| Circle_xyz.csv | 38341 | 9 | 88 | 1.627666 сек |
| Random1_xyz.csv | 105107 | 6 | 76 | 25.145795 сек |
| Random2_xyz.csv | 65366 | 9 | 70 | 8.518997 сек |
| Still_xyz.csv | 2082 | 4 | 25 | 0.009583 сек |
| Zigzag_xyz.csv | 80312 | 10 | 58 | 11.177204 сек |

Во всех случаях:
- алгоритм завершился без ошибок;
- результат был сохранён в CSV формата `x,y,z,cluster`.

### `KD-Tree`: insert

Для проверки вставки использовалась точка:

```text
1234.5,2345.5,3456.5
```

Для всех пяти файлов:
- число точек после вставки увеличилось ровно на 1;
- был создан отдельный `*_inserted.csv`;
- повторный запуск `-kd_nearest` на новом файле возвращал именно вставленную точку;
- `KD-Tree` и `Brute force` снова совпадали полностью.

Это подтверждает корректность вставки в массив точек и `KD-Tree`.

---

## 2D-визуализации

В проекте есть несколько Python-скриптов для построения 2D-визуализаций по осям X-Y.

### 1. Исходные точки

```bash
python3 scripts/plot_points_2D.py Tests/Still_xyz.csv
```

Пример результата:

```bash
Results/PNG/Still_xyz_points_2D.png
```

### 2. Ближайший сосед

```bash
python3 scripts/plot_nearest_2D.py Tests/Still_xyz.csv 0,0,0
```

Пример результата:

```bash
Results/PNG/Still_xyz_nearest_2D.png
```

### 3. Range query

```bash
python3 scripts/plot_range_2D.py Tests/Still_xyz.csv Results/CSV/Still_xyz_range.csv
```

Пример результата:

```bash
Results/PNG/Still_xyz_range_2D.png
```

### 4. DBSCAN

```bash
python3 scripts/plot_dbscan_2D.py Results/CSV/Still_xyz_dbscan.csv
```

Пример результата:

```bash
Results/PNG/Still_xyz_dbscan_2D.png
```

Каталог Results/PNG создаётся автоматически.
Если matplotlib не установлен, можно использовать виртуальное окружение:

```bash
python3 -m venv .venv
source .venv/bin/activate
pip install matplotlib
```

---

## Ограничения проекта

На текущем этапе есть несколько известных ограничений:

1. Основной входной формат — CSV вида `x,y,z`.
2. CSV с дополнительными столбцами нужно предварительно преобразовать.
3. `Fuzzy C-means` пока не реализован.
4. В репозитории могут отсутствовать крупные тестовые CSV-файлы, если они используются только локально.
5. На больших наборах данных `DBSCAN` работает заметно дольше, чем операции `nearest` и `range`.

---

## Что уже закрыто по обязательной части

На текущем этапе проект закрывает основные обязательные пункты по теме `k-d tree`:

- вставка в `KD-Tree`;
- поиск ближайшего соседа;
- диапазонный поиск;
- сравнение `KD-Tree` и `Brute force` по корректности;
- сравнение `KD-Tree` и `Brute force` по времени;
- `DBSCAN`;
- сохранение результатов в CSV;
- 2D-визуализации исходных точек, результата nearest neighbor, range query, кластеров `DBSCAN` и шумовых точек.

---

## Что ещё можно улучшить

В будущем в проект можно добавить:
- автоматический запуск визуализаций после выполнения CLI-команд;
- `Makefile` для сборки проекта одной командой;
- отдельный режим удаления точки из KD-Tree;
- более подробный отчёт по производительности на нескольких наборах данных;
- завершение реализации Fuzzy C-means.
---

## Примеры полного запуска

```bash
gcc -Wall -Wextra -Werror -std=c11 src/main.c src/kd_tree.c src/dbscan.c src/grid_index.c src/io_utils.c src/bruteforce_utils.c src/cli_handlers.c -Iinclude -lm -o robot_spatial

./robot_spatial Tests/Still_xyz.csv -dbscan 0.5 5
python3 scripts/plot_dbscan_2D.py Results/CSV/Still_xyz_dbscan.csv

./robot_spatial Tests/Still_xyz.csv -kd_range -5,-5,-5 5,5,5
python3 scripts/plot_range_2D.py Tests/Still_xyz.csv Results/CSV/Still_xyz_range.csv

python3 scripts/plot_points_2D.py Tests/Still_xyz.csv
python3 scripts/plot_nearest_2D.py Tests/Still_xyz.csv 0,0,0
```

---

## Автор

Проект выполнен в рамках учебной работы по курсу **«Императивное программирование»**.
