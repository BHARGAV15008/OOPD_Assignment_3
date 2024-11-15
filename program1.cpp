#include <boost/heap/fibonacci_heap.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <iostream>
#include <string>
#include <vector>
using namespace std;
// Task class to represent scheduled tasks
class Task {
private:
    string name;
    int priority;
    boost::posix_time::ptime scheduledTime;
    string description;

public:
    // Constructor
    Task(const string& n, int p, const boost::posix_time::ptime& t, const string& d)
        : name(n), priority(p), scheduledTime(t), description(d) {}

    // Operator overload for heap comparison
    bool operator<(const Task& other) const {
        if (priority != other.priority)
            return priority < other.priority;
        return scheduledTime > other.scheduledTime;
    }

    // Getter methods
    string getName() const { return name; }
    int getPriority() const { return priority; }
    boost::posix_time::ptime getTime() const { return scheduledTime; }
    string getDescription() const { return description; }

    // Method to display task details
    void displayTask() const {
        cout << "Name: " << name << "\n"
                  << "Priority: " << priority << "\n"
                  << "Scheduled Time: " << scheduledTime << "\n"
                  << "Description: " << description << "\n";
    }
};

// TaskScheduler class to manage tasks
class TaskScheduler {
private:
    boost::heap::fibonacci_heap<Task> taskHeap;
    int totalTasks;

public:
    // Constructor
    TaskScheduler() : totalTasks(0) {}

    // Method to add a new task
    void addTask(const string& name, int priority, 
                const boost::posix_time::ptime& time, 
                const string& description) {
        taskHeap.push(Task(name, priority, time, description));
        totalTasks++;
        cout << "Task added: " << name << " (Priority: " << priority << ")\n";
    }

    // Method to process the next highest priority task
    void processNextTask() {
        if (taskHeap.empty()) {
            cout << "No tasks in queue!\n";
            return;
        }

        const Task& topTask = taskHeap.top();
        cout << "\nProcessing highest priority task:\n";
        topTask.displayTask();
        
        taskHeap.pop();
        totalTasks--;
    }

    // Method to display scheduler statistics
    void displayStats() const {
        cout << "\nScheduler Statistics:\n";
        cout << "Total tasks in queue: " << totalTasks << "\n";
        if (!taskHeap.empty()) {
            const Task& next = taskHeap.top();
            cout << "Next task: " << next.getName() 
                     << " (Priority: " << next.getPriority() << ")\n";
        }
    }

    // Method to get total number of tasks
    int getTotalTasks() const {
        return totalTasks;
    }
};

// Test function to verify scheduler functionality
void runSchedulerTests(TaskScheduler& scheduler) {
    boost::posix_time::ptime now = boost::posix_time::second_clock::local_time();
    
    // Test Case 1: Add tasks with different priorities
    cout << "\nTest Case 1: Adding tasks with different priorities\n";
    scheduler.addTask("Urgent Meeting", 10,
        now + boost::posix_time::minutes(30),
        "Emergency team meeting for project review");
        
    scheduler.addTask("Code Review", 5,
        now + boost::posix_time::hours(2),
        "Review pull requests for feature branch");
        
    scheduler.addTask("Daily Backup", 3,
        now + boost::posix_time::hours(1),
        "Perform daily system backup");
        
    scheduler.addTask("Documentation", 4,
        now + boost::posix_time::minutes(45),
        "Update API documentation");

    // Test Case 2: Display initial statistics
    cout << "\nTest Case 2: Checking initial statistics\n";
    scheduler.displayStats();

    // Test Case 3: Process tasks in priority order
    cout << "\nTest Case 3: Processing tasks in priority order\n";
    for (int i = 0; i < 3; ++i) {
        scheduler.processNextTask();
        scheduler.displayStats();
    }

    // Test Case 4: Adding task after processing
    cout << "\nTest Case 4: Adding new task after processing others\n";
    scheduler.addTask("New Feature", 8,
        now + boost::posix_time::hours(3),
        "Implement new feature request");
    scheduler.displayStats();

    // Test Case 5: Process remaining tasks
    cout << "\nTest Case 5: Processing remaining tasks\n";
    while (scheduler.getTotalTasks() > 0) {
        scheduler.processNextTask();
        scheduler.displayStats();
    }
}

int main() {
    cout << "Starting Task Scheduler Tests...\n";
    TaskScheduler scheduler;
    
    try {
        runSchedulerTests(scheduler);
    }
    catch (const exception& e) {
        cerr << "Error occurred: " << e.what() << endl;
        return 1;
    }

    cout << "\nAll tests completed successfully!\n";
    return 0;
}