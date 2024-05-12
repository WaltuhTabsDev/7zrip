#include <iostream>
#include <fstream>
#include <cstdlib>
#include <string>
#include <vector>
#include <algorithm>

#ifdef _WIN32
    #include <windows.h>
    #define CLEAR_SCREEN "cls"
#else
    #include <term.h>
    #define CLEAR_SCREEN "clear"
#endif

int zipsystem = 0;

#ifdef _WIN32
bool zipprompt_triggered = false;
void simulate_keypress() {
    INPUT ip;
    ip.type = INPUT_KEYBOARD;
    ip.ki.wScan = 0; // hardware scan code for key
    ip.ki.time = 0;
    ip.ki.dwExtraInfo = 0;

    // Press the "N" key
    ip.ki.wVk =  0x4E; // virtual-key code for the "N" key
    ip.ki.dwFlags = 0; // 0 for key press
    SendInput(1, &ip, sizeof(INPUT));
    ip.ki.wVk =  0x0D;
    SendInput(1, &ip, sizeof(INPUT));
}
#endif

int check7zip() {
    FILE* pipe = popen("7z", "r");
    if (pipe) {
        pclose(pipe);
        zipsystem = 1;
        return 0;
    }

    pipe = popen("7za", "r");
    if (pipe) {
        pclose(pipe);
        zipsystem = 2;
        return 0;
    }

    pipe = popen("xz", "r");
    if (pipe) {
        pclose(pipe);
        zipsystem = 3;
        return 0;
    }

    std::cout << "7z command not found. Please make sure 7z is installed." << std::endl;
    return -1;
}

std::string generate_random_pass(int digit_count) {
    if (digit_count <= 0) {
        std::cout << "Invalid digit count. Please enter a positive number." << std::endl;
        return "";
    }
    
    std::string password = "";
    for (int i = 0; i < digit_count; ++i) {
        int random_digit = rand() % 10; // Generate a random digit from 0 to 9
        password += std::to_string(random_digit);
    }
    
    return password;
}

int get_user_digit_count() {
    int digit_count;
    while (true) {
        std::cout << "How many digits should the random number have? ";
        std::cin >> digit_count;
        if (digit_count > 0) {
            return digit_count;
        } else {
            std::cout << "Please enter a positive number." << std::endl;
        }
    }
}

int run_7z(std::string password, std::string path_to_file) {
    std::string command;
    if (zipsystem == 1) {
        command = "7z x "  + path_to_file + " -p" + password;
    } else if (zipsystem == 2) {
        command = "7za x " + path_to_file + " -p" + password;
    } else if (zipsystem == 3) {
        command = "xz -d -c -k -T0 -q -e -p" + password + " " + path_to_file + " > " + path_to_file.substr(0, path_to_file.find_last_of('.'));
    } else {
        std::cout << "7z command not found. Please make sure 7z is installed." << std::endl;
        return -1;
    }

    FILE* pipe = popen(command.c_str(), "r");
    if (!pipe) {
        std::cout << "Failed to run the 7z command." << std::endl;
        return -1;
    }

    int result = pclose(pipe);
    if (result == -1) {
        std::cout << "Failed to get the return code of the 7z command." << std::endl;
        return -1;
    }
    return result;
}

void save_progress(std::vector<std::string> failed_passes, std::string path_to_file) {
    std::ofstream file("data/" + path_to_file.substr(0, path_to_file.find_last_of('.')) + ".txt");
    if (!file.is_open()) {
        std::cout << "Failed to open progress file for writing." << std::endl;
        return;
    }
    for (const std::string& password : failed_passes) {
        file << password << std::endl;
    }
    file.close();
}

void crack_7z_file() {
    std::cout << R"(
 _____ _________  ___ ____
|___  |__  /  _ \|_ _|  _ \
   / /  / /| |_) || || |_) |
  / /  / /_|  _ < | ||  __/
 /_/  /____|_| \_\___|_|
-------------------------
| Written By WaltuhTabs |
-------------------------
)" << std::endl;
    std::cout << "7ZRIP is a brute-force tool that cracks rar, 7z, and zip passwords" << std::endl;
    std::cout << "NOTE: This tool was made for educational purposes only. Use at your own risk." << std::endl;
    std::cout << std::endl;

    std::vector<std::string> failed_passes;
    std::string path_to_file;
    std::cout << "What is the path to the 7z file? ";
    std::cin >> path_to_file;
    while (!std::ifstream(path_to_file)) {
        std::cout << "Invalid file path. Please enter a valid path." << std::endl;
        std::cout << "What is the path to the 7z file? ";
        std::cin >> path_to_file;
    }

    std::string method;
    std::cout << "Generate brute force (Only numbers) (g) or get from a file (f) (File is faster)? ";
    std::cin >> method;
    while (method != "g" && method != "f") {
        std::cout << "Invalid method. Please choose 'g' for generate or 'f' for file." << std::endl;
        std::cout << "Generate brute force (Only numbers) (g) or get from a file (f)? ";
        std::cin >> method;
    }

    std::string log_false_pass;
    std::cout << "Do you want false passwords to be written (Y/N)? ";
    std::cin >> log_false_pass;
    while (log_false_pass != "Y" && log_false_pass != "N") {
        std::cout << "Invalid. Please choose 'Y' to agree or 'N' to disagree." << std::endl;
        std::cout << "Do you want false passwords to be written (Y/N)? ";
        std::cin >> log_false_pass;
    }

    int digit_count;
    std::string txtfile;
    std::vector<std::string> passwords;
    if (method == "g") {
        digit_count = get_user_digit_count();
    } else if (method == "f") {
        std::cout << "What is the password file? ";
        std::cin >> txtfile;
        while (!std::ifstream(txtfile)) {
            std::cout << "Invalid file path. Please enter a valid path." << std::endl;
            std::cout << "What is the password file? ";
            std::cin >> txtfile;
        }

        std::ifstream file(txtfile);
        if (file.is_open()) {
            std::string password;
            while (std::getline(file, password)) {
                passwords.push_back(password);
            }
            file.close();
        }

        if (passwords.empty()) {
            std::cout << "Password file is empty. Please provide a valid password file." << std::endl;
            return;
        }
    }

    std::cout << "Cracking Started" << std::endl;

    std::string progress_file = "data/" + path_to_file.substr(0, path_to_file.find_last_of('.')) + ".txt";
    if (std::ifstream(progress_file)) {
        std::string resume;
        std::cout << "A progress file was found. Do you want to resume cracking from where you left off? (Y/N) ";
        std::cin >> resume;
        while (resume != "Y" && resume != "N") {
            std::cout << "Invalid input. Please choose 'Y' to resume or 'N' to start from the beginning." << std::endl;
            std::cout << "Do you want to resume cracking from where you left off? (Y/N) ";
            std::cin >> resume;
        }

        if (resume == "Y") {
            std::ifstream file(progress_file);
            if (file.is_open()) {
                std::string password;
                while (std::getline(file, password)) {
                    failed_passes.push_back(password);
                }
                file.close();
            }
        } else {
            std::remove(progress_file.c_str());
        }
    }

    try {
        while (true) {
            std::string password;
            if (method == "g") {
                password = generate_random_pass(digit_count);
            } else if (method == "f") {
                password = passwords[rand() % passwords.size()];
            }

            if (std::find(failed_passes.begin(), failed_passes.end(), password) != failed_passes.end()) {
                continue;
            }

            int result = run_7z(password, path_to_file);
            if (result == 0) {
                std::cout << "Password FOUND: " << password << " | After " << failed_passes.size() << " tries..." << std::endl;
                std::ofstream file("found_pass_" + path_to_file.substr(path_to_file.find_last_of('/') + 1) + ".txt");
                if (file.is_open()) {
                    file << password;
                    file.close();
                }
                #ifdef _WIN32
                    system("pause");
                #else
                    system("read");
               #endif
                break;
            } else if(result == 2) {
                failed_passes.push_back(password);
                if (log_false_pass == "Y") {
                    std::cout << "Password FAILED | TRY NUM#" << failed_passes.size() << " | " << password << std::endl;
                }
            }
            save_progress(failed_passes, path_to_file);
        }
    } catch (...) {
        std::cout << "Cracking process interrupted. Saving progress..." << std::endl;
        save_progress(failed_passes, path_to_file);
    }
}

int main() {
    srand(time(NULL));
    system(CLEAR_SCREEN);
    if (check7zip() != 0) {
        return -1;
    }
    crack_7z_file();
    return 0;
}
