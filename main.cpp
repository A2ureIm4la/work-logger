#include <iostream>     // For terminal i/o
#include <cstdlib>      // To run system commands (clear/cls)
#include <limits>       // Used in wait_enter() to clear line

#include <iomanip>      // To convert epoch to readable format
#include <ctime>        // To get system time

#include <cmath>        // To round numbers

#include <vector>       // For lists
#include <string>       // For string manipulation
#include <format>       // F-strings kinda
#include <map>          // Used for key:vaue maps

#include <thread>       // Get thread for sleep
#include <chrono>       // Sleep

#include <fstream>      // File manipulation
#include <filesystem>   // Check for present files

#include "main.h"       // Include header file

// A struct to keep important values that need to be accessed globally
struct data {  

    // Clocked on/off state 
    int clocked_on = 0;
    
    // Epoch times of clocked on/off
    std::time_t clocked_on_epoch = 0;
    std::time_t clocked_off_epoch = 0;

    // Pay rate
    float hourly_pay = 20.0f;

};
// Declare the user struct
data User;

// A struct for acsi styles
struct style {

    // Functional
    std::string END = "\033[0m";

    // Colours
    std::string RED = "\033[31m";
    std::string YEL = "\033[33m";
    std::string GRE = "\033[32m";
    std::string BLU = "\033[34m";
    std::string MAG = "\033[35m";
    std::string CYA = "\033[36m";

    // Graphics
    std::string BOL = "\033[1m";
    std::string ITA = "\033[3m";

    // Cursors
    std::string CRH = "\033[?25l";
    std::string CRS = "\033[?25h";

};
style Style;

// Helper function to clear screen
void clear_terminal() {

    std::system("clear");

}

// Helper function to sleep terminal
void sleep(int ms) {

    std::this_thread::sleep_for(std::chrono::milliseconds(ms));

}

// Helper function to wait for enter key press
void wait_enter() {

    // Wait for key press
    std::cin.get();

}

// Helper function to get current time in epoch format
std::time_t current_epoch_time() {

    // Return current time in epoch
    return std::time(nullptr);
    
}

// Helper function to convert epoch time to readable format
std::string convert_epoch(std::time_t epoch, std::string params) {

    // Convert epoch to local time structure
    std::tm* local_time = std::localtime(&epoch);

    // Use stringstream instead of direct assignment
    std::stringstream ss;
    ss << std::put_time(local_time, params.c_str());

    // Extract the std::string
    std::string formatted_time = ss.str();

    // Return the formatted epoch time
    return formatted_time;

}

// Helper class to read and write to csv file
class csv {
private:

    // Private vars (can only be accessed within class)
    const std::string header = "Date,Start Time,Stop Time,Hours Worked,Total Pay($),Description";
    const std::string csv_file_path = "./loggedwork.csv";
    std::ofstream csv_file;

public:

    // Consructor (runs on class inilization)
    csv() {

        // If csv file does not exist, create one.
        if (!std::filesystem::exists(csv_file_path)) {

            // Create and open the csv file
            csv_file.open(csv_file_path);

            // Write the header to the csv file
            csv_file << header << std::endl;

        }

        // Close the file
        csv_file.close();

    }

    // Function to write to file
    void write(std::string& date, std::string& start_time, std::string& stop_time, double hours_worked, double total_pay, std::string& description) {

        // Open file
        csv_file.open(csv_file_path, std::ios_base::app);

        // Write the row of date to it.
        csv_file << date << "," << start_time << "," << stop_time << "," << hours_worked << "," << total_pay << "," << description << std::endl;

        csv_file.close();

    }

};
csv Csv;

// Config file class
class config {
private:

    const std::string config_file_path = "./config.cfg";

public:

    // Function to save values of user struct to the config file
    void save() {

        // Open file in write mode
        std::ofstream config_file(config_file_path);

        // Write each values to the config file
        config_file << "clocked_on=" << User.clocked_on << std::endl;
        config_file << "clocked_on_epoch=" << User.clocked_on_epoch << std::endl;
        config_file << "clocked_off_epoch=" << User.clocked_off_epoch << std::endl;
        config_file << "hourly_pay=" << User.hourly_pay << std::endl; 

    }

    // Function to load values from the config file to the user struct
    void load() {

        std::ifstream config_file(config_file_path);

        // If no config file yet return
        if (!config_file) {return;}

        std::string line;
        while (std::getline(config_file, line)) {
            
            // Get the equal sign index
            int equal_sign = line.find("=");

            // If the equals sign isn't found then exit out of while loop
            if (equal_sign == std::string::npos) {continue;}

            // Get key and value pairs relative to the equals sign
            std::string key = line.substr(0, equal_sign);
            std::string val = line.substr(equal_sign + 1);

            // Update the user struct with the new values, converting values at need
            if (key == "clocked_on") { 
                User.clocked_on = std::stoi(val);
            } else if (key == "clocked_on_epoch") {
                User.clocked_on_epoch = std::stoll(val);
            } else if (key == "clocked_off_epoch") {
                User.clocked_off_epoch = std::stoll(val);
            } else if (key == "hourly_pay") {
                User.hourly_pay = std::stof(val);
            }


        }

    }

};
config Config;

// Main menu function
int menu_main() {

    // Clear the terminal to start off with
    clear_terminal();

    // Print main menu header with blue and bold
    std::cout << Style.BLU << Style.BOL;
    std::cout << R"(  _                       _                _____           _                  )" << std::endl;
    std::cout << R"( | |                     (_)              / ____|         | |                 )" << std::endl;
    std::cout << R"( | |     ___   __ _  __ _ _ _ __   __ _  | (___  _   _ ___| |_ ___ _ __ ___   )" << std::endl;
    std::cout << R"( | |    / _ \ / _` |/ _` | | '_ \ / _` |  \___ \| | | / __| __/ _ \ '_ ` _ \  )" << std::endl;
    std::cout << R"( | |___| (_) | (_| | (_| | | | | | (_| |  ____) | |_| \__ \ ||  __/ | | | | | )" << std::endl;
    std::cout << R"( |______\___/ \__, |\__, |_|_| |_|\__, | |_____/ \__, |___/\__\___|_| |_| |_| )" << std::endl;
    std::cout << R"(               __/ | __/ |         __/ |          __/ |                       )" << std::endl;
    std::cout << R"(              |___/ |___/         |___/          |___/                        )" << std::endl;
    std::cout << R"(                                                                              )" << std::endl;
    std::cout << Style.END;

    // If block to switch clock-on and clock-off options respectfully
    std::vector<std::string> menu_items;
    if (User.clocked_on == 0) {
        menu_items = {"Clock On", "Stats", "Exit"};
    } else {
        menu_items = {"Clock Off", "Stats", "Exit"};
    }

    // Print the menu items
    int count = 1;
    for (std::string& item : menu_items) {

        // Print item to terminal with an index
        std::cout << Style.BLU << "[" << count << "] " << Style.END << item << std::endl;
        count++;

    }

    // Spacer
    std::cout << std::endl;

    // Get the users choice of action
    std::cout << "Select an Option " << Style.BLU << "-> " << Style.END;
    int choice;
    std::cin >> choice;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    // Run the selected option
    switch (choice) {
    
    // Clock on/off
    case 1:
        
        if (User.clocked_on == 0) {clock_on();} else {clock_off();}
        break;
    
    // Stats page
    case 2:

        show_stats();
        break;
    
    // Exit
    case 3:

        // Return 0
        return 0;
        
    // Else user inputs invalid option
    default:
        std::cout << Style.RED << "Invalid Option." << Style.END << std::flush;
        sleep(1000);
        return 1;
    }

    return 1;

}

// Clock on function
void clock_on() {

    // Hide cursor
    std::cout << Style.CRH;

    // Set some varibles
    User.clocked_on_epoch = current_epoch_time();
    User.clocked_off_epoch = 0;
    User.clocked_on = 1;

    // Save status to file
    Config.save();

    // Output success message
    std::cout << Style.GRE << Style.ITA << "Successfully clocked in" << Style.END << std::endl;
    std::cout << "Date: " << Style.BLU << convert_epoch(User.clocked_on_epoch, "%D") << Style.END << std::endl;
    std::cout << "Time: " << Style.BLU << convert_epoch(User.clocked_on_epoch, "%H:%M:%S") << Style.END << std::endl;

    std::cout << Style.ITA << Style.GRE << "Press ENTER to return" << Style.END;

    wait_enter();

    // Show cursor
    std::cout << Style.CRS;

}

// Clock off function
void clock_off() {

    // Print message
    std::cout << Style.GRE << Style.ITA << "Clocking Off..." << Style.END << std::endl;

    // Save clocked off time
    User.clocked_off_epoch = current_epoch_time();

    // Ask user for shift desc
    std::cout << "Shift Notes " << Style.BLU << "-> " << Style.END;
    std::string shift_desc;
    std::getline(std::cin, shift_desc);

    // Hide cursor
    std::cout << Style.CRH;

    // Calc duration in hours of how long shift was
    std::time_t duration = User.clocked_off_epoch - User.clocked_on_epoch;

    double hours_worked = std::round((static_cast<double>(duration) / 3600.0) * 100.0) / 100.0;

    // Calc total pay
    double total_pay = std::round((hours_worked * User.hourly_pay) * 100.0) / 100.0;

    // Set date
    std::string date = convert_epoch(User.clocked_on_epoch, "%d/%m/%Y");

    // Get start time
    std::string start_time = convert_epoch(User.clocked_on_epoch, "%H:%M:%S");

    // Get stop time
    std::string stop_time = convert_epoch(User.clocked_off_epoch, "%H:%M:%S");

    // Finnally write all the data to the csv
    Csv.write(
        date,
        start_time,
        stop_time,
        hours_worked,
        total_pay,
        shift_desc
    );

    // Shift is now complete
    User.clocked_on = 0;
    User.clocked_on_epoch = 0;
    User.clocked_off_epoch = 0;
    Config.save();

    // Print success message
    std::cout << Style.GRE << Style.ITA << "Successfully clocked off" << Style.END << std::endl;
    std::cout << "Hours Worked:  " << Style.BLU << hours_worked << Style.END << std::endl;
    std::cout << "Total Pay:     " << Style.BLU << "$" << total_pay << Style.END << std::endl;
    std::cout << Style.ITA << Style.GRE << "Press ENTER to return" << Style.END;

    wait_enter();

    // Show cursor
    std::cout << Style.CRS;

}

// Show stats function
void show_stats() {

    clear_terminal();

    std::cout << "stat page" << std::flush;

    sleep(1000);

}

// Main entry point
int main() {        

    // Load config values into user struct
    Config.load();

    // Main loop
    // menu_main will return 0 if to re-loop or 1 to exit the application
    int exit_code;
    while (exit_code != 0) {

        exit_code = menu_main();

    }
    
    return 0;
}