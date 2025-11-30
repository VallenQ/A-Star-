
// g++ -std=c++17 A*.cpp -O2 -o a_star
// ./a_star mapa.txt

#include <bits/stdc++.h>
using namespace std;

struct Node {
    double f, g, h;
    int pi, pj;
    Node() : f(INFINITY), g(INFINITY), h(INFINITY), pi(-1), pj(-1) {}
};

struct CellPos { int i= -1, j= -1; };

bool is_separator_line_format(const string &line) {
    stringstream ss(line);
    vector<string> tokens;
    string tk;
    while (ss >> tk) tokens.push_back(tk);
    return tokens.size() > 1;
}

vector<vector<char>> parse_file_to_grid(const string &filename, CellPos &start, CellPos &goal) {
    ifstream fin(filename);
    if (!fin.is_open()) throw runtime_error("Não foi possível abrir o arquivo: " + filename);

    vector<string> raw_lines;
    string line;
    while (getline(fin, line)) {
        if (!line.empty() && line.back() == '\r') line.pop_back();
        raw_lines.push_back(line);
    }
    fin.close();

    if (raw_lines.empty()) throw runtime_error("Arquivo vazio.");

    int tokenized_votes = 0;
    for (size_t k = 0; k < raw_lines.size() && k < 10; ++k) {
        if (is_separator_line_format(raw_lines[k])) tokenized_votes++;
    }
    bool likely_tokenized = (tokenized_votes > 3);

    vector<vector<char>> grid;
    size_t max_cols = 0;

    if (likely_tokenized) {
        for (auto &raw : raw_lines) {
            stringstream ss(raw);
            vector<char> row;
            string token;
            while (ss >> token) {
                if (token.empty()) continue;
                row.push_back(token[0]);
            }
            max_cols = max(max_cols, row.size());
            grid.push_back(row);
        }
    } else {
        for (auto &raw : raw_lines) {
            vector<char> row;
            for (char c : raw) row.push_back(c);
            max_cols = max(max_cols, row.size());
            grid.push_back(row);
        }
    }

    for (auto &r : grid) {
        if (r.size() < max_cols) r.resize(max_cols, 'x');
    }

    bool foundStart = false, foundGoal = false;
    for (int i = 0; i < (int)grid.size(); ++i) {
        for (int j = 0; j < (int)grid[i].size(); ++j) {
            char c = grid[i][j];
            if (c == '*') {
                if (foundStart) throw runtime_error("Mais de um '*' encontrado (apenas 1 permitido).");
                start.i = i; start.j = j; foundStart = true;
            } else if (c == '6') {
                if (foundGoal) throw runtime_error("Mais de um '6' encontrado (apenas 1 permitido).");
                goal.i = i; goal.j = j; foundGoal = true;
            }
        }
    }
    if (!foundStart) throw runtime_error("Ponto de origem '*' não encontrado no mapa.");
    if (!foundGoal) throw runtime_error("Ponto de destino '6' não encontrado no mapa.");

    return grid;
}

bool in_bounds(int i, int j, int R, int C) {
    return i >= 0 && i < R && j >= 0 && j < C;
}

bool is_free_cell(char c) {
    return (c == '.' || c == '*' || c == '6');
}

double heuristic(int i, int j, int gi, int gj) {
    double di = double(i - gi);
    double dj = double(j - gj);
    return sqrt(di*di + dj*dj);
}

void print_grid(const vector<vector<char>>& grid) {
    for (const auto &row : grid) {
        for (char c : row) cout << c;
        cout << '\n';
    }
}

int main(int argc, char** argv) {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    try {
        if (argc < 2) {
            cerr << "Uso: " << argv[0] << " arquivo.txt\n";
            return 1;
        }
        string filename = argv[1];
        CellPos start, goal;
        vector<vector<char>> grid = parse_file_to_grid(filename, start, goal);

        int R = (int)grid.size();
        int C = (R>0? (int)grid[0].size() : 0);

        vector<vector<Node>> nodes(R, vector<Node>(C));
        vector<vector<bool>> closed(R, vector<bool>(C,false));

        using PQItem = tuple<double,int,int>;
        auto cmp = [](const PQItem &a, const PQItem &b) {
            if (get<0>(a) != get<0>(b)) return get<0>(a) > get<0>(b);
            if (get<1>(a) != get<1>(b)) return get<1>(a) > get<1>(b);
            return get<2>(a) > get<2>(b);
        };
        priority_queue<PQItem, vector<PQItem>, decltype(cmp)> open(cmp);

        nodes[start.i][start.j].g = 0.0;
        nodes[start.i][start.j].h = heuristic(start.i, start.j, goal.i, goal.j);
        nodes[start.i][start.j].f = nodes[start.i][start.j].h;
        nodes[start.i][start.j].pi = start.i;
        nodes[start.i][start.j].pj = start.j;
        open.emplace(nodes[start.i][start.j].f, start.i, start.j);

        const int di[8] = {-1, 1, 0, 0, -1, -1, 1, 1};
        const int dj[8] = {0, 0, 1, -1, 1, -1, 1, -1};
        const double cost[8] = {1.0, 1.0, 1.0, 1.0, 1.41421356237, 1.41421356237, 1.41421356237, 1.41421356237};

        bool found = false;
        while (!open.empty()) {
            auto [curF, ci, cj] = open.top(); open.pop();
            if (closed[ci][cj]) continue;
            closed[ci][cj] = true;

            if (ci == goal.i && cj == goal.j) { found = true; break; }

            for (int k = 0; k < 8; ++k) {
                int ni = ci + di[k];
                int nj = cj + dj[k];
                if (!in_bounds(ni, nj, R, C)) continue;
                if (!is_free_cell(grid[ni][nj])) continue;


                double tentative_g = nodes[ci][cj].g + cost[k];
                if (tentative_g < nodes[ni][nj].g) {
                    nodes[ni][nj].g = tentative_g;
                    nodes[ni][nj].h = heuristic(ni,nj,goal.i,goal.j);
                    nodes[ni][nj].f = tentative_g + nodes[ni][nj].h;
                    nodes[ni][nj].pi = ci;
                    nodes[ni][nj].pj = cj;
                    open.emplace(nodes[ni][nj].f, ni, nj);
                }
            }
        }

        if (!found) {
            cerr << "Falha ao encontrar um caminho do '*' para '6'.\n";
            return 2;
        }

        int ri = goal.i, rj = goal.j;
        vector<pair<int,int>> path;
        while (!(nodes[ri][rj].pi == ri && nodes[ri][rj].pj == rj)) {
            path.emplace_back(ri,rj);
            int pi = nodes[ri][rj].pi;
            int pj = nodes[ri][rj].pj;
            if (pi == -1 || pj == -1) break;
            ri = pi; rj = pj;
        }
        path.emplace_back(start.i, start.j);

        for (auto &p : path) {
            int i = p.first, j = p.second;
            if (i == start.i && j == start.j) continue;
            if (i == goal.i && j == goal.j) continue;
            if (grid[i][j] == '.') grid[i][j] = '-';
        }

        print_grid(grid);
        return 0;
    }
    catch (const exception &ex) {
        cerr << "Erro: " << ex.what() << "\n";
        return 1;
    }
}
