#include <iostream>
#include "a*.h"

using namespace std;

int main(int argc, char** argv) {
    try {
        
        string file = argv[1];

        CellPos start, goal;
        std::vector<std::vector<char>> map = parse_file_to_grid(file, start, goal);

        std::vector<std::vector<char>> mapa_retornado = a_star_algorithm(map, start, goal);

        for (std::vector<char> &linha : mapa_retornado) {
            for (char &c : linha) cout << c;
            cout << "\n";
        }
    }
    catch (exception &e) {
        cerr << "Erro: " << e.what() << endl;
    }
}
