#include <iostream>
#include <deque>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <cmath>
#include <queue>


struct Point 
{
    int x, y;
    bool passable;
};

// Structure pour représenter un nœud dans l'algorithme A*
struct Node 
{
    //differents couts et ptr 
    Point point;
    int g_cost;  
    int h_cost;  
    Node* parent; 
};

// var globales
std::deque<std::deque<Point>> tab(10, std::deque<Point>(10));
std::unordered_map<int, std::unordered_map<int, Node*>> open_list;
std::unordered_map<int, std::unordered_map<int, Node*>> closed_list;
std::unordered_map<int, std::unordered_map<int, int>> usage_count;

Point start{ 0, 0, true }; // Départ
Point goal{ 9, 9, true }; // Objectif
std::deque<Point> path; // Chemin trouvé

// fct pour calculer le coût G
int calculate_g_cost(Node* parent, const Point& neighbor) 
{
    return parent->g_cost + 1 + usage_count[neighbor.y][neighbor.x]; // Pénalité pour l'usage
}

// fct pour calculer le coût H
int calculate_h_cost(const Point& a, const Point& b) 
{
    return std::abs(a.x - b.x) + std::abs(a.y - b.y); // Distance de Manhattan
}

// fct pour ajouter des nœuds adjacents
void add_adjacent_nodes(Node* current) 
{
    int directions[4][2] = { {-1, 0}, {1, 0}, {0, -1}, {0, 1} }; // Haut, Bas, Gauche, Droite

    for (auto& direction : directions) 
    {
        int new_x = current->point.x + direction[0];
        int new_y = current->point.y + direction[1];

        // Vérifie les limites et la passabilité
        if (new_x >= 0 && new_y >= 0 && new_x < 10 && new_y < 10) 
        {
            Point& neighbor = tab[new_y][new_x]; 
            if (neighbor.passable) 
            {
                // Si le nœud est déjà dans la liste fermée, l'ignore
                if (closed_list.count(new_y) && closed_list[new_y].count(new_x)) 
                {
                    continue;
                }

                //les coûts
                int g_cost = calculate_g_cost(current, neighbor);
                int h_cost = calculate_h_cost(neighbor, goal);

                // verifie si le nœud est déjà dans la liste ouverte
                if (!open_list.count(new_y) || !open_list[new_y].count(new_x)) 
                {
                    Node* neighbor_node = new Node{ neighbor, g_cost, h_cost, current }; // Crée le nœud
                    open_list[new_y][new_x] = neighbor_node; // Ajoute le nœud à la liste ouverte
                }
                else 
                {
                    Node* existing_node = open_list[new_y][new_x];
                    if (g_cost < existing_node->g_cost) 
                    {
                        existing_node->g_cost = g_cost;
                        existing_node->parent = current; // Met à jour le parent
                    }
                }

                // Incrémente le compteur d'usage pour le nœud
                usage_count[new_y][new_x]++;
            }
        }
    }
}

// Fonction principale de l'algorithme A*
void a_star() 
{
    Node* start_node = new Node{ start, 0, calculate_h_cost(start, goal), nullptr };
    open_list[start.y][start.x] = start_node;

    while (!open_list.empty()) 
    {
        // Trouver le nœud avec le coût total le plus bas
        auto current_iter = std::min_element(open_list.begin(), open_list.end(),
            [](const auto& a, const auto& b) 
            {
                return a.second.begin()->second->g_cost + a.second.begin()->second->h_cost <
                    b.second.begin()->second->g_cost + b.second.begin()->second->h_cost;
            });

        Node* current = current_iter->second.begin()->second;

        // Vérifie si on a atteint l'objectif
        if (current->point.x == goal.x && current->point.y == goal.y) 
        {
            // Récupérer le chemin
            while (current)
            {
                path.push_front(current->point);
                current = current->parent;
            }
            return; // Terminer si le chemin est trouvé
        }

        // Déplace le nœud courant vers la liste fermée
        closed_list[current->point.y][current->point.x] = current;
        open_list[current->point.y].erase(current->point.x);
        if (open_list[current->point.y].empty()) {
            open_list.erase(current->point.y);
        }

        // Ajoute les nœuds adjacents
        add_adjacent_nodes(current);
    }
}

int main() {
    // init des coordonnées de chaque point dans le tableau 10x10
    for (size_t i = 0; i < tab.size(); i++) 
    {
        for (size_t j = 0; j < tab[i].size(); j++) 
        {
            tab[i][j] = { static_cast<int>(j), static_cast<int>(i), true }; // Par défaut, tous les points sont passables
        }
    }

    //Des points non passables
    tab[1][1].passable = false;
    tab[1][2].passable = false;
    tab[1][3].passable = false;
    tab[2][3].passable = false;
    tab[3][3].passable = false;
    tab[4][4].passable = false;

    // Affichage du tableau 10x10
    std::cout << "\nAffichage du tableau 10x10 :\n";
    for (size_t i = 0; i < tab.size(); i++) 
    {
        for (size_t j = 0; j < tab[i].size(); j++) 
        {
            std::cout << (tab[i][j].passable ? "P" : "X") << " "; // P pour passable, X pour non passable
        }
        std::cout << std::endl;  // Retour à la ligne après chaque 10 colonnes
    }

    // Lancer l'algorithme A*
    a_star();

    // Afficher le chemin trouvé
    std::cout << "\nChemin trouve :\n";
    if (path.empty()) 
    {
        std::cout << "Aucun chemin trouve.\n";
    }
    else 
    {
        for (const auto& p : path) 
        {
            std::cout << "(" << p.x << ", " << p.y << ") ";
        }
    }

    // Libération de la mémoire
    for (const auto& row : open_list) 
    {
        for (const auto& entry : row.second) 
        {
            delete entry.second; // Libère chaque nœud de la liste ouverte
        }
    }

    return 0;
}
