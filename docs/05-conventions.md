# Соглашения по коду

## Общие принципы

**Читаемость важнее краткости.** Код читают чаще, чем пишут. Явное лучше неявного. Если выбор между «умным» однострочником и понятным развёрнутым кодом — выбираем понятный.

**Консистентность важнее личных предпочтений.** Единый стиль по всей кодовой базе. Если соглашение принято — следуем ему, даже если лично предпочли бы иначе.

**Ошибки должны быть явными.** Никаких тихих сбоев. Если что-то пошло не так — это должно быть видно: лог, assert, исключение, код возврата.

**Минимизация состояния.** Меньше мутабельного состояния — меньше багов. Предпочитаем чистые функции, иммутабельные структуры, локальные переменные.

---

## Именование

### Общие правила

| Элемент | Стиль | Пример |
|---------|-------|--------|
| Классы и структуры | PascalCase | `PlayerCharacter`, `AtmosphereSystem` |
| Функции и методы | camelCase | `calculateDamage()`, `updatePosition()` |
| Переменные | camelCase | `playerHealth`, `currentTick` |
| Константы | SCREAMING_SNAKE_CASE | `MAX_PLAYERS`, `DEFAULT_TICK_RATE` |
| Перечисления (enum class) | PascalCase | `enum class DamageType { Blunt, Sharp, Thermal }` |
| Значения enum | PascalCase | `DamageType::Sharp` |
| Пространства имён | lowercase | `namespace atmosphere`, `namespace net` |
| Файлы | snake_case | `player_character.cpp`, `atmosphere_system.hpp` |
| Приватные поля класса | camelCase с префиксом m_ | `m_health`, `m_position` |
| Статические поля | camelCase с префиксом s_ | `s_instanceCount` |
| Глобальные переменные | camelCase с префиксом g_ | `g_gameConfig` (избегать) |
| Параметры шаблонов | PascalCase | `template<typename EntityType>` |

### Именование по смыслу

**Булевы переменные** — вопросительная форма или префикс:
```
isAlive, hasWeapon, canMove, shouldUpdate
wasProcessed, willExpire
```

**Функции-предикаты** — возвращают bool, именуются как вопрос:
```
isEmpty(), hasComponent(), canPerformAction()
```

**Функции-действия** — глагол в начале:
```
calculateDamage(), updateState(), sendPacket()
spawnEntity(), destroyComponent(), processInput()
```

**Функции-геттеры** — без префикса get, если нет двусмысленности:
```
health()        // вместо getHealth()
position()      // вместо getPosition()
// Но: getOrCreate(), getDefaultValue() — когда есть побочные эффекты или параметры
```

**Функции-сеттеры** — префикс set:
```
setHealth(), setPosition()
```

**Фабричные функции** — create или make:
```
createEntity(), makePacket()
```

### Аббревиатуры

Аббревиатуры в именах — как обычные слова:
```
HttpConnection  // не HTTPConnection
JsonParser      // не JSONParser
entityId        // не entityID
```

Исключение — двухбуквенные аббревиатуры:
```
IOManager, UIElement
```

### Запрещённые практики

- Однобуквенные имена кроме счётчиков циклов (i, j, k) и лямбд
- Венгерская нотация (strName, nCount)
- Имена, отличающиеся только регистром
- Транслит (igrokZdorovie)
- Избыточные суффиксы (PlayerManager, DataInfo, SystemController)

---

## Структура файлов

### Заголовочные файлы (.hpp)

