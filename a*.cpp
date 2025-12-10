#include "a*.h"
#include <bits/stdc++.h>
using namespace std;

struct Node {
    double f, g, h;
    int pi, pj;
    Node() : f(INFINITY), g(INFINITY), h(INFINITY), pi(-1), pj(-1) {}
};

bool is_free_cell(char c) {
    return (c == ' ' || c == '*' || c == '6');
}

bool in_bounds(int i, int j, int R, int C) {
    return i >= 0 && i < R && j >= 0 && j < C;
}

double heuristic(int i, int j, int gi, int gj) {
    double di = i - gi;
    double dj = j - gj;
    return sqrt(di*di + dj*dj);
}

vector<vector<char>> parse_file_to_grid(const string &filename, CellPos &start, CellPos &goal) {
   
    ifstream fin(filename);
    if (!fin.is_open()) throw runtime_error("Erro ao abrir arquivo.");

    vector<vector<char>> grid;
    string line;

    int row = 0;
    bool foundStart = false, foundGoal = false;

    while (getline(fin, line)) {
        vector<char> rowVec(line.begin(), line.end());
        for (int j = 0; j < (int)rowVec.size(); j++) {
            if (rowVec[j] == '*') {
                start = {row, j};
                foundStart = true;
            } else if (rowVec[j] == '6') {
                goal = {row, j};
                foundGoal = true;
            }
        }
        grid.push_back(rowVec);
        row++;
    }

    if (!foundStart) throw runtime_error("Mapa não contém '*'");
    if (!foundGoal) throw runtime_error("Mapa não contém '6'");

    return grid;
}

vector<vector<char>> a_star_algorithm(const vector<vector<char>> &grid, const CellPos &start, const CellPos &goal) {
    int R = grid.size();
    int C = grid[0].size();

    vector<vector<Node>> nodes(R, vector<Node>(C));
    vector<vector<bool>> closed(R, vector<bool>(C,false));

    using PQItem = tuple<double,int,int>;
    auto cmp = [](const PQItem &a, const PQItem &b){
        return get<0>(a) > get<0>(b);
    };
    priority_queue<PQItem, vector<PQItem>, decltype(cmp)> open(cmp);

    nodes[start.i][start.j].g = 0.0;
    nodes[start.i][start.j].h = heuristic(start.i,start.j, goal.i,goal.j);
    nodes[start.i][start.j].f = nodes[start.i][start.j].h;
    nodes[start.i][start.j].pi = start.i;
    nodes[start.i][start.j].pj = start.j;

    open.emplace(nodes[start.i][start.j].f, start.i, start.j);

    const int di[8] = {-1,1,0,0,-1,-1,1,1};
    const int dj[8] = {0,0,1,-1,1,-1,1,-1};
    const double cost[8] = {
        1,1,1,1,
        1.4142,1.4142,1.4142,1.4142
    };

    bool found = false;

    while (!open.empty()) {
        auto [curF, ci, cj] = open.top();
        open.pop();

        if (closed[ci][cj]) continue;
        closed[ci][cj] = true;

        if (ci == goal.i && cj == goal.j) {
            found = true;
            break;
        }

        for (int k = 0; k < 8; k++) {
            int ni = ci + di[k];
            int nj = cj + dj[k];

            if (!in_bounds(ni,nj,R,C)) continue;
            if (!is_free_cell(grid[ni][nj])) continue;

            double tg = nodes[ci][cj].g + cost[k];
            if (tg < nodes[ni][nj].g) {
                nodes[ni][nj].g = tg;
                nodes[ni][nj].h = heuristic(ni,nj,goal.i,goal.j);
                nodes[ni][nj].f = tg + nodes[ni][nj].h;

                nodes[ni][nj].pi = ci;
                nodes[ni][nj].pj = cj;

                open.emplace(nodes[ni][nj].f, ni, nj);
            }
        }
    }

    if (!found) throw runtime_error("Nenhum caminho encontrado.");

    vector<vector<char>> newGrid = grid;

    int ri = goal.i, rj = goal.j;
    while (!(nodes[ri][rj].pi == ri && nodes[ri][rj].pj == rj)) {
        if (newGrid[ri][rj] == ' ') newGrid[ri][rj] = '-';
        int pi = nodes[ri][rj].pi;
        int pj = nodes[ri][rj].pj;
        ri = pi; rj = pj;
    }

    return newGrid;
}
