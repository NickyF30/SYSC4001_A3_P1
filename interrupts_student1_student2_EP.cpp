/**
 * @file interrupts.cpp
 * @author nicky fang
 * @brief template main.cpp file for Assignment 3 Part 1 of SYSC4001
 * 
 */

#include<interrupts_student1_student2.hpp>

// sort: Lower PID = Higher Priority
void EP_Scheduler(std::vector<PCB> &ready_queue) {
    std::sort(
        ready_queue.begin(),
        ready_queue.end(),
        [](const PCB &a, const PCB &b) {
            // primary sort (pid)
            if (a.priority != b.priority)
                return a.priority < b.priority;
            return a.arrival_time < b.arrival_time;
        }
    );
}

//rewritten
std::tuple<std::string> run_simulation(std::vector<PCB> list_processes) {
    std::vector<PCB> ready_queue;
    std::vector<PCB> wait_queue;
    std::vector<PCB> all_processes = list_processes;

    unsigned int current_time = 0;
    PCB running_process;
    idle_CPU(running_process);

    std::string execution_log = print_exec_header();

    // simulation loop
    while (!all_process_terminated(all_processes)) {

        // 1. check arrivals
        for (auto &p : all_processes) {
            if (p.state == NEW && p.arrival_time == current_time) {
                // try and assign memory
                if (assign_memory(p)) {
                    p.state = READY;
                    ready_queue.push_back(p);
                    execution_log += print_exec_status(current_time, p.PID, NEW, READY);
                    execution_log += print_memory_usage();
                } else {
                    //empty for now
                }
            }
            // if didnt assign before, retry
            else if (p.state == NEW && p.arrival_time < current_time) {
                 if (assign_memory(p)) {
                    p.state = READY;
                    ready_queue.push_back(p);
                    execution_log += print_exec_status(current_time, p.PID, NEW, READY);
                    execution_log += print_memory_usage();
                }
            }
        }

        // 2. wait Queue
        auto it = wait_queue.begin();
        while (it != wait_queue.end()) {
            if (it->io_completion_time == current_time) {
                // io complete
                it->state = READY;
                // time reset
                if (it->io_freq > 0) it->time_until_io = it->io_freq;

                // sync
                for(auto &p : all_processes) if(p.PID == it->PID) p = *it;

                ready_queue.push_back(*it);
                execution_log += print_exec_status(current_time, it->PID, WAITING, READY);
                it = wait_queue.erase(it);
            } else {
                ++it;
            }
        }

        // 3. scheduler dispatching
        // if idle, get process
        if (running_process.PID == -1 && !ready_queue.empty()) {
            EP_Scheduler(ready_queue); //priority
            running_process = ready_queue.front();
            ready_queue.erase(ready_queue.begin());
            running_process.state = RUNNING;
            if (running_process.start_time == -1) {
                running_process.start_time = current_time;
            }
            // sync
            for(auto &p : all_processes) if(p.PID == running_process.PID) p = running_process;

            execution_log += print_exec_status(current_time, running_process.PID, READY, RUNNING);
        }
        if (running_process.PID != -1) {
            running_process.remaining_time--;
            if(running_process.io_freq > 0) {
                running_process.time_until_io--;
            }

            if (running_process.remaining_time == 0) {
                running_process.state = TERMINATED;
                free_memory(running_process);
                running_process.finish_time = current_time + 1;
                // Sync
                for(auto &p : all_processes) if(p.PID == running_process.PID) p = running_process;

                execution_log += print_exec_status(current_time + 1, running_process.PID, RUNNING, TERMINATED);
                execution_log += print_memory_usage();

                idle_CPU(running_process); // Free CPU
            }
            // check io aggain
            else if (running_process.io_freq > 0 && running_process.time_until_io == 0) {
                running_process.state = WAITING;
                running_process.io_completion_time = current_time + 1 + running_process.io_duration;

                // Sync
                for(auto &p : all_processes) if(p.PID == running_process.PID) p = running_process;

                wait_queue.push_back(running_process);
                execution_log += print_exec_status(current_time + 1, running_process.PID, RUNNING, WAITING);

                idle_CPU(running_process); //F REE CPU!
            }
        }

        current_time++;
    }

    execution_log += print_exec_footer();
    return std::make_tuple(execution_log);
}


int main(int argc, char** argv) {

    //Get input file
    if(argc != 2) {
        std::cout << "ERROR!\nExpected 1 argument, received " << argc - 1 << std::endl;
        std::cout << "To run the program, do: ./interrutps <your_input_file.txt>" << std::endl;
        return -1;
    }

    // input file
    auto file_name = argv[1];
    std::ifstream input_file;
    input_file.open(file_name);

    //parsing file
    std::string line;
    std::vector<PCB> list_process;
    while(std::getline(input_file, line)) {
        auto input_tokens = split_delim(line, ", ");
        auto new_process = add_process(input_tokens);
        list_process.push_back(new_process);
    }
    input_file.close();

    // run simulation
    auto [exec] = run_simulation(list_process);

    write_output(exec, "execution.txt");

    return 0;
}