```
#pragma once

// Системные заголовки
#include <vector>
#include <string>

// Внешние библиотеки
#include <entt/entt.hpp>
#include <glm/glm.hpp>

// Заголовки проекта
#include "core/types.hpp"
#include "ecs/component.hpp"

namespace game::atmosphere {

// Предварительные объявления
class Compartment;
struct GasComposition;

// Константы
constexpr float DEFAULT_PRESSURE = 101.325f;

// Перечисления
enum class GasType { Oxygen, Nitrogen, CarbonDioxide, Toxin };

// Классы
class AtmosphereSystem {
public:
    // Типы
    using CompartmentId = uint32_t;
    
    // Конструкторы и деструктор
    AtmosphereSystem();
    ~AtmosphereSystem();
    
    // Запрет копирования (если нужен)
    AtmosphereSystem(const AtmosphereSystem&) = delete;
    AtmosphereSystem& operator=(const AtmosphereSystem&) = delete;
    
    // Публичные методы
    void update(float deltaTime);
    float pressure(CompartmentId id) const;
    
private:
    // Приватные методы
    void recalculateFlow();
    
    // Поля
    std::vector<Compartment> m_compartments;
    float m_lastUpdateTime;
};

} // namespace game::atmosphere
```

### Файлы реализации (.cpp)

```
#include "atmosphere/atmosphere_system.hpp"

// Дополнительные заголовки, нужные только для реализации
#include "atmosphere/gas_math.hpp"
#include "core/logging.hpp"

namespace game::atmosphere {

// Анонимное пространство имён для локальных функций
namespace {

float calculateDiffusionRate(float pressureDiff, float area) {
    // ...
}

} // namespace

AtmosphereSystem::AtmosphereSystem()
    : m_lastUpdateTime(0.0f)
{
    // ...
}

void AtmosphereSystem::update(float deltaTime) {
    // ...
}

} // namespace game::atmosphere
```

### Порядок include

1. Соответствующий заголовок (для .cpp файла)
2. Пустая строка
3. Системные заголовки C++ (`<vector>`, `<string>`)
4. Заголовки C (`<cstdint>`, `<cmath>`)
5. Пустая строка
6. Внешние библиотеки
7. Пустая строка
8. Заголовки проекта

Внутри каждой группы — алфавитный порядок.

---

## Форматирование

### Отступы и скобки

- Отступ: 4 пробела, не табы
- Открывающая скобка на той же строке (K&R стиль)
- Всегда скобки для блоков, даже однострочных

```
if (condition) {
    doSomething();
}

for (int i = 0; i < count; ++i) {
    process(i);
}

void function() {
    // тело
}

class MyClass {
public:
    void method();
};
```

### Длина строки

- Мягкий лимит: 100 символов
- Жёсткий лимит: 120 символов
- Перенос параметров — каждый на новой строке с дополнительным отступом

```
void processComplexOperation(
    EntityId entityId,
    const Transform& transform,
    const ComponentMask& requiredComponents,
    std::function<void(Entity&)> callback
) {
    // ...
}
```

### Пробелы

```
// Вокруг бинарных операторов
int result = a + b * c;
bool check = x == y && z != w;

// После запятых
function(arg1, arg2, arg3);

// После ключевых слов
if (condition)
for (auto& item : collection)
while (running)

// Нет пробела после имени функции
doSomething();  // правильно
doSomething (); // неправильно

// Нет пробелов внутри скобок
function(arg);    // правильно
function( arg );  // неправильно
```

### Вертикальные отступы

- Один пустой строка между функциями
- Один пустой строка между логическими блоками внутри функции
- Два пустых строки между секциями класса (public/private)
- Нет пустых строк в начале и конце блоков

---

## Паттерны и идиомы

### RAII

Ресурсы управляются через время жизни объекта. Никаких ручных new/delete, malloc/free.

```
// Плохо
Texture* texture = new Texture(path);
// ... использование ...
delete texture;

// Хорошо
auto texture = std::make_unique<Texture>(path);
// автоматически освободится

// Ещё лучше — на стеке, если возможно
Texture texture(path);
```

### Умные указатели

| Тип | Использование |
|-----|---------------|
| `std::unique_ptr` | Единоличное владение, основной выбор |
| `std::shared_ptr` | Разделяемое владение, только когда необходимо |
| `std::weak_ptr` | Наблюдение без владения, разрыв циклов |
| Сырой указатель | Только для не-владеющих ссылок, nullable |
| Ссылка | Не-владеющая, не-nullable, предпочтительнее указателя |

