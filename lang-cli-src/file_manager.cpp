#include "file_manager.h"
#include "console_manager.h"
#include <iostream>
#include <fstream>
#include <curl/curl.h>
#include <iomanip> // For std::setw
#include <chrono> // For time tracking
#include <iomanip> // For std::setw
#include <termios.h> // For disabling keyboard input
#ifdef _WIN32
    #include <conio.h>  // Windows specific for _getch()
#else
    #include <termios.h>  // POSIX terminal control
    #include <unistd.h>   // For STDIN_FILENO
#endif

bool file_exists(const std::string & path) {
    std::ifstream f(path.c_str());
    return f.good();
}

bool file_is_empty(const std::string & path) {
    std::ifstream f;
    f.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    f.open(path.c_str(), std::ios::in | std::ios::binary | std::ios::ate);
    return f.tellg() == 0;
}

bool file_create(const std::string & path) {
    std::ofstream f(path.c_str());
    return f.good();
}

bool json_file_create(const std::string & path) {
    std::ofstream f(path.c_str());
    f << "{}";
    return f.good();
}

// Function to display progress bar with speed
void show_progress_bar(double percentage, double speed) {
    int bar_width = 50;
    std::cout << "[";
    int pos = static_cast<int>(bar_width * percentage);
    for (int i = 0; i < bar_width; ++i) {
        if (i < pos) std::cout << "=";
        else if (i == pos) std::cout << ">";
        else std::cout << " ";
    }
    // Set fixed and precision for both percentage and speed
    std::cout << "] " << std::fixed << std::setprecision(2) << (percentage * 100.0) << " % "
              << std::fixed << std::setprecision(2) << speed << " MB/s\r";
    std::cout.flush();
}

// Callback function to write the downloaded data to a file
static size_t write_data(void* buffer, size_t size, size_t nmemb, void* userp) {
    std::ofstream* ofs = static_cast<std::ofstream*>(userp);
    ofs->write(static_cast<char*>(buffer), size * nmemb);
    return size * nmemb;
}

static int progress_callback(void* ptr, curl_off_t total, curl_off_t now, curl_off_t, curl_off_t) {
    static auto last_update_time = std::chrono::steady_clock::now(); // Track time of the last update
    static curl_off_t prev_downloaded = 0;
    static double accumulated_time = 0.0; // To accumulate time for more stable speed calculation

    // Define the minimum interval between updates (e.g., 0.5 seconds)
    constexpr double update_interval = 0.5; 

    // Calculate elapsed time since the last update
    auto current_time = std::chrono::steady_clock::now();
    std::chrono::duration<double> elapsed_since_last_update = current_time - last_update_time;

    if (elapsed_since_last_update.count() >= update_interval && total > 0) {
        std::chrono::duration<double> elapsed_seconds = current_time - last_update_time;

        // Calculate speed in MB/s
        curl_off_t downloaded = now - prev_downloaded;
        accumulated_time += elapsed_seconds.count();

        double speed = 0.0;
        if (accumulated_time > 0) {
            speed = static_cast<double>(downloaded) / (1024 * 1024) / accumulated_time; // In MB/s
        }

        // Update progress bar
        double progress = static_cast<double>(now) / static_cast<double>(total);
        show_progress_bar(progress, speed);

        // Reset tracking variables
        last_update_time = current_time;
        prev_downloaded = now;
        accumulated_time = 0.0; // Reset accumulated time for the next interval
    }

    return 0;
}

bool download_file(const std::string& url, const std::string& file_path) {
    CURL* curl;
    CURLcode res;

    std::ofstream file(file_path, std::ios::binary);

    if (!file) {
        // std::cerr << "Error: Could not open file " << file_path << " for writing.\n";
        // print_error("Could not open file" + file_path + " for writing.");
        print_error(("Could not open file " + file_path + " for writing.").c_str());
        return false;
    }

    curl = curl_easy_init();
    if (curl) {
        // Set the URL for the download
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

        // Follow HTTP redirects if necessary
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

        // Write the data to a file
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &file);

        // Set progress function
        curl_easy_setopt(curl, CURLOPT_XFERINFOFUNCTION, progress_callback);
        curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L); // Enable progress callback

        // Enable verbose output for debugging (optional)
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

        // Perform the download
        res = curl_easy_perform(curl);

        // Check for errors
        if (res != CURLE_OK) {
            std::cerr << "Error: Download failed with error: " << curl_easy_strerror(res) << "\n";
            curl_easy_cleanup(curl);
            return false;
        }

        // Clean up
        curl_easy_cleanup(curl);
        file.close();
        std::cout << "\nDownload successful: " << file_path << "\n";
        return true;
    } else {
        std::cerr << "Error: Could not initialize CURL.\n";
        return false;
    }
}
