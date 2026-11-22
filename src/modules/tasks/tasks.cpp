#include "tasks.h"

#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <algorithm>
#include <esp_system.h>
#include <cmath>
#include <cstdlib>

namespace Tasks {

    namespace {
        std::vector<Task> taskList;
        std::vector<Task> tasks;
        
        // ссылка для github raw
        const char* TASKS_URL = 
            "";
        
            // преобразования выбора типа задания из json
        bool ParseTaskType(const String& value, TaskType& result) {
            if (value == "choice") {
                result = TaskType::Choice;
            }
            else if (value == "number") {
                result = TaskType::Number;
            }
            else if (value == "text") {
                result = TaskType::Text;
            }
            else {
                return false;
            }
            return true;
        }

            // преобразования сложности задания из json
        bool ParseDifficulty(const String& value, Difficulty& result) {
            if (value == "easy") {
                result = Difficulty::Easy;
            }
            else if (value == "medium") {
                result = Difficulty::Medium;
            }
            else if (value == "high") {
                result = Difficulty::High;
            }
            else {
                return false;
            }
            return true;
        }
        bool ready = false;

        // загрузка json
        bool LoadTasksFromServer() {
        if (WiFi.status() != WL_CONNECTED) {
            return false;
        }

        WiFiClientSecure client;
        // прототип!!!!
        client.setInsecure();

        HTTPClient http;

        if (!http.begin(client, TASKS_URL)) {
            return false;
        }

        int httpCode = http.GET();

        if (httpCode != HTTP_CODE_OK) {
            http.end();
            return false;
        }

        JsonDocument doc;

        DeserializationError error =
            deserializeJson(doc, http.getStream());

        http.end();

        if (error) {
            return false;
        }

        if (!doc["tasks"].is<JsonArray>()) {
            return false;
        }

        JsonArray tasksJson = doc["tasks"].as<JsonArray>();

        std::vector<Task> newTasks;

        for (JsonObject item : tasksJson) {
            Task task{};

            task.id = item["id"] | 0;

            if (!ParseTaskType(
                    item["type"] | "",
                    task.type
                )) {
                return false;
            }

            if (!ParseDifficulty(
                    item["difficulty"] | "",
                    task.difficulty
                )) {
                return false;
            }

            task.topic = item["topic"] | "";
            task.question = item["question"] | "";
            task.explanation = item["explanation"] | "";
            task.tolerance = item["tolerance"] | 0.0f;

            if (task.id == 0 || task.topic.isEmpty() ||
                task.question.isEmpty() || !item["answers"].is<JsonArray>()) {
                return false;
            }

            if (item["options"].is<JsonArray>()) {
                for (const char* option : item["options"].as<JsonArray>()) {
                    task.options.push_back(String(option));
                }
            }

            for (const char* answer : item["answers"].as<JsonArray>()) {
                task.answers.push_back(String(answer));
            }

            if (task.answers.empty()) {
                return false;
            }

            newTasks.push_back(task);
        }

        if (newTasks.empty()) {
            return false;
        }

        // проверка id
        for (size_t i = 0; i < newTasks.size(); i++) {
            for (size_t j = i + 1; j < newTasks.size(); j++) {
                if (newTasks[i].id == newTasks[j].id) {
                    return false;
                }
            }
        }

        taskList.swap(newTasks);

        return true;
        }
        
        // сообщает приложению, удалось ли получить базу
        bool Init() {
            taskList.clear();
            ready = false;

            if (!LoadTasksFromServer()) {
                return false;
            }

            ready = true;

            return true;
        }
        // даёт команду обращения к заданиям другим модулям
        bool IsReady() {
            return ready;
        }

        void Update() {
        }

        // возвращение количества заданий в векторе
        uint32_t GetTaskCount() {
            return taskList.size();
        }

        // если функция найдена - копируем в result и возвращаем true
        bool GetTaskById(uint32_t id, Task& result) {
            for (const Task& task : taskList) {
                if (task.id == id) {
                    result = task;
                    return true;
                }
            }

            return false;
        }

        // получение случайного индекса задачи (чтобы не было одних и тех же задач подряд)
        bool GetRandomTask(Task& result) {
            if (!ready || taskList.empty()) {
                return false;
            }

            if (tasks.empty()) {
                return false;
            }
            
            uint32_t index = random(0, static_cast<long>(tasks.size()));

            result = taskList[index];

            return true;
        }

        // возвращение вектоора с заданиями
        std::vector<Task> GetTasksByTopic(const String& topic) {
            std::vector<Task> result;

            for (const Task& task : taskList) {
                if (task.topic == topic) {
                    result.push_back(task);
                }
            }
            
            return result;
        }

        // поиск задач по сложности
        std::vector<Task> GetTasksByDifficulty(Difficulty difficulty) {
            std::vector<Task> result;

            for (const Task& task: taskList) {
                if (task.difficulty == difficulty) {
                    result.push_back(task);
                }
            }

            return result;
        }

        // поиск задач по типу
        std::vector<Task> GetTaskByType(TaskType type) {
            std::vector<Task> result;

            for (const Task& task : taskList) {
                if (task.type == type) {
                    result.push_back(task);
                }
            }

            return result;
        }

        // нормализация введённой строки
        String NormalizeAnswer(String value) {
            value.trim();
            value.toLowerCase();

            return value;
        }

        // проверка ответа
        bool CheckAnswer(const Task& task, const String& answer) {
            String userAnswer = NormalizeAnswer(answer);

            if (task.type == TaskType::Number) {
                char* endPtr = nullptr;

                float userValue = strtof(
                    userAnswer.c_str(),
                    &endPtr
                );

                if (endPtr == userAnswer.c_str() || *endPtr != '\0') {
                    return false;
                }

                for (const String& correct : task.answers) {
                    char* correctEnd = nullptr;

                    float correctValue = strtof(
                        correct.c_str(),
                        &correctEnd
                    );

                    if (correctEnd == correct.c_str() || *correctEnd != '\0') {
                        continue;
                    }

                    if (fabs(userValue - correctValue) <= task.tolerance) {
                        return true;
                    }
                }

                return false;
            }

            for (const String& correct : task.answers) {
                if (userAnswer == NormalizeAnswer(correct)) {
                    return true;
                }
            }

            return false;
        }
    }
}