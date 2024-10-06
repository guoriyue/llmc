#include <iostream>
#include <fstream>
#include <curl/curl.h>
#include <iomanip> // For std::setw

// Function to display progress bar
void show_progress_bar(double percentage) {
    int bar_width = 50;
    std::cout << "[";
    int pos = static_cast<int>(bar_width * percentage);
    for (int i = 0; i < bar_width; ++i) {
        if (i < pos) std::cout << "=";
        else if (i == pos) std::cout << ">";
        else std::cout << " ";
    }
    std::cout << "] " << int(percentage * 100.0) << " %\r";
    std::cout.flush();
}

// Callback function to write the downloaded data to a file
size_t write_data(void* buffer, size_t size, size_t nmemb, void* userp) {
    std::ofstream* ofs = static_cast<std::ofstream*>(userp);
    ofs->write(static_cast<char*>(buffer), size * nmemb);
    return size * nmemb;
}

// Progress callback function for curl
int progress_callback(void* ptr, curl_off_t total, curl_off_t now, curl_off_t, curl_off_t) {
    if (total > 0) {
        double progress = static_cast<double>(now) / static_cast<double>(total);
        show_progress_bar(progress);
    }
    return 0;
}

static bool download_file(const std::string& url, const std::string& file_path) {
    CURL* curl;
    CURLcode res;
    std::ofstream file(file_path, std::ios::binary);

    if (!file) {
        std::cerr << "Error: Could not open file " << file_path << " for writing.\n";
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
