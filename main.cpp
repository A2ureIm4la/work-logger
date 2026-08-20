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
struct user {  

    // Clocked on/off state 
    int clocked_on = 0;
    
    // Epoch times of clocked on/off
    std::time_t clocked_on_epoch = 0;
    std::time_t clocked_off_epoch = 0;

    // Pay rate
    double hourly_pay = 20.00;

    // Stats
    int shifts_worked = 0;
    double total_hours_worked = 0;
    double total_pay = 0;

};
// Declare the user struct
user User;

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

// Helper function to print styled message to terminal
void msg(std::string message, std::string type, bool newline_encase) {

    // Print newline after message 
    if (newline_encase) {std::cout << std::endl;}

    // Find what type it print
    if (type == "error") {
        
        // Message encased in red flags
        std::cout << Style.RED << "[!] " << Style.END << message << Style.RED << " [!]" << Style.END << std::endl;
        sleep(1500);

    } else if (type == "info") {
        
        // Message encased in yellow flags
        std::cout << Style.YEL << "[!] " << Style.END << message << Style.YEL << " [!]" << Style.END << std::endl;

    } else if (type == "success") {

        // Message encased in green flags
        std::cout << Style.GRE << "[!] " << Style.END << message << Style.GRE << " [!]" << Style.END << std::endl;

    }


    // Print newline after message 
    if (newline_encase) {std::cout << std::endl;}

    return;

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

// Helper function to update stats
void update_stats(int shifts_worked, double total_hours_worked, double total_pay) {

    // Update each entry in user struct 
    User.shifts_worked += shifts_worked;
    User.total_hours_worked += total_hours_worked;
    User.total_pay += total_pay;

    return;

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
        config_file << "shifts_worked=" << User.shifts_worked << std::endl;
        config_file << "total_hours_worked=" << User.total_hours_worked << std::endl;
        config_file << "total_pay=" << User.total_pay << std::endl;

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
            } else if (key == "shifts_worked") {
                User.shifts_worked = std::stoi(val);
            } else if (key == "total_hours_worked") {
                User.total_hours_worked = std::stod(val);
            } else if (key == "total_pay") {
                User.total_pay = std::stod(val);
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
        menu_items = {"Clock On", "Stats", "Settings"};
    } else {
        menu_items = {"Clock Off", "Stats", "Settings"};
    }

    // Print the menu items
    int count = 1;
    for (std::string& item : menu_items) {

        // Print item to terminal with an index
        std::cout << Style.BLU << " [" << count << "] " << Style.END << item << std::endl;
        count++;

    }

    // Print exit with custom index
    std::cout << Style.BOL << Style.RED << " [" << "0" << "] " << "Exit" <<  Style.END << std::endl;


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
        
        // Settings
        case 3:
            
            settings();
            break;
        
        // Exit
        case 0:
            
            msg("Exiting", "info", true);

            // Return 0
            return 0;
            
        // Else user inputs invalid option
        default:
            msg("Invalid Option", "error", true);
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
    msg("Clocking In", "info", true);

    std::cout << " Date: " << Style.BLU << convert_epoch(User.clocked_on_epoch, "%D") << Style.END << std::endl;
    std::cout << " Time: " << Style.BLU << convert_epoch(User.clocked_on_epoch, "%H:%M:%S") << Style.END << std::endl;

    msg("Press ENTER to return", "info", true);

    wait_enter();

    // Show cursor
    std::cout << Style.CRS;

}

// Clock off function
void clock_off() {

    // Print message
    msg("Clocking Off", "info", true);

    // Save clocked off time
    User.clocked_off_epoch = current_epoch_time();

    // Ask user for shift desc
    std::cout << " Shift Notes " << Style.BLU << "-> " << Style.END;
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
    std::string date = convert_epoch(User.clocked_on_epoch, "%c");

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

    // Call the stats update
    update_stats(1, hours_worked, total_pay);

    // Shift is now complete
    User.clocked_on = 0;
    User.clocked_on_epoch = 0;
    User.clocked_off_epoch = 0;
    Config.save();

    // Print success message
    msg("Successfully Clocked Off", "success", true);
    std::cout << " Hours Worked:  " << Style.BLU << hours_worked << Style.END << std::endl;
    std::cout << " Total Pay:     " << Style.BLU << "$" << total_pay << Style.END << std::endl;
    msg("Press ENTER to Return", "info", true);

    wait_enter();

    // Show cursor
    std::cout << Style.CRS;

}

// Show stats function
void show_stats() {

    // Hide cursor
    std::cout << Style.CRH;

    // Space line
    std::cout << std::endl;

    // Print stats
    std::cout << "--------< " << Style.BLU << "STATS" << Style.END << " >--------" << std::endl;
    std::cout << " Shifts Worked " << Style.BLU << " ->  " << Style.END << User.shifts_worked << std::endl;
    std::cout << " Hours Worked  " << Style.BLU << " ->  " << Style.END << User.total_hours_worked << std::endl;
    std::cout << " Total Pay     " << Style.BLU << " ->  " << Style.END << "$" << User.total_pay << std::endl;

    msg("Press ENTER to Return", "info", true);    

    wait_enter();

    std::cout << Style.CRS;

}

// Settings function
void settings() {

    // Spacer
    std::cout << std::endl;

    // Print setting options
    std::vector<std::string> menu_items = {
        "Hourly Pay",
        "Wipe Data"
    };
    int count = 1;
    for (std::string& item : menu_items) {

        // Print item to terminal with an index
        std::cout << Style.BLU << " [" << count << "] " << Style.END << item << std::endl;
        count++;

    }

    std::cout << Style.RED << Style.BOL << " [" << "0" << "] " << "Return" << Style.END << std::endl;

    // Spacer
    std::cout << std::endl;

    // Get the users choice
    std::cout << "Select a Setting to Run/Edit" << Style.BLU << "-> " << Style.END;
    int choice;
    std::cin >> choice;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    // Spacer
    std::cout << std::endl;

    // Runt the selected choice
    switch (choice) {

        // Hourly pay edit
        case 1:

            // Print current value and new value
            std::cout << " Current Value " << Style.BLU << "-> " << Style.END << User.hourly_pay << std::endl;
            std::cout << " New Value     " << Style.BLU << "-> " << Style.END;
            
            // Promt use for new value
            double new_value;
            std::cin >> new_value;

            // Save the new value
            User.hourly_pay = new_value;

            Config.save();

            // Display success message
            msg("Value Successfully Set", "success", true);

            sleep(1500);

            return;

        // Wipe data
        case 2:

            // Print are you sure message
            std::cout << " Wipe all data? " << Style.BLU << "[y/n] -> " << Style.END;
            char choice;
            std::cin >> choice;

            // Exit if no
            if (choice == 'n') {return;}

            // Print are you sure sure message
            std::cout << " Are you sure? " << Style.BLU << "[y/n] -> " << Style.END;
            std::cin >> choice;

            // Exit if no
            if (choice == 'n') {return;}

            // Wipe all data
            std::filesystem::remove("config.cfg");
            std::filesystem::remove("loggedwork.csv");

            msg("Data Wiped", "info", true);

            sleep(1500);

            return;

        // Return
        case 0:
            return;

    }

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