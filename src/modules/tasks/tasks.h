#pragma once

#include <Arduino.h>
#include <vector>

namespace Tasks {

    enum class TaskType {
        Choice,
        Number,
        Text
    };

    enum class Difficulty {
        Easy,
        Medium,
        High
    };
    
    struct Task {
        uint32_t id;

        TaskType type;
        Difficulty difficulty;

        String topic;

        String question;
        std::vector<String> options;
        std::vector<String> answers;
        float tolerance;
        String explanation;
    };

    bool Init();
    void Update();

    bool IsReady();

    uint32_t GetTaskCount();

    bool GetTaskById(uint32_t id, Task& result);

    bool GetRandomTask(Task& result);

    std::vector<Task> GetTasksByTopic(const String& topic);
    std::vector<Task> GetTasksByDifficulty(Difficulty difficulty);
    std::vector<Task> GetTasksByType(TaskType type);

    bool CheckAnswer(const Task& task, const String& answer);
}