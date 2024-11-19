#ifndef INTERRUPTS_HPP_
#define INTERRUPTS_HPP_

#include<iostream>
#include<fstream>
#include<string>
#include<vector>
#include<random>
#include<utility>
#include<sstream>
#include<iomanip>
#include<queue>
#include<algorithm>


enum states {
    NEW,
    READY,
    RUNNING,
    WAITING,
    TERMINATED,
    NOT_ASSIGNED
};
std::ostream& operator<<(std::ostream& os, const enum states& s) {

	std::string state_names[] = {
                                "NEW",
                                "READY",
                                "RUNNING",
                                "WAITING",
                                "TERMINATED",
                                "NOT_ASSIGNED"
    };
    return (os << state_names[s]);
}

struct memory_partition{
    unsigned int    partition_number;
    unsigned int    size;
    int             occupied;
} memory_paritions[] = {
    {1, 40, -1},
    {2, 25, -1},
    {3, 15, -1},
    {4, 10, -1},
    {5, 8, -1},
    {6, 2, -1}
};

struct PCB{
    int             PID;
    unsigned int    size;
    unsigned int    arrival_time;
    int             start_time;
    unsigned int    processing_time;
    unsigned int    remaining_time;
    int             partition_number;
    enum states     state;
    unsigned int    io_freq;
    unsigned int    io_duration;
};

class SimpleLCG {
    public:
    SimpleLCG(uint32_t seed) : current(seed) {}

    // Generate a random number using LCG
    uint32_t generate() {
        current = (current * 1664525 + 1013904223) % (1u << 31);
        return current;
    }

    // Get a number in the desired range
    int get_random(int min, int max) {
        return min + (generate() % (max - min + 1));
    }

    private:
    uint32_t current;
};

//------------------------------------HELPER FUNCTIONS FOR THE SIMULATOR------------------------------
// Following function was taken from stackoverflow; helper function for splitting strings
std::vector<std::string> split_delim(std::string input, std::string delim) {
    std::vector<std::string> tokens;
    std::size_t pos = 0;
    std::string token;
    while ((pos = input.find(delim)) != std::string::npos) {
        token = input.substr(0, pos);
        tokens.push_back(token);
        input.erase(0, pos + delim.length());
    }
    tokens.push_back(input);

    return tokens;
}

std::string print_PCB(std::vector<PCB> _PCB) {
    const int tableWidth = 83;

    std::stringstream buffer;
    
    // Print top border
    buffer << "+" << std::setfill('-') << std::setw(tableWidth) << "+" << std::endl;
    
    // Print headers
    buffer << "|"
              << std::setfill(' ') << std::setw(4) << "PID"
              << std::setw(2) << "|"
              << std::setfill(' ') << std::setw(11) << "Partition"
              << std::setw(2) << "|"
              << std::setfill(' ') << std::setw(5) << "Size"
              << std::setw(2) << "|"
              << std::setfill(' ') << std::setw(13) << "Arrival Time"
              << std::setw(2) << "|"
              << std::setfill(' ') << std::setw(11) << "Start Time"
              << std::setw(2) << "|"
              << std::setfill(' ') << std::setw(14) << "Remaining Time"
              << std::setw(2) << "|"
              << std::setfill(' ') << std::setw(11) << "State"
              << std::setw(2) << "|" << std::endl;
    
    // Print separator
    buffer << "+" << std::setfill('-') << std::setw(tableWidth) << "+" << std::endl;
    
    // Print each PCB entry
    for (const auto& program : _PCB) {
        buffer << "|"
                  << std::setfill(' ') << std::setw(4) << program.PID
                  << std::setw(2) << "|"
                  << std::setw(11) << program.partition_number
                  << std::setw(2) << "|"
                  << std::setw(5) << program.size
                  << std::setw(2) << "|"
                  << std::setw(13) << program.arrival_time
                  << std::setw(2) << "|"
                  << std::setw(11) << program.start_time
                  << std::setw(2) << "|"
                  << std::setw(14) << program.remaining_time
                  << std::setw(2) << "|"
                  << std::setw(11) << program.state
                  << std::setw(2) << "|" << std::endl;
    }
    
    // Print bottom border
    buffer << "+" << std::setfill('-') << std::setw(tableWidth) << "+" << std::endl;

    return buffer.str();
}

std::string print_PCB(PCB _PCB) {
    std::vector<PCB> temp;
    temp.push_back(_PCB);
    return print_PCB(temp);
}

