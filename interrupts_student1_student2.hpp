/**
 * @file interrupts.hpp
 * @author Sasisekhar Govind
 * @brief template main.cpp file for Assignment 3 Part 1 of SYSC4001
 * 
 */

#ifndef INTERRUPTS_HPP_
#define INTERRUPTS_HPP_

#include<iostream>
#include<fstream>
#include<string>
#include<vector>
#include<tuple>
#include<random>
#include<utility>
#include<sstream>
#include<iomanip>
#include<algorithm>

// Enum
enum states {
    NEW,
    READY,
    RUNNING,
    WAITING,
    TERMINATED,
    NOT_ASSIGNED
};

// overload
std::ostream& operator<<(std::ostream& os, const enum states& s) {
    std::string state_names[] = {
        "NEW", "READY", "RUNNING", "WAITING", "TERMINATED", "NOT_ASSIGNED"
    };
    return (os << state_names[s]);
}

// memory partition
struct memory_partition {
    unsigned int    partition_number;
    unsigned int    size;
    int             occupied;
} memory_partitions[] = {
    {1, 40, -1},
    {2, 25, -1},
    {3, 15, -1},
    {4, 10, -1},
    {5, 8, -1},
    {6, 2, -1}
};

struct PCB {
    //tabbed this over because it was hard to read
    int             PID;
    unsigned int    size;
    unsigned int    arrival_time;
    int             start_time;
    unsigned int    processing_time; // total CPU time required
    unsigned int    remaining_time;
    int             partition_number;
    enum states     state;
    unsigned int    io_freq;
    unsigned int    io_duration;

    //added for a3 logic
    int             priority;
    unsigned int    time_until_io;
    unsigned int    io_completion_time;
    unsigned int    finish_time;
};

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

std::string print_exec_header() {
    return "+------------------------------------------------------------+\n"
            "| Time of Transition |  PID  |   Old State  |   New State  |\n"
            "+------------------------------------------------------------+\n";
}

std::string print_exec_status(unsigned int current_time, int PID, states old_state, states new_state) {
    std::stringstream buffer;
    buffer << "|" << std::setfill(' ') << std::setw(18) << current_time
           << " | " << std::setw(5) << PID
           << " | " << std::setw(12) << old_state
           << " | " << std::setw(12) << new_state << " |" << std::endl;
    return buffer.str();
}

std::string print_exec_footer() {
    const int tableWidth = 60;
    std::stringstream buffer;
    buffer << "+" << std::setfill('-') << std::setw(tableWidth) << "+" << std::endl;
    return buffer.str();
}

std::string print_memory_usage() {
    std::stringstream buffer;
    int total_used = 0;
    int total_free = 0;
    int total_usable = 0;
    buffer << "   [MEMORY STATUS] partitions: ";
    for (int i = 0; i < 6; i++) {
        buffer << "P" << memory_partitions[i].partition_number << "(" << memory_partitions[i].size << "MB):";
        if (memory_partitions[i].occupied != -1) {
            buffer << "Used(PID" << memory_partitions[i].occupied << ") ";
            total_used += memory_partitions[i].size;
        } else {
            buffer << "Free ";
            total_free += memory_partitions[i].size;
            total_usable += memory_partitions[i].size;
        }
    }
    buffer << "| Total Usable Free: " << total_usable << " MB" << std::endl;
    return buffer.str();
}

// write output
void write_output(std::string execution, const char* filename) {
    std::ofstream output_file(filename);
    if (output_file.is_open()) {
        output_file << execution;
        output_file.close();
        std::cout << "Output generated in " << filename << std::endl;
    } else {
        std::cerr << "Error opening output file!" << std::endl;
    }
}

// memory Management
bool assign_memory(PCB &program) {
    for(int i = 5; i >= 0; i--) {
        if(program.size <= memory_partitions[i].size && memory_partitions[i].occupied == -1) {
            memory_partitions[i].occupied = program.PID;
            program.partition_number = memory_partitions[i].partition_number;
            return true;
        }
    }
    return false;
}

bool free_memory(PCB &program){
    for(int i = 0; i < 6; i++) {
        if(program.PID == memory_partitions[i].occupied) {
            memory_partitions[i].occupied = -1;
            program.partition_number = -1;
            return true;
        }
    }
    return false;
}

//pasrse
PCB add_process(std::vector<std::string> tokens) {
    PCB process;
    process.PID = std::stoi(tokens[0]);
    process.size = std::stoi(tokens[1]);
    process.arrival_time = std::stoi(tokens[2]);
    process.processing_time = std::stoi(tokens[3]);
    process.remaining_time = process.processing_time;
    process.io_freq = std::stoi(tokens[4]);
    process.io_duration = std::stoi(tokens[5]);
    process.start_time = -1;
    process.partition_number = -1;
    process.state = NEW; // Start as NEW
    process.priority = process.PID;
    if(process.io_freq == 0) process.time_until_io = process.processing_time + 1;
    else process.time_until_io = process.io_freq;

    process.io_completion_time = 0;

    return process;
}

bool all_process_terminated(const std::vector<PCB>& processes) {
    for(const auto& process : processes) {
        if(process.state != TERMINATED) return false;
    }
    return true;
}

// setup idle
void idle_CPU(PCB &running) {
    running.PID = -1;
    running.state = NOT_ASSIGNED;
    running.remaining_time = 0;
    running.processing_time = 0;
}

#endif