```
class EntityManager {
public:
    // Возвращает владеющий указатель — вызывающий получает владение
    std::unique_ptr<Entity> createEntity();
    
    // Принимает владение
    void addEntity(std::unique_ptr<Entity> entity);
    
    // Наблюдение без владения — сырой указатель
    Entity* findEntity(EntityId id);
    
    // Гарантированно существует — ссылка
    Entity& getEntity(EntityId id);
    
private:
    std::vector<std::unique_ptr<Entity>> m_entities;
};
```

### Передача параметров

| Ситуация | Способ |
|----------|--------|
| Чтение примитива | По значению: `void f(int x)` |
| Чтение объекта | По const-ссылке: `void f(const std::string& s)` |
| Модификация | По ссылке: `void f(std::string& s)` |
| Передача владения | По значению unique_ptr: `void f(std::unique_ptr<T> p)` |
| Опциональный параметр | Указатель или std::optional |
| Sink-параметр (сохраняется) | По значению + move: `void f(std::string s)` |

### Возврат значений

```
// Возврат по значению — компилятор оптимизирует (RVO/NRVO)
std::vector<Entity> getEntities();
std::string formatMessage();

// Не используем выходные параметры для возврата
// Плохо:
void getPosition(float& outX, float& outY);

// Хорошо:
glm::vec2 getPosition();
std::pair<float, float> getPosition();
```

### Error handling

**Исключения** — для исключительных ситуаций, которые не должны происходить при нормальной работе:
- Ошибки инициализации
- Нехватка памяти
- Нарушение инвариантов

**Коды возврата / std::optional / std::expected** — для ожидаемых неудач:
- Файл не найден
- Сущность не существует
- Парсинг невалидных данных

```
// Исключение — критическая ошибка
void loadConfig(const std::string& path) {
    if (!fileExists(path)) {
        throw std::runtime_error("Config file not found: " + path);
    }
}

// Optional — нормальная ситуация «не найдено»
std::optional<Entity*> findEntity(EntityId id) {
    auto it = m_entities.find(id);
    if (it == m_entities.end()) {
        return std::nullopt;
    }
    return it->second.get();
}

// Expected — операция, которая может не удаться с причиной
std::expected<ParsedData, ParseError> parsePacket(std::span<uint8_t> data);
```

### Enum class

Всегда `enum class`, никогда `enum`:

```
// Плохо
enum DamageType { Blunt, Sharp };
// Sharp доступен в глобальной области

// Хорошо
enum class DamageType { Blunt, Sharp };
// Требуется DamageType::Sharp
```

### Константы

```
// Compile-time константы
constexpr float PI = 3.14159f;
constexpr int MAX_PLAYERS = 12;

// Не используем #define для констант
#define MAX_PLAYERS 12  // плохо

// Не используем магические числа
float damage = health * 0.15f;  // плохо
float damage = health * POISON_DAMAGE_PERCENT;  // хорошо
```

### Циклы

```
// Range-based for — предпочтительно
for (const auto& entity : entities) {
    process(entity);
}

// С модификацией
for (auto& entity : entities) {
    entity.update();
}

// Индексный цикл — когда нужен индекс
for (size_t i = 0; i < entities.size(); ++i) {
    processWithIndex(entities[i], i);
}

// Алгоритмы — когда подходят
auto it = std::find_if(entities.begin(), entities.end(), 
    [](const Entity& e) { return e.isActive(); });
```

---

## Специфика проекта

### ECS-компоненты

Компоненты — только данные, никакой логики:

```
// Хорошо
struct HealthComponent {
    float current;
    float maximum;
    float regeneration;
};

// Плохо — логика в компоненте
struct HealthComponent {
    float current;
    float maximum;
    
    void takeDamage(float amount) {  // не должно быть здесь
        current -= amount;
    }
};
```

Системы обрабатывают компоненты:

```
class HealthSystem {
public:
    void update(entt::registry& registry, float deltaTime) {
        auto view = registry.view<HealthComponent>();
        for (auto [entity, health] : view.each()) {
            health.current = std::min(
                health.current + health.regeneration * deltaTime,
                health.maximum
            );
        }
    }
    
    void applyDamage(entt::registry& registry, entt::entity target, float amount) {
        if (auto* health = registry.try_get<HealthComponent>(target)) {
            health->current -= amount;
            if (health->current <= 0) {
                registry.emplace_or_replace<DeadTag>(target);
            }
        }
    }
};
```

