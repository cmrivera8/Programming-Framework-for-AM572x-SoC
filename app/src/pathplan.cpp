#include <iostream>
#include <vector>
#include <queue>
#include <utility>
#include <cmath>
#include <unistd.h>

const int ROWS = 30;
const int COLS = 30;

const int dx[4] = {-1, 0, 1, 0};
const int dy[4] = {0, 1, 0, -1};

int grid[ROWS][COLS];
int h[ROWS][COLS];
int g[ROWS][COLS];
int f[ROWS][COLS];
std::vector<std::pair<int, int>> path;

struct Node {
  int x, y;
  int f, g, h;

  bool operator<(const Node &n2) const {
    return f > n2.f;
  }
};

void initGrid(int startX, int startY, int endX, int endY) {
  for (int i = 0; i < ROWS; i++) {
    for (int j = 0; j < COLS; j++) {
      grid[i][j] = 0;
      h[i][j] = abs(i - endX) + abs(j - endY);
    }
  }
  grid[startX][startY] = 2;
  grid[endX][endY] = 3;
}

void printGrid() {
  // usleep(1000*100);
  system("clear");
  std::cout << "  ";
  for (int i = 0; i < COLS; i++) {
    std::cout << i << " ";
  }
  std::cout << std::endl;
  for (int i = 0; i < ROWS; i++) {
    std::cout << i << " \t";
    for (int j = 0; j < COLS; j++) {
      if (grid[i][j] == 0) {
        std::cout << ". ";
      } else if (grid[i][j] == 2) {
        std::cout << "S ";
      } else if (grid[i][j] == 3) {
        std::cout << "T ";
      } else {
        std::cout << "X ";
      }
    }
    std::cout << std::endl;
  }
  std::cout << std::endl;
}

void printPath() {
  std::cout << "Path: " << std::endl;
  for (int i = path.size() - 1; i >= 0; i--) {
    std::cout << "(" << path[i].first << ", " << path[i].second << ")" << std::endl;
  }
}

bool isValid(int x, int y) {
  if (x < 0 || x >= ROWS || y < 0 || y >= COLS) {
    return false;
  }
  return true;
}

bool isDestination(int x, int y, int endX, int endY) {
  return x == endX && y == endY;
}

void aStarSearch(int startX, int startY, int endX, int endY) {
  std::priority_queue<Node> openList;
  Node startNode = {startX, startY, 0, 0, h[startX][startY]};
  openList.push(startNode);

  while (!openList.empty()) {
    Node current = openList.top();
    int x = current.x;
    int y = current.y;

    if (isDestination(x, y, endX, endY)) {
      path.emplace_back(x, y);
      break;
    }

    openList.pop();
    grid[x][y] = 1;

    for (int i = 0; i < 4; i++) {
      int xNext = x + dx[i];
      int yNext = y + dy[i];
      if (isValid(xNext, yNext) && grid[xNext][yNext] != 1) {
        int gNext = g[x][y] + 1;
        int hNext = h[xNext][yNext];
        int fNext = gNext + hNext;
        if (grid[xNext][yNext] == 0 || gNext < g[xNext][yNext]) {
          g[xNext][yNext] = gNext;
          openList.push({xNext, yNext, fNext, gNext, hNext});
          grid[xNext][yNext] = 4;
        }
      }
    }
    printGrid();
  }
}


int main() {
  int startX = 0, startY = 0;
  int endX = 25, endY = 25;

  initGrid(startX, startY, endX, endY);
  aStarSearch(startX, startY, endX, endY);

  for (auto point : path) {
    grid[point.first][point.second] = 40;
  }

  std::cout << "Path: " << std::endl;
  printPath();

  return 0;
}