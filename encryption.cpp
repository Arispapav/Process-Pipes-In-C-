#include <iostream>
#include <string>
#include <cctype>

using namespace std;

string passkey = "";  // Global passkey variable

// Function prototypes
string encryptVigenere(const string& text, const string& key);
string decryptVigenere(const string& text, const string& key);
void handleCommand(const string& commandLine);

int main() {
    string commandLine;

    // Continuously read from stdin
    while (getline(cin, commandLine)) 
        {
        handleCommand(commandLine);
            if (commandLine == "QUIT") 
                {   
                break;
                }
        }
    return 0;
}

// Encrypt using Vigenère cipher
string encryptVigenere(const string& text, const string& key) {
    string result = "";
    int keyLen = key.length();

    for (size_t i = 0; i < text.length(); ++i) {
        // Only encrypt alphabetic characters
        if (isalpha(text[i])) 
            {
            char base = isupper(text[i]) ? 'A' : 'a';
            result += (text[i] - base + (toupper(key[i % keyLen]) - 'A')) % 26 + base;
            } 
        else 
            {
            result += text[i];  // Non-alphabetic characters remain unchanged
            }
    }

    return result;
}

// Decrypt using Vigenère cipher
string decryptVigenere(const string& text, const string& key) {
    string result = "";
    int keyLen = key.length();

    for (size_t i = 0; i < text.length(); ++i)
        {
            // Only decrypt alphabetic characters
            if (isalpha(text[i])) {
                char base = isupper(text[i]) ? 'A' : 'a';
                result += (text[i] - base - (toupper(key[i % keyLen]) - 'A') + 26) % 26 + base;
            } 
            else 
            {
                result += text[i];  // Non-alphabetic characters remain unchanged
            }
        }

    return result;
}

// Handle a command by parsing and executing the appropriate function
void handleCommand(const string& commandLine) 
    {
    // Find the first space to separate command from argument
    size_t spaceIndex = commandLine.find(' ');
    string command = commandLine.substr(0, spaceIndex);
    string argument = (spaceIndex == string::npos) ? "" : commandLine.substr(spaceIndex + 1);

    if (command == "PASSKEY") {
        passkey = argument;  // Set the passkey
        cout << "RESULT Passkey set successfully.\n";

    } else if (command == "ENCRYPT") 
    {
        if (passkey.empty()) { // validate existing passkey 
            cout << "ERROR No passkey set.\n";
        } else 
        {
            string encryptedText = encryptVigenere(argument, passkey); // encrypt message and send result to stdin
            cout << "RESULT " << encryptedText << "\n";
        }

    } else if (command == "DECRYPT") 
    {
        if (passkey.empty()) {// validate existing passkey 
            cout << "ERROR No passkey set.\n";
        } else {
            string decryptedText = decryptVigenere(argument, passkey); // decrypt message and send result to stdin
            cout << "RESULT " << decryptedText << "\n";
        }

    } else if (command == "QUIT") {
        cout << "RESULT Exiting program.\n";
        exit(0);  // Exit the program

    } else {
        // Handle missinput
        cout << "ERROR Unknown command.\n";
    }
}


