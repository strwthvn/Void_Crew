# Void Crew

Кооперативный хоррор-симулятор с системой ГеймМастера. Команда игроков управляет космическим кораблём в процедурно генерируемой вселенной. PSX-эстетика, глубокие ролевые системы, асимметрия знаний между членами экипажа.

## Требования

- **CMake** 3.25+
- **vcpkg** (manifest mode)
- **Компилятор C++20:**
  - Windows: MSVC (Visual Studio 2022+)
  - Linux: GCC 13+ или Clang 16+
  - macOS: Apple Clang (Xcode 15+)
- **Vulkan SDK** 1.3+

## Сборка

```bash
# Клонирование
git clone <repo-url>
cd Void_Crew

# Конфигурация (указать путь к vcpkg toolchain)
cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=<vcpkg-root>/scripts/buildsystems/vcpkg.cmake

# Сборка
cmake --build build --config Debug
```

### Конфигурации

| Конфигурация | Назначение |
|---|---|
| `Debug` | Полная отладочная информация, проверки, sanitizers |
| `Release` | Оптимизации, без отладки |
| `RelWithDebInfo` | Оптимизации + символы для профилирования |

### Цели сборки

| Цель | Описание |
|---|---|
| `server` | Серверный executable — вся игровая логика |
| `client` | Клиентский executable — рендеринг, звук, ввод |
| `common` | Общая статическая библиотека |
| `tests` | Модульные и интеграционные тесты |

```bash
# Собрать конкретную цель
cmake --build build --config Debug --target server
```

## Тесты

```bash
# Запуск всех тестов
ctest --test-dir build --output-on-failure

# Запуск конкретного теста
ctest --test-dir build -R <test_name> --output-on-failure
```

## Качество кода

```bash
# Форматирование
clang-format -i src/**/*.cpp src/**/*.hpp

# Статический анализ
clang-tidy src/**/*.cpp
```

Конфигурация форматирования — `.clang-format`, статического анализа — `.clang-tidy`.

## Структура проекта

```
Void_Crew/
├── src/
│   ├── server/         # Сервер (игровая логика, ECS, сеть, AI, физика)
│   ├── client/         # Клиент (рендеринг, аудио, ввод, UI)
│   └── common/         # Общий код (компоненты, протокол, утилиты)
├── include/            # Публичные заголовки
├── libs/               # Сторонние библиотеки (не в vcpkg)
├── tests/              # Тесты (Catch2)
├── tools/              # Инструменты разработки
└── docs/               # Проектная документация
```

## Документация

- [Концепт-документ](docs/01-first_iteration.md)
- [Сетевая архитектура](docs/02-network_pattern.md)
- [Технологический стек](docs/03-technology_stack.md)
- [Библиотеки](docs/04-libraries.md)
- [Соглашения по коду](docs/05-conventions.md)
- [Best practices](docs/06-best_practices.md)
- [Roadmap](docs/07-roadmap.md)