std::string get_memory_header() {

    const int tableWidth = 91;

    std::stringstream buffer;
    
    // Print top border
    buffer << "+" << std::setfill('-') << std::setw(tableWidth) << "+" << std::endl;
    
    // Print headers
    buffer  << "|"
            << std::setfill(' ') << std::setw(13) << "Time of Event"
            << std::setw(2) << "|"
            << std::setfill(' ') << std::setw(11) << "Memory Used"
            << std::setw(2) << "|"
            << std::setfill(' ') << std::setw(22) << "Partitions State"
            << std::setw(2) << "|"
            << std::setfill(' ') << std::setw(17) << "Total Free Memory"
            << std::setw(2) << "|"
            << std::setfill(' ') << std::setw(18) << "Usable Free Memory"
            << std::setw(2) << "|" << std::endl;
    
    // Print separator
    buffer << "+" << std::setfill('-') << std::setw(tableWidth) << "+" << std::endl;

    return buffer.str();

}

std::string log_memory_status(unsigned int current_time, std::vector<PCB> job_queue) {

    const int tableWidth = 91;

    std::stringstream buffer;

    unsigned int memory_used = 0;
    unsigned int total_free_memory = 0;
    unsigned int usable_free_memory = 0;
    std::string partitions_state;

    for(auto partition : memory_paritions) {
        if(partition.occupied == -1) {
            usable_free_memory += partition.size;
            total_free_memory += partition.size;
        } else {
            for(auto job : job_queue) {
                if(partition.occupied == job.PID) {
                    total_free_memory += (partition.size - job.size);
                    memory_used += job.size;
                }
            }
        }

        if(partition.partition_number < 6){
            partitions_state += std::to_string(partition.occupied) + ", ";
        } else {
            partitions_state += std::to_string(partition.occupied);
        }

    }

    buffer  << "|"
            << std::setfill(' ') << std::setw(13) << current_time
            << std::setw(2) << "|"
            << std::setw(11) << memory_used
            << std::setw(2) << "|"
            << std::setw(22) << partitions_state
            << std::setw(2) << "|"
            << std::setw(17) << total_free_memory
            << std::setw(2) << "|"
            << std::setw(18) << usable_free_memory
            << std::setw(2) << "|" << std::endl;

    return buffer.str();
}

std::string get_memory_footer() {
    const int tableWidth = 91;
    std::stringstream buffer;

    // Print bottom border
    buffer << "+" << std::setfill('-') << std::setw(tableWidth) << "+" << std::endl;

    return buffer.str();
}

std::string get_exec_header() {

    const int tableWidth = 49;

    std::stringstream buffer;
    
    // Print top border
    buffer << "+" << std::setfill('-') << std::setw(tableWidth) << "+" << std::endl;
    
    // Print headers
    buffer  << "|"
            << std::setfill(' ') << std::setw(18) << "Time of Transition"
            << std::setw(2) << "|"
            << std::setfill(' ') << std::setw(3) << "PID"
            << std::setw(2) << "|"
            << std::setfill(' ') << std::setw(10) << "Old State"
            << std::setw(2) << "|"
            << std::setfill(' ') << std::setw(10) << "New State"
            << std::setw(2) << "|" << std::endl;
    
    // Print separator
    buffer << "+" << std::setfill('-') << std::setw(tableWidth) << "+" << std::endl;

    return buffer.str();

}

std::string log_exec_status(unsigned int current_time, int PID, states old_state, states new_state) {

    const int tableWidth = 49;

    std::stringstream buffer;

    buffer  << "|"
            << std::setfill(' ') << std::setw(18) << current_time
            << std::setw(2) << "|"
            << std::setw(3) << PID
            << std::setw(2) << "|"
            << std::setw(10) << old_state
            << std::setw(2) << "|"
            << std::setw(10) << new_state
            << std::setw(2) << "|" << std::endl;

    return buffer.str();
}

std::string get_exec_footer() {
    const int tableWidth = 49;
    std::stringstream buffer;

    // Print bottom border
    buffer << "+" << std::setfill('-') << std::setw(tableWidth) << "+" << std::endl;

    return buffer.str();
}

void set_queue(std::vector<PCB> &job_queue, PCB running) {
    for(auto &process : job_queue) {
        if(process.PID == running.PID) {
            process = running;
        }
    }
}

//--------------------------------------------FUNCTIONS FOR THE "OS"-------------------------------------

//Assign memory partition to program
bool assign_memory(PCB &program) {
    int size_to_fit = program.size;
    int available_size = 0;

    for(int i = 5; i >= 0; i--) {
        available_size = memory_paritions[i].size;

        if(size_to_fit <= available_size && memory_paritions[i].occupied == -1) {
            memory_paritions[i].occupied = program.PID;
            program.partition_number = memory_paritions[i].partition_number;
            return true;
        }
    }

    return false;
}

bool free_memory(PCB &program){
    for(int i = 5; i >= 0; i--) {
        if(program.PID == memory_paritions[i].occupied) {
            memory_paritions[i].occupied = -1;
            program.partition_number = -1;
            return true;
        }
    }
    return false;
}

