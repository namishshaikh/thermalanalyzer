#include <iostream>
#include <fstream>
#include <vector>
#include <experimental/filesystem>
#include <cstdlib>
#include <ctime>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

namespace fs = std::experimental::filesystem;

#define PORT 8080
#define IMAGE_FOLDER "mock_images"

std::vector<std::string> getImageFiles() {
    std::vector<std::string> imageFiles;
    std::string folderPath="mock_images";
    for(const auto& entry : fs::directory_iterator(folderPath)){
       if(fs::is_regular_file(entry.status())){
          std::string path= entry.path().string();
          if(path.find(".jpg")!=std::string::npos ||
             path.find(".png")!=std::string::npos ||
             path.find(".jpeg")!=std::string::npos ||
             path.find(".bmp")!=std::string::npos){
             imageFiles.push_back(path);
}
  }
}
    return imageFiles;
}

std::vector<char> readImageAsBytes(const std::string& filePath) {
    std::ifstream file(filePath, std::ios::binary);
    std::vector<char> buffer((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    return buffer;
}

int main() {
    srand(time(0));
    std::vector<std::string> imageFiles = getImageFiles();

    if (imageFiles.empty()) {
        std::cerr << "No images found in folder: " << IMAGE_FOLDER << std::endl;
        return 1;
    }

    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    bind(server_fd, (struct sockaddr*)&address, sizeof(address));
    listen(server_fd, 3);

    std::cout << "Mock server running on port " << PORT << "...\n";

    while (true) {
        new_socket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen);
        if (new_socket < 0) {
            std::cerr << "Failed to accept connection\n";
            continue;
        }

        std::string randomImage = imageFiles[rand() % imageFiles.size()];
        std::vector<char> imageData = readImageAsBytes(randomImage);

        int size = imageData.size();
        send(new_socket, &size, sizeof(size), 0);
        send(new_socket, imageData.data(), size, 0);

        std::cout << "Sent image: " << randomImage << " (" << size << " bytes)\n";

        close(new_socket);
    }

    return 0;
}
