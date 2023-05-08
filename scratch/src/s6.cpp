// Copyright (C) 2022, 2023 by Mark Melton
//

#include <iostream>
#include <vector>
#include <random>
#include <algorithm>
#include <stack>

const int SIZE = 10;

enum Direction {
    TOP,
    RIGHT,
    BOTTOM,
    LEFT
};

struct Cell {
    bool visited;
    bool walls[4];  // top, right, bottom, left

    Cell() {
        visited = false;
        std::fill(walls, walls + 4, true);
    }
};

int getRandomNumber(int min, int max) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dis(min, max);
    return dis(gen);
}


bool isValidCell(int row, int col) {
    return (row >= 0 && row < SIZE && col >= 0 && col < SIZE);
}

void removeWall(Cell& current, Cell& next, Direction direction) {
    current.walls[direction] = false;

    if (direction == TOP)
        next.walls[BOTTOM] = false;
    else if (direction == RIGHT)
        next.walls[LEFT] = false;
    else if (direction == BOTTOM)
        next.walls[TOP] = false;
    else if (direction == LEFT)
        next.walls[RIGHT] = false;
}

void generateMaze(std::vector<std::vector<Cell>>& maze, int row, int col) {
    static const int dx[] = {0, 1, 0, -1};  // right, down, left, up
    static const int dy[] = {-1, 0, 1, 0};

    maze[row][col].visited = true;

    std::vector<int> directions = {TOP, RIGHT, BOTTOM, LEFT};
    std::shuffle(directions.begin(), directions.end(), std::mt19937(std::random_device()()));

    for (int direction : directions) {
        int newRow = row + dy[direction];
        int newCol = col + dx[direction];

        if (isValidCell(newRow, newCol) && !maze[newRow][newCol].visited) {
            removeWall(maze[row][col], maze[newRow][newCol], static_cast<Direction>(direction));
            generateMaze(maze, newRow, newCol);
        }
    }
}

void generateMaze(std::vector<std::vector<Cell>>& maze) {
    generateMaze(maze, 0, 0);
}

void displayMaze(const std::vector<std::vector<Cell>>& maze) {
    for (int row = 0; row < SIZE; ++row) {
        for (int col = 0; col < SIZE; ++col) {
            std::cout << "+";
            std::cout << (maze[row][col].walls[TOP] ? "---" : "   ");
        }
        std::cout << "+\n";

        for (int col = 0; col < SIZE; ++col) {
            std::cout << (maze[row][col].walls[LEFT] ? "|" : " ");
            std::cout << "   ";
        }
        std::cout << "|\n";
    }

    for (int col = 0; col < SIZE; ++col) {
        std::cout << "+---";
    }
    std::cout << "+\n";
}

bool solveMazeDFS(std::vector<std::vector<Cell>>& maze, int row, int col, std::vector<std::pair<int, int>>& path) {
    static const int dx[] = {0, 1, 0, -1}; // right, down, left, up
    static const int dy[] = {-1, 0, 1, 0};
    if (!isValidCell(row, col) || maze[row][col].visited)
        return false;

    maze[row][col].visited = true;

    if (row == SIZE - 1 && col == SIZE - 1) {
        // Reached the end of the maze
        path.push_back(std::make_pair(row, col));
        return true;
    }

    for (int direction = 0; direction < 4; ++direction) {
        int newRow = row + dy[direction];
        int newCol = col + dx[direction];

	if (not maze[row][col].walls[direction]) {
	    if (solveMazeDFS(maze, newRow, newCol, path)) {
		// Found a valid path, add the current cell to the path
		path.push_back(std::make_pair(row, col));
		return true;
	    }
	}
    }

    return false;
}

bool solveMaze(std::vector<std::vector<Cell>>& maze, std::vector<std::pair<int, int>>& path) {
    // Reset the visited flag of cells in the maze
    for (int i = 0; i < SIZE; ++i) {
        for (int j = 0; j < SIZE; ++j) {
            maze[i][j].visited = false;
        }
    }

    // Start solving the maze from the beginning
    return solveMazeDFS(maze, 0, 0, path);
}


int main() {
    std::vector<std::vector<Cell>> maze(SIZE, std::vector<Cell>(SIZE));
    generateMaze(maze);
    // displayMaze(maze);
    std::vector<std::pair<int, int>> path;
    if (solveMaze(maze, path)) {
        // std::cout << "Solution:\n";
        // for (int i = path.size() - 1; i >= 0; --i) {
        //     std::cout << "(" << path[i].first << ", " << path[i].second << ")";
        //     if (i > 0)
        //         std::cout << " -> ";
        // }
        // std::cout << std::endl;
    } else {
        std::cout << "No solution found.\n";
    }
    
    return 0;
}
