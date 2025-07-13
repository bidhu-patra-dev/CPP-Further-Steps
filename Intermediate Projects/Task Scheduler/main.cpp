#include <iostream>
#include <vector>
#include <algorithm>
#include <queue>

using namespace std;

class Task {
public:
    string name;
    int burst_time;
    int priority;
    int remaining_time;
    int start_time;
    int finish_time;

    Task(string name, int burst_time, int priority)
        : name(name), burst_time(burst_time), priority(priority), remaining_time(burst_time), start_time(-1), finish_time(-1) {}
};

class Scheduler {
private:
    vector<Task> tasks;

public:
    void get_user_tasks() {
        int num_tasks;
        cout << "Enter the number of tasks: ";
        cin >> num_tasks;

        for (int i = 0; i < num_tasks; ++i) {
            string name;
            int burst_time, priority;
            cout << "Enter name for Task " << i + 1 << ": ";
            cin >> name;
            cout << "Enter burst time for " << name << ": ";
            cin >> burst_time;
            cout << "Enter priority for " << name << " (lower number = higher priority): ";
            cin >> priority;
            tasks.emplace_back(name, burst_time, priority);
        }
    }

    void reset_tasks() {
        for (auto& task : tasks) {
            task.remaining_time = task.burst_time;
            task.start_time = -1;
            task.finish_time = -1;
        }
    }

    void fcfs() {
        cout << "\nRunning First-Come, First-Serve (FCFS) Scheduling:" << endl;
        int current_time = 0;

        for (auto& task : tasks) {
            task.start_time = current_time;
            current_time += task.burst_time;
            task.finish_time = current_time;
            cout << task.name << ": start at " << task.start_time << ", finish at " << task.finish_time << endl;
        }
    }

    void round_robin(int quantum) {
        cout << "\nRunning Round Robin Scheduling:" << endl;
        queue<Task*> task_queue;
        for (auto& task : tasks) task_queue.push(&task);
        int current_time = 0;

        while (!task_queue.empty()) {
            Task* task = task_queue.front();
            task_queue.pop();

            if (task->start_time == -1)
                task->start_time = current_time;

            if (task->remaining_time > quantum) {
                current_time += quantum;
                task->remaining_time -= quantum;
                task_queue.push(task);
            } else {
                current_time += task->remaining_time;
                task->remaining_time = 0;
                task->finish_time = current_time;
                cout << task->name << ": start at " << task->start_time << ", finish at " << task->finish_time << endl;
            }
        }
    }

    void priority_scheduling() {
        cout << "\nRunning Priority Scheduling:" << endl;
        sort(tasks.begin(), tasks.end(), [](const Task& a, const Task& b) {
            return a.priority < b.priority;
        });
        int current_time = 0;

        for (auto& task : tasks) {
            task.start_time = current_time;
            current_time += task.burst_time;
            task.finish_time = current_time;
            cout << task.name << " (Priority " << task.priority << ")";
            cout << ": start at " << task.start_time << ", finish at " << task.finish_time << endl;
        }
    }

    void main_menu() {
        while (true) {
            cout << "\nChoose a Scheduling Algorithm:" << endl;
            cout << "1. First-Come, First-Serve (FCFS)" << endl;
            cout << "2. Round Robin (RR)" << endl;
            cout << "3. Priority Scheduling (Non-Preemptive)" << endl;
            cout << "4. Exit" << endl;

            int choice;
            cout << "Enter your choice (1-4): ";
            cin >> choice;
            reset_tasks();

            if (choice == 1) {
                fcfs();
            } 
            else if (choice == 2) {
                int quantum;
                cout << "Enter time quantum for Round Robin: ";
                cin >> quantum;
                round_robin(quantum);
            } 
            else if (choice == 3) {
                priority_scheduling();
            } 
            else if (choice == 4) {
                cout << "Exiting CPU Task Scheduler. Goodbye!" << endl;
                break;
            } 
            else {
                cout << "Invalid choice! Please enter a valid option." << endl;
            }
        }
    }
};

int main() {
    Scheduler scheduler;
    scheduler.get_user_tasks();
    scheduler.main_menu();
    return 0;
}