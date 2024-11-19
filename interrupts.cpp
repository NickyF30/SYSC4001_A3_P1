#include<interrupts.hpp>

int main(int argc, char** argv) {

    // if(argc != 2) {
    //     std::cout << "ERROR!\nExpected 1 argument, received " << argc - 1 << std::endl;
    //     std::cout << "To run the program, do: ./interrutps <your_input_file.txt>" << std::endl;
    // }

    auto file_name = "input_file.txt";
    std::ifstream input_file;
    input_file.open(file_name);
    std::string line;
    std::vector<PCB> list_process;

    if (!input_file.is_open()) {
        std::cerr << "Error: Unable to open file: " << file_name << std::endl;
        exit;
    }
    while(std::getline(input_file, line)) {
        auto input_tokens = split_delim(line, ", ");
        auto new_process = add_process(input_tokens);
        list_process.push_back(new_process);
    }
    input_file.close();


    SimpleLCG lcg(12345);  // Seed the LCG with a number (can be customized)

    auto [exec, mem] = run_simulation(list_process, lcg);

    std::cout << exec << std::endl;
    std::cout << mem << std::endl;

    return 0;
}