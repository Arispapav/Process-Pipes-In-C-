#include <iostream>
#include <unistd.h>
#include <string.h>
#include <list>
#include <sys/wait.h>
#include <fcntl.h>
#include <fstream>
using namespace std;

// Function Prototypes 
string printMenu(list<string>&, int);
int printHistory(const list<string>&);
string historySearch(const list<string>&, int);

// Helper function to send and receive messages via pipes
string sendAndReceive(int writeFD, int readFD, const string& message) {
    // Send message to the encryption program
    write(writeFD, message.c_str(), message.size());

    // Receive response from the encryption program
    char buffer[256];
    ssize_t bytesRead = read(readFD, buffer, sizeof(buffer) - 1);
    if (bytesRead > 0) {
        buffer[bytesRead] = '\0';
        return string(buffer);
    }
    return "ERROR: No response from encryption program.";
}
// Main driver Program
int main (int argc, char* argv[]) {
    string filename = argv[1]; // Log file name for logger
    int logPipe[2], encPipe[2], encResponsePipe[2];
    list<string> historyObj;

    // Create pipes for logger and encryption program
    if (pipe(logPipe) == -1 || pipe(encPipe) == -1 || pipe(encResponsePipe) == -1) {
        perror("pipe");
        return 1;
    }

    // Fork the logger process
    pid_t loggerPID = fork();
    if (loggerPID == -1) {
        perror("fork");
        return 1;
    }

    if (loggerPID == 0) { // Logger child process
        close(logPipe[1]); // Close write end of logger pipe
        dup2(logPipe[0], STDIN_FILENO); // Redirect stdin to read from pipe
        execl("./logger", "./logger", filename.c_str(), NULL); // Execute logger
        perror("execl");
        return 1;
    }

    // Fork the encryption process
    pid_t encPID = fork();
    if (encPID == -1) {
        perror("fork");
        return 1;
    }

    if (encPID == 0) { // Encryption child process
        close(encPipe[1]); // Close write end of encryption pipe
        close(encResponsePipe[0]); // Close read end of response pipe
        dup2(encPipe[0], STDIN_FILENO); // Redirect stdin to read from pipe
        dup2(encResponsePipe[1], STDOUT_FILENO); // Redirect stdout to write to pipe
        execl("./encryption", "./encryption", NULL); // Execute encryption program
        perror("execl");
        return 1;
    }

    // Parent process - Driver Program
    close(logPipe[0]); // Close read end of logger pipe
    close(encPipe[0]); // Close read end of encryption pipe
    close(encResponsePipe[1]); // Close write end of response pipe

    int choice = 0;
    while (choice != 5) 
        {
        string message, userString;

        cout << "Choose an option:\n"
             << "1. Password\n"
             << "2. Encrypt\n"
             << "3. Decrypt\n"
             << "4. History\n"
             << "5. Quit\n";
        cin >> choice;

        if (choice == 1) 
            {
                message = "PASSKEY " + printMenu(historyObj, 0) + "\n"; // get string password
                string logMessage = "PASSWORD New Password Selected\n"; // message for the logger 
                write(logPipe[1], logMessage.c_str(), logMessage.size()); // set password in encryption program
                sendAndReceive(encPipe[1], encResponsePipe[0], message); // update logger
                
            } 
        else if (choice == 2) 
            {
                message = "ENCRYPT " + printMenu(historyObj, 1) + "\n"; // get string to encrypt 
                string response = sendAndReceive(encPipe[1], encResponsePipe[0], message); // pass message in to get encryped string
                string logMessage = "ENCRYPT " + response; 
                write(logPipe[1], logMessage.c_str(), logMessage.size()); // updated logger with result msg
                cout << response; // output the result to the user
            } 
        else if (choice == 3) 
            {
                message = "DECRYPT " + printMenu(historyObj, 1) + "\n"; // get string to decrypt 
                string response = sendAndReceive(encPipe[1], encResponsePipe[0], message); // pass message in to get decrypted string
                string logMessage = "DECRYPT " + response; 
                write(logPipe[1], logMessage.c_str(), logMessage.size()); // update logger with result msg
                cout << response; // output result to the user 
            } 
        else if (choice == 4)
            {
                printHistory(historyObj); // print history
            } 
        else if (choice == 5) 
            {
                // Send QUIT to both logger and encryption programs
                write(logPipe[1], "QUIT\n", 5);
                sendAndReceive(encPipe[1], encResponsePipe[0], "QUIT\n");
            } 
        else 
            {
                cout << "Invalid option.\n";
            }
        }   
    // closing pipes
    close(logPipe[1]);
    close(encPipe[1]);
    close(encResponsePipe[0]);
    wait(nullptr);
    wait(nullptr);
    return 0;
}

// Passing in history obj for accessing and storing new strings
string printMenu(list<string>& historyObj, int histFlag) 
    {
    int choice1, choice2;
    string userString = "";
      
    cout << "Choose an option:\n"
         << "1. String from History\n"
         << "2. New String\n";
         
    cin >> choice1;
    
    if (choice1 == 1)  
        {
            int hSize = printHistory(historyObj); // print out history and get current size
            cout << "Pick an option or select 0 to go back\n";
            cin >> choice2;
            if (choice2 > 0 && choice2 <= hSize) //validate user choice 
                { 
                userString = historySearch(historyObj, choice2); // set user string to item in history
                } 
            else if (choice2 == 0) // recursive call if user wants to go back
            {
                return printMenu(historyObj, histFlag);
            }
            else { // user input not accepted
                cout << "Invalid option.\n";
                return printMenu(historyObj, histFlag);
            }
        } 
    else if (choice1 == 2)
        {
            cout << "Please Enter New String:\n";
            cin.ignore(); // Clear buffer

            getline(cin, userString);

            if (histFlag == 1) { // if its not a new password being set we add it to history
                historyObj.push_back(userString); // Add new string to history
            }
        }
    else 
        { // invalid choice we recall 
            cout << "Invalid choice.\n";
            return printMenu(historyObj, histFlag);
        }

    return userString;
    }

int printHistory(const list<string>& historyObj) {
    int counter = 1;
    if (historyObj.empty()) {
        cout << "History is empty\n";
    } else {
        cout << "History List:\n";
        for (const auto& entry : historyObj) {
            cout << counter++ << ". " << entry << endl;
        }
    }
    return counter;
}

string historySearch(const list<string>& historyObj, int val) {
    if (historyObj.empty()) {
        cout << "History is empty\n";
        return "";
    }
    auto it = historyObj.begin();
    advance(it, val - 1); // Move iterator to the correct position
    return *it;
}
