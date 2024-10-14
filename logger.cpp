#include <iostream>
#include <sstream>
#include <vector>
#include <ctime>
#include <fstream>  

// Function Prototypes
void logger(std::ofstream&);
std::string getCurrentDateTime();
std::vector<std::string> createFormatLine(std::string input);

// Create the logger and open the log file
int main(int argc, char* argv[]) {
    // Create and open the log file
    std::ofstream outFile((argv[1]), std::ios::app);  

    if (!outFile) {
        std::cerr << "Error opening the log file\n";
        return 0;
    }

    // Call the logger function to start logging
    logger(outFile);
    
    // Close the output file
    outFile.close();  
    return 0;
}

// Logger function for reading input and writing logs
void logger(std::ofstream &outFile) {
    std::string line;

    // Write the start log message
    std::string currentTime = getCurrentDateTime();
    outFile << currentTime << " [START] Logging Started\n"; 
    outFile.flush();  

    // Loop waiting for input from stdin of main
    while (std::getline(std::cin, line)) {
        // Format input line into a vector of words
        std::vector<std::string> formatLine = createFormatLine(line);
        
        // Get current time for each log entry
        currentTime = getCurrentDateTime();
        outFile << currentTime << " "; 

        // Write the formatted log line
        for (size_t i = 0; i < formatLine.size(); ++i) {
            if (i == 0) {
                if (formatLine[0] == "QUIT") {
                    // Log stop event and exit
                    outFile << "[" << formatLine[0] << "] Logging Stopped\n";
                    outFile.flush();
                    return; 
                }
                outFile << "[" << formatLine[i] << "] ";
            } else {
                outFile << formatLine[i] << " ";
            }
        }
        outFile << "\n";
        outFile.flush();  // Flush the output after every log entry
    }
}

// Function to get the current date and time in YYYY-MM-DD HH:MM format
std::string getCurrentDateTime() {
    // Get current time as time_t
    std::time_t t = std::time(nullptr);

    // Convert to local time structure
    std::tm* now = std::localtime(&t);

    char buffer[20];  

    // Format: YYYY-MM-DD HH:MM 
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M", now);

    return std::string(buffer);
}

// Function to split the input line into words
std::vector<std::string> createFormatLine(std::string input) {
    std::vector<std::string> formatLine;
    std::istringstream iss(input);
    std::string word;
    
    // Parse the line by whitespace
    while (iss >> word) {
        formatLine.push_back(word);
    }
    
    return formatLine;
}
