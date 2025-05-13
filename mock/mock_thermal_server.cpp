#include <iostream>
#include <cstdlib>
#include <ctime>
#include <unistd.h>
#include <netinet/in.h>
#include <cstring>
#include <arpa/inet.h>

#define PORT 12345
#define WIDTH  640
#define HEIGHT 512

void generateThermalImage(uint8_t* buffer, int width, int height) {
    for (int i = 0; i < width * height; ++i) {
        buffer[i] = rand() % 256; // Simulated thermal pixel (0-255)
    }
}

int main() {
    srand(time(nullptr));

    int server_fd, client_fd;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    // Create socket file descriptor
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == 0) {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    // Attach socket to port 12345
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY; // 0.0.0.0
    address.sin_port = htons(PORT);

    // Bind
    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    // Listen
    if (listen(server_fd, 1) < 0) {
        perror("Listen");
        exit(EXIT_FAILURE);
    }

    std::cout << "Mock thermal server listening on port " << PORT << "...\n";

    // Accept client connection
    client_fd = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen);
    if (client_fd < 0) {
        perror("Accept");
        exit(EXIT_FAILURE);
    }

    std::cout << "Client connected! Sending mock thermal images...\n";

    // Allocate buffer
    uint8_t* imageBuffer = new uint8_t[WIDTH * HEIGHT];

    while (true) {
        generateThermalImage(imageBuffer, WIDTH, HEIGHT);

        // Send image size (first)
        int imageSize = WIDTH * HEIGHT;
        send(client_fd, &imageSize, sizeof(imageSize), 0);

        // Send image buffer
        send(client_fd, imageBuffer, imageSize, 0);

        std::cout << "Mock image sent.\n";
        sleep(2); // simulate delay
    }

    delete[] imageBuffer;
    close(client_fd);
    close(server_fd);

    return 0;
}