### Сетевые сообщения

Все сообщения описываются в FlatBuffers-схемах:

```
// messages.fbs
namespace game.net;

enum MessageType : uint8 {
    PlayerInput = 1,
    WorldSnapshot = 2,
    EntitySpawn = 3,
}

table PlayerInput {
    tick: uint32;
    movement: Vec2;
    actions: [ActionType];
}

table WorldSnapshot {
    tick: uint32;
    entities: [EntityState];
}
```

Обработчики сообщений — отдельные функции:

```
class MessageHandler {
public:
    void handle(const PlayerInput& input, ClientId sender);
    void handle(const WorldSnapshot& snapshot);
    
private:
    void validateInput(const PlayerInput& input);
};
```

### Серверная валидация

Любые данные от клиента — недоверенные:

```
void handlePlayerInput(const PlayerInput& input, ClientId sender) {
    // Проверяем, что клиент вообще подключён
    auto* client = m_clients.find(sender);
    if (!client) {
        LOG_WARN("Input from unknown client: {}", sender);
        return;
    }
    
    // Проверяем tick — не из будущего, не слишком старый
    if (input.tick > m_currentTick + 1 || input.tick < m_currentTick - MAX_INPUT_LAG) {
        LOG_WARN("Invalid tick {} from client {}", input.tick, sender);
        return;
    }
    
    // Проверяем разумность значений
    if (glm::length(input.movement) > MAX_MOVEMENT_SPEED) {
        LOG_WARN("Suspicious movement speed from client {}", sender);
        input.movement = glm::normalize(input.movement) * MAX_MOVEMENT_SPEED;
    }
    
    // Только теперь применяем
    applyInput(client->entityId, input);
}
```

### Логирование

Уровни логов:

| Уровень | Использование |
|---------|---------------|
| TRACE | Детальная отладка, отключена в релизе |
| DEBUG | Отладочная информация |
| INFO | Значимые события: подключения, загрузка уровня |
| WARN | Подозрительное, но не критичное |
| ERROR | Ошибка, требующая внимания |
| CRITICAL | Фатальная ошибка, приложение не может продолжать |

```
LOG_TRACE("Processing entity {}", entityId);
LOG_DEBUG("Client {} connected from {}", clientId, address);
LOG_INFO("Game started with {} players", playerCount);
LOG_WARN("Client {} has high latency: {}ms", clientId, latency);
LOG_ERROR("Failed to load asset: {}", path);
LOG_CRITICAL("Out of memory, shutting down");
```

Контекст — что, где, почему:

```
// Плохо
LOG_ERROR("Failed");

// Хорошо
LOG_ERROR("Failed to load texture '{}': {}", path, error.message());
```

---

## Чеклист код-ревью

### Корректность

- [ ] Код делает то, что заявлено в описании PR
- [ ] Обработаны все краевые случаи
- [ ] Нет утечек памяти и ресурсов
- [ ] Потокобезопасность соблюдена (если применимо)
- [ ] Нет неопределённого поведения (UB)

### Безопасность (для серверного кода)

- [ ] Все данные от клиента валидируются
- [ ] Нет доверия к индексам и размерам из сети
- [ ] Защита от переполнения буферов
- [ ] Нет раскрытия серверного состояния клиенту сверх необходимого

### Производительность

- [ ] Нет лишних аллокаций в горячих путях
- [ ] Нет копирования больших структур (используются ссылки/move)
- [ ] Алгоритмическая сложность адекватна задаче
- [ ] Нет блокирующих операций в игровом цикле

### Стиль и читаемость

- [ ] Именование соответствует соглашениям
- [ ] Форматирование консистентно
- [ ] Комментарии объясняют «почему», а не «что»
- [ ] Нет закомментированного кода
- [ ] Нет TODO без номера задачи

### Архитектура

