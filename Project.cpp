#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>

class Arrow {
public:
    std::string color;
    std::string direction;
    bool visited;
    int index;

    Arrow(const std::string& input_string, int ind) {
        this->input_string = input_string;
        parseInputString();
        visited = false;
        index = ind;
    }

private:
    std::string input_string;

    void parseInputString() {
        size_t pos = input_string.find('-');
        if (pos != std::string::npos) {
            color = input_string.substr(0, pos);
            direction = input_string.substr(pos + 1);
        } else {
            color = "";
            direction = input_string;
        }
    }
};

std::unordered_map<const Arrow*, std::vector<const Arrow*>> createAdjacencyList(const std::vector<std::vector<Arrow>>& matrix) {
    std::unordered_map<const Arrow*, std::vector<const Arrow*>> adjList;

    // Define directions and their corresponding changes in row and column indices
    std::unordered_map<std::string, std::pair<int, int>> directions = {
        {"N", {-1, 0}}, {"S", {1, 0}},
        {"E", {0, 1}},  {"W", {0, -1}},
        {"NE", {-1, 1}}, {"NW", {-1, -1}},
        {"SE", {1, 1}}, {"SW", {1, -1}},
        {"O", {0, 0}}  // Not sure what "O" represents, assuming no movement
    };

    int rows = matrix.size();
    int cols = matrix[0].size();

    for (int row = 0; row < rows; ++row) {
        for (int col = 0; col < cols; ++col) {
            adjList[&matrix[row][col]] = {};
            std::string currentDirection = matrix[row][col].direction;
            std::string currentColor = matrix[row][col].color;

            if (currentDirection == "O") {
                adjList[&matrix[row][col]].push_back(nullptr);
                continue;
            }

            auto [dx, dy] = directions[currentDirection];
            int k = row + dx;
            int l = col + dy;

            while (0 <= k && k < rows && 0 <= l && l < cols) {
                if (matrix[k][l].color != currentColor) {
                    adjList[&matrix[row][col]].push_back(&matrix[k][l]);
                }
                k += dx;
                l += dy;
            }
        }
    }

    return adjList;
}

std::vector<std::vector<Arrow>> readFileAndCreate2DArray(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "Error opening file: " << filePath << std::endl;
        exit(1);
    }

    int n, m;
    file >> n >> m;

    std::vector<std::vector<Arrow>> twoDArray;
    int index = 0;

    for (int i = 0; i < n; ++i) {
        std::vector<Arrow> rowData;
        std::string dataPoint;

        for (int j = 0; j < m; ++j) {
            file >> dataPoint;
            Arrow arrow(dataPoint, index);
            rowData.push_back(arrow);
            index++;
        }

        twoDArray.push_back(rowData);
    }

    file.close();

    return twoDArray;
}

int main() {
    std::string filePath = "tiny.txt";
    std::vector<std::vector<Arrow>> matrix = readFileAndCreate2DArray(filePath);
    std::unordered_map<const Arrow*, std::vector<const Arrow*>> adjList = createAdjacencyList(matrix);

    // Print the adjacency list representation of the graph
    for (const auto& pair : adjList) {
        const Arrow* arrow = pair.first;
        const std::vector<const Arrow*>& neighbors = pair.second;

        // Print arrow details
        std::cout << "Arrow: Index = " << arrow->index
                  << ", Color = " << arrow->color
                  << ", Direction = " << arrow->direction
                  << ", Visited = " << (arrow->visited ? "true" : "false") << std::endl;

        // Print adjacent arrows
        std::cout << "  Adjacent Arrows: ";
        for (const auto& neighbor : neighbors) {
            if (neighbor) {
                std::cout << neighbor->index << " ";
            } else {
                std::cout << "nullptr ";
            }
        }
        std::cout << std::endl;
    }

    return 0;
}