PCB add_process(std::vector<std::string> tokens) {
    PCB process;
    process.PID = std::stoi(tokens[0]);
    process.size = std::stoi(tokens[1]);
    process.arrival_time = std::stoi(tokens[2]);
    process.processing_time = std::stoi(tokens[3]);
    process.remaining_time = std::stoi(tokens[3]);
    process.io_freq = std::stoi(tokens[4]);
    process.io_duration = std::stoi(tokens[5]);
    process.start_time = -1;
    process.partition_number = -1;
    process.state = NOT_ASSIGNED;

    return process;
}

bool all_process_terminated(std::vector<PCB> processes) {

    for(auto process : processes) {
        if(process.state != TERMINATED) {
            return false;
        }
    }

    return true;
}

void terminate_process(PCB &running, std::vector<PCB> &job_queue) {
    running.remaining_time = 0;
    running.state = TERMINATED;
    free_memory(running);
    set_queue(job_queue, running);
}

void run_process(PCB &running, std::vector<PCB> &job_queue, std::vector<PCB> &ready_queue, unsigned int current_time) {
    running = ready_queue.back();
    ready_queue.pop_back();
    running.start_time = current_time;
    running.state = RUNNING;
    set_queue(job_queue, running);
}

void idle_CPU(PCB &running) {
    running.start_time = 0;
    running.processing_time = 0;
    running.remaining_time = 0;
    running.arrival_time = 0;
    running.io_duration = 0;
    running.io_freq = 0;
    running.partition_number = 0;
    running.size = 0;
    running.state = NOT_ASSIGNED;
    running.PID = -1;
}

void FCFS(std::vector<PCB> &list_processes, std::vector<PCB> &ready_queue) {
    std::sort( 
                ready_queue.begin(),
                ready_queue.end(),
                []( const PCB &first, const PCB &second ){
                    return (first.arrival_time > second.arrival_time); 
                } 
            );
}



std::tuple<std::string, std::string> run_simulation(std::vector<PCB> list_processes, SimpleLCG lcg) {

    std::vector<PCB> ready_queue;
    std::vector<PCB> wait_queue;
    std::vector<PCB> job_queue;

    unsigned int current_time = 0;
    PCB running;

    //Initialize an empty running process
    idle_CPU(running);

    std::string memory_status;
    std::string execution_status;

    memory_status = get_memory_header();
    memory_status += log_memory_status(current_time, job_queue);
    execution_status = get_exec_header();

    std::vector<PCB> prev_job_queue;

    while(!all_process_terminated(job_queue) || job_queue.empty()) {
        std::vector<PCB> new_processes;

        for(auto &process : list_processes) {
            if(process.arrival_time == current_time) {
                assign_memory(process);

                process.state = READY;
                set_queue(list_processes, process);
                ready_queue.push_back(process);
                job_queue.push_back(process);

                execution_status += log_exec_status(current_time, process.PID, NEW, READY);
                memory_status += log_memory_status(current_time, job_queue);
            }
        }

        if(running.PID != -1 && current_time == (running.start_time + running.io_freq) && running.remaining_time > 0) {
            running.remaining_time = running.remaining_time - running.io_freq;
            running.state = WAITING;
            set_queue(job_queue, running);
            wait_queue.push_back(running);

            execution_status += log_exec_status(current_time, running.PID, RUNNING, WAITING);
            idle_CPU(running);
        }

        if(wait_queue.size() > 1){
            std::sort (
                wait_queue.begin(),
                wait_queue.end(),
                [] (const PCB &first, const PCB &second) {
                    auto io_processing_time_1 = first.start_time + first.io_freq + first.io_duration;
                    auto io_processing_time_2 = second.start_time + second.io_freq + second.io_duration;
                    return io_processing_time_1 > io_processing_time_2;
                }
            );
        }

        while(!wait_queue.empty() && current_time == (wait_queue.back().start_time + wait_queue.back().io_freq + wait_queue.back().io_duration))  {
            wait_queue.back().state = READY;
            set_queue(job_queue, wait_queue.back());
            ready_queue.push_back(wait_queue.back());
            wait_queue.pop_back();

            execution_status += log_exec_status(current_time, ready_queue.back().PID, WAITING, READY);
        }

        FCFS(job_queue, ready_queue);

        if(current_time >= (running.remaining_time + running.start_time)) {

            if(ready_queue.empty()) {
                if(running.PID != -1){
                    execution_status += log_exec_status(current_time, running.PID, RUNNING, TERMINATED);
                    terminate_process(running, job_queue);
                    memory_status += log_memory_status(current_time, job_queue);
                }

                idle_CPU(running);
            } else {
                if(running.PID != -1){
                    execution_status += log_exec_status(current_time, running.PID, RUNNING, TERMINATED);
                    terminate_process(running, job_queue);
                    memory_status += log_memory_status(current_time, job_queue);
                }

                run_process(running, job_queue, ready_queue, current_time);
                execution_status += log_exec_status(current_time, running.PID, READY, RUNNING);
            }
        }

        current_time++;
    }

    memory_status += get_memory_footer();
    execution_status += get_exec_footer();

    return std::make_tuple(execution_status, memory_status);
}

#endif