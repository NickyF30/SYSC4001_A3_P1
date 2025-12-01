/**
 * @file interrupts.cpp
 * @author nicky fang
 * @brief template main.cpp file for Assignment 3 Part 1 of SYSC4001
 * 
 */

#include<interrupts_student1_student2.hpp>

const unsigned int TIME_QUANTUM = 100;

void RR_Scheduler(std::vector<PCB> &ready_queue) {
    // queue because fifo
}

std::tuple<std::string> run_simulation(std::vector<PCB> list_processes) {
    std::vector<PCB> ready_queue;
    std::vector<PCB> wait_queue;
    std::vector<PCB> all_processes = list_processes;

    unsigned int current_time = 0;
    PCB running_process;
    idle_CPU(running_process);

    unsigned int quantum_remaining = 0;

    std::string execution_log = print_exec_header();

    while (!all_process_terminated(all_processes)) {

        for (auto &p : all_processes) {
            if (p.state == NEW && p.arrival_time == current_time) {
                if (assign_memory(p)) {
                    p.state = READY;
                    ready_queue.push_back(p);
                    execution_log += print_exec_status(current_time, p.PID, NEW, READY);
                    execution_log += print_memory_usage();
                }
            }
            else if (p.state == NEW && p.arrival_time < current_time) {
                if (assign_memory(p)) {
                    p.state = READY;
                    ready_queue.push_back(p);
                    execution_log += print_exec_status(current_time, p.PID, NEW, READY);
                    execution_log += print_memory_usage();
                }
            }
        }
        auto it = wait_queue.begin();
        while (it != wait_queue.end()) {
            if (it->io_completion_time == current_time) {
                it->state = READY;
                if (it->io_freq > 0) it->time_until_io = it->io_freq;

                for(auto &p : all_processes) {
                    if(p.PID == it->PID) p = *it;
                }

                ready_queue.push_back(*it);
                execution_log += print_exec_status(current_time, it->PID, WAITING, READY);
                it = wait_queue.erase(it);
            } else {
                ++it;
            }
        }
        if (running_process.PID == -1 && !ready_queue.empty()) {
            running_process = ready_queue.front();
            ready_queue.erase(ready_queue.begin());

            running_process.state = RUNNING;
            quantum_remaining = TIME_QUANTUM;

            for(auto &p : all_processes) {
                if(p.PID == running_process.PID) p = running_process;
            }

            execution_log += print_exec_status(current_time, running_process.PID, READY, RUNNING);
        }
        if (running_process.PID != -1) {
            running_process.remaining_time--;
            quantum_remaining--;
            if(running_process.io_freq > 0) {
                running_process.time_until_io--;
            }
            if (running_process.remaining_time == 0) {
                running_process.state = TERMINATED;
                free_memory(running_process);

                for(auto &p : all_processes) {
                    if(p.PID == running_process.PID) p = running_process;
                }

                execution_log += print_exec_status(current_time + 1, running_process.PID, RUNNING, TERMINATED);
                execution_log += print_memory_usage();

                idle_CPU(running_process);
            }
            // io
            else if (running_process.io_freq > 0 && running_process.time_until_io == 0) {
                running_process.state = WAITING;
                running_process.io_completion_time = current_time + 1 + running_process.io_duration;

                for(auto &p : all_processes) {
                    if(p.PID == running_process.PID) p = running_process;
                }

                wait_queue.push_back(running_process);
                execution_log += print_exec_status(current_time + 1, running_process.PID, RUNNING, WAITING);

                idle_CPU(running_process);
            }
            //expiry
            else if (quantum_remaining == 0) {
                running_process.state = READY;
                for(auto &p : all_processes) {
                    if(p.PID == running_process.PID) p = running_process;
                }
                ready_queue.push_back(running_process);
                execution_log += print_exec_status(current_time + 1, running_process.PID, RUNNING, READY);
                idle_CPU(running_process);
            }
        }
        current_time++;
    }
    execution_log += print_exec_footer();
    return std::make_tuple(execution_log);
}



int main(int argc, char** argv) {

    //Get the input file from the user
    if(argc != 2) {
        std::cout << "ERROR!\nExpected 1 argument, received " << argc - 1 << std::endl;
        std::cout << "To run the program, do: ./interrutps <your_input_file.txt>" << std::endl;
        return -1;
    }

    //Open the input file
    auto file_name = argv[1];
    std::ifstream input_file;
    input_file.open(file_name);

    //Ensure that the file actually opens
    if (!input_file.is_open()) {
        std::cerr << "Error: Unable to open file: " << file_name << std::endl;
        return -1;
    }

    //Parse the entire input file and populate a vector of PCBs.
    //To do so, the add_process() helper function is used (see include file).
    std::string line;
    std::vector<PCB> list_process;
    while(std::getline(input_file, line)) {
        auto input_tokens = split_delim(line, ", ");
        auto new_process = add_process(input_tokens);
        list_process.push_back(new_process);
    }
    input_file.close();

    //With the list of processes, run the simulation
    auto [exec] = run_simulation(list_process);

    write_output(exec, "execution.txt");

    return 0;
}