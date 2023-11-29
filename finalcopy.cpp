#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <cmath>
#include <algorithm>

class Arrow {
public:
    std::string shade;
    std::string direction;
    bool explored;
    int distanceFromOrigin;
    int identifier;
    int row, column;

    Arrow(const std::string& inputStr, int id, int r, int c) {
        parseInput(inputStr);
        explored = false;
        identifier = id;
        distanceFromOrigin = 0;
        row = r;
        column = c;
    }

private:

    void parseInput(std::string inputStr) {
        size_t pos = inputStr.find('-');
        if (pos != std::string::npos) {
            shade = inputStr.substr(0, pos);
            direction = inputStr.substr(pos + 1);
        } else {
            shade = "";
            direction = inputStr;
        }
    }
};

std::unordered_map< Arrow*, std::vector< Arrow*>> createAdjacencyList(std::vector<std::vector<Arrow>>& matrix) {
    std::unordered_map< Arrow*, std::vector< Arrow*>> adjList;

    std::unordered_map<std::string, std::pair<int, int>> directions = {
        {"N", {-1, 0}}, {"S", {1, 0}},
        {"E", {0, 1}},  {"W", {0, -1}},
        {"NE", {-1, 1}}, {"NW", {-1, -1}},
        {"SE", {1, 1}}, {"SW", {1, -1}},
        {"O", {0, 0}}  
    };

    int rows = matrix.size();
    int cols = matrix[0].size();

    for (int r = 0; r < rows; ++r) {
        for (int c = 0; c < cols; ++c) {
            adjList[&matrix[r][c]] = {};
            std::string currDirection = matrix[r][c].direction;
            std::string currShade = matrix[r][c].shade;

            if (currDirection == "O") {
                adjList[&matrix[r][c]].push_back(nullptr);
                continue;
            }

            int dx = directions[currDirection].first;
            int dy = directions[currDirection].second;
            int k = r + dx;
            int l = c + dy;

            while (0 <= k && k < rows && 0 <= l && l < cols) {
                if (matrix[k][l].shade != currShade) {
                    adjList[&matrix[r][c]].push_back(&matrix[k][l]);
                }
                k += dx;
                l += dy;
            }
        }
    }

    return adjList;
}

std::vector<std::vector<Arrow>> readAndCreate2DArray(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "Error: Unable to open file: " << filePath << std::endl;
        exit(1);
    }

    int rows, columns;
    file >> rows >> columns;

    std::vector<std::vector<Arrow>> twoDArray;
    int id = 0;

    for (int r = 0; r < rows; ++r) {
        std::vector<Arrow> rowData;
        std::string dataPoint;

        for (int c = 0; c < columns; ++c) {
            file >> dataPoint;
            Arrow arrow(dataPoint, id, r, c);
            rowData.push_back(arrow);
            id++;
        }

        twoDArray.push_back(rowData);
    }

    file.close();

    return twoDArray;
}

std::vector<std::vector<int>> computePath(Arrow* parentNode, std::unordered_map< Arrow*, std::vector< Arrow*>>& adjList, std::vector<std::vector<Arrow>>& matrix){
    parentNode->explored = true;
    std::vector<std::vector<int>> result;
    for(int i = 0; i < adjList[parentNode].size(); i++){
        Arrow* childNode = adjList[parentNode][i];
        if (childNode->direction == "O"){
            result = {};
            std::vector<int> path = {childNode->identifier, childNode->row, childNode->column};
            result.push_back(path);
            return result;
        }
        else{
            if (childNode->explored == false){
                result = computePath(childNode, adjList, matrix);
                if (result.size() != 0){
                    std::vector<int> path = {childNode->identifier, childNode->row, childNode->column};
                    result.push_back(path);
                    return result;
                }
            }
            result = {};
        }
    }
    return result;
}

void calculateDistAndDirection(std::vector<std::vector<int>>& coordinates, const std::string& outputFilename) {
    std::ofstream outputFile(outputFilename);

    if (!outputFile.is_open()) {
        std::cout << "Error: Unable to open file!";
        return;
    }

    for (int i = coordinates.size() - 1; i > 0; --i) {
        int x1 = coordinates[i][1];
        int x2 = coordinates[i - 1][1];
        int dx = x1 - x2;
        int y1 = coordinates[i][2];
        int y2 = coordinates[i - 1][2];
        int dy = y1 - y2;
        int distance = std::max(std::abs(dx), std::abs(dy));
        
        std::string direction;

        if (dx != 0){
            if (dx > 0){
                direction = direction + "N";
            }
            else{
                direction = direction + "S";
            }
        }
        if (dy != 0){
            if (dy > 0){
                direction = direction + "W";
            }
            else{
                direction = direction + "E";
            }
        }
        outputFile << distance << direction << " ";   
    }
    outputFile.close();
}

int main() {
    std::string filePath = "apollodiana.txt";
    std::vector<std::vector<Arrow>> matrix = readAndCreate2DArray(filePath);
    std::unordered_map< Arrow*, std::vector< Arrow*>> adjList = createAdjacencyList(matrix);
    std::vector<std::vector<int>> answer = computePath(&matrix[0][0], adjList, matrix);
    answer.push_back({0, 0, 0});

    calculateDistAndDirection(answer, "Output.txt");

    return 0;
}
