# R-trees

[![CMake](https://github.com/Algorithms-and-Data-Structures-2021/semester-work-r-tree-deep-dark-fantasies/actions/workflows/cmake.yml/badge.svg)](https://github.com/Algorithms-and-Data-Structures-2021/semester-work-r-tree-deep-dark-fantasies/actions/workflows/cmake.yml)


- _Какая структура данных реализуется?_
- _Что она из себя представляет (сбалансированное дерево, линейный список и пр.)?_
- _Где и как она используется (приложения)?_
- _Какие операции над ней можно выполнять (поиск, удаление, добавление, вставка и пр.)?_
- _Какова теоретическая сложность операций (поиск за O(log(n)), вставка за O(n^2) и т.д.)?_
- _Какая-то другая справочная информация о проекте._

## Команда "♂Deep dark fantasies♂"

_Заполните таблицу с указанием вклада каждого из участников в проект._

Примечание. Преподаватель может определить вклад любого из участников команды по истории коммитов.

| Фамилия Имя    | Вклад (%) | Прозвище |
| :---           |   ---:    |  ---:    |
| Мингачев Рустам| 33,33     |  BOOBA   |
| Валитов Дамир  | 33,33     |  ABIBA   |
| Гордеев Илья   | 33,33     |  ABOBA   |

**Девиз команды**
> _ABIBA и ABOBA пришли получать зарплату..._

## Структура проекта

_Описание основных частей семестрового проекта._

Пример. Проект состоит из следующих частей:

- [src](src)/[include](include) - реализация структуры данных (исходный код и заголовочные файлы);
- [benchmark](benchmark) - контрольные тесты производительности структуры данных (операции добавления, удаления,
  поиска и пр.);
- [examples](examples) - примеры работы со структурой данных;
- [dataset](dataset) - наборы данных для запуска контрольных тестов и их генерация;

## Требования (Prerequisites)


Рекомендуемые требования:

1. С++ компилятор c поддержкой стандарта C++17 (например, _GNU GCC 8.1.x_ и выше).
2. Система автоматизации сборки _CMake_ (версия _3.12.x_ и выше).
3. Интерпретатор _Python_ (версия _3.7.x_ и выше).
4. Рекомендуемый объем оперативной памяти - не менее 8 ГБ.
5. Свободное дисковое пространство объемом ~ 5 ГБ (набор данных для контрольных тестов).

## Сборка и запуск

### Пример (Windows)

#### Сборка проекта


Склонируйте проект к себе на устройство через [Git for Windows](https://gitforwindows.org/) (либо используйте
возможности IDE):

```shell
git clone https://github.com/Algorithms-and-Data-Structures-2021/semester-work-r-tree-deep-dark-fantasies.git
```

Для ручной сборки проекта в терминале введите:

```shell
# переход в папку с проектом
cd C:\Users\username\asd-projects\semester-work-r-tree-deep-dark-fantasies

# создание папки для файлов сборки (чтобы не засорять папку с проектом) 
mkdir -p build && cd build 

# сборка проекта
cmake .. -DCMAKE_BUILD_TYPE=RelWithDebInfo && cmake --config RelWithDebInfo --build . 
```

#### Генерация тестовых данных

Генерация тестового набора данных в
формате [comma-seperated values (CSV)](https://en.wikipedia.org/wiki/Comma-separated_values):

```shell
# переход в папку генерации набора данных
cd dataset

# запуск Python-скрипта
python generate_csv_bench_dataset.py data/insert/<имя папки>
```

- `output` - выходной файл и т.д.

Тестовые данные представлены в CSV формате (см.
[`dataset/data/dataset-example.csv`](dataset/data/dataset-example.csv)):

## Источники
* https://github.com/th-10/R-Tree-cpp
* https://github.com/maja42/rtree


_**Это не плагиат, это уважение чужого труда и помощь своим сокурсникам более подробно разобраться в теме.**_