- [ ] Изменения в правильном месте (сервер/клиент/общий код)
- [ ] Нет нарушения разделения ответственности
- [ ] Зависимости направлены правильно (нет циклических)
- [ ] Компоненты не содержат логики

### Тесты

- [ ] Новый код покрыт тестами
- [ ] Существующие тесты не сломаны
- [ ] Тесты проверяют поведение, а не реализацию

### Документация

- [ ] Публичный API задокументирован
- [ ] Сложная логика объяснена
- [ ] README обновлён (если нужно)

---

## Комментарии и документация

### Когда комментировать

**Комментируем:**
- Почему выбрано это решение (особенно если неочевидно)
- Хаки и обходные пути с объяснением
- Сложные алгоритмы — краткое описание идеи
- Публичные API — что делает, параметры, возврат

**Не комментируем:**
- Что делает код (должно быть понятно из кода)
- Очевидные вещи
- Изменения для будущих поколений (используем git)

```
// Плохо — описывает что
// Увеличиваем счётчик
counter++;

// Хорошо — объясняет почему
// Компенсируем потерю точности при малых deltaTime
// См. https://gafferongames.com/post/fix_your_timestep/
accumulator += std::min(deltaTime, MAX_FRAME_TIME);
```

### Документация API

```
/// Применяет урон к сущности с учётом брони и модификаторов.
/// 
/// @param registry Реестр сущностей
/// @param target Целевая сущность (должна иметь HealthComponent)
/// @param damage Базовый урон до применения модификаторов
/// @param type Тип урона для расчёта сопротивлений
/// @return Фактически нанесённый урон после всех модификаторов
/// @throws std::invalid_argument если target не имеет HealthComponent
float applyDamage(
    entt::registry& registry,
    entt::entity target,
    float damage,
    DamageType type
);
```

### TODO и FIXME

```
// TODO(#123): Добавить поддержку нескольких типов брони
// FIXME(#456): Временный хак, убрать после рефакторинга инвентаря
// HACK: Обход бага в библиотеке X, см. issue upstream/repo#789
```

Всегда с номером задачи в трекере. Без номера — не принимается в main.

---

## Инструменты

### Форматирование

**clang-format** с конфигурацией в `.clang-format`:

```yaml
BasedOnStyle: LLVM
IndentWidth: 4
ColumnLimit: 120
BreakBeforeBraces: Attach
AllowShortFunctionsOnASingleLine: Empty
AllowShortIfStatementsOnASingleLine: false
AllowShortLoopsOnASingleLine: false
```

Запуск перед коммитом или через pre-commit hook.

### Статический анализ

**clang-tidy** с включёнными проверками:
- modernize-* (использование современного C++)
- bugprone-* (типичные ошибки)
- performance-* (проблемы производительности)
- readability-* (читаемость)

CI блокирует мерж при наличии предупреждений.

### Sanitizers

В CI запускаются тесты с:
- AddressSanitizer (память)
- UndefinedBehaviorSanitizer (UB)
- ThreadSanitizer (гонки данных) — для многопоточного кода

---

## Git workflow

### Ветки

- `main` — стабильная версия, всегда собирается и проходит тесты
- `develop` — интеграционная ветка для фич
- `feature/XXX-description` — фичи (XXX — номер задачи)
- `fix/XXX-description` — исправления
- `refactor/description` — рефакторинг без изменения поведения

### Коммиты

Формат сообщения:
```
type(scope): краткое описание

Подробное описание, если нужно.

Refs #123
```

Типы:
- `feat` — новая функциональность
- `fix` — исправление бага
- `refactor` — рефакторинг
- `docs` — документация
- `test` — тесты
- `chore` — сборка, CI, зависимости

```
feat(atmosphere): добавлена симуляция токсичных газов

Токсины распространяются через вентиляцию и наносят урон
персонажам без защиты. Скорость распространения зависит
от давления и проходимости.

Refs #234
```

### Pull Request

- Один PR — одна логическая задача
- Название: `[XXX] Краткое описание`
- Описание: что сделано, как тестировать, скриншоты если UI
- Ревью минимум от одного человека
- CI должен быть зелёным
- Мерж через squash для чистой истории
