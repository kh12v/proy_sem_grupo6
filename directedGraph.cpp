#include <iostream>
#include <vector>
#include <unordered_map>
#include <string>
#include <queue>
#include <limits>
#include <stack>

// Estructura de arista (dirigida y con peso)
struct Arista {
    std::string destino;
    double peso;
};

class GrafoDirigido {
private:
    // Lista de Adyacencia
    // Llave: Nodo de origen
    // Valor: Vector de aristas salientes
    std::unordered_map<std::string, std::vector<Arista>> listaAdyacencia;

public:
    // Método para agregar una arista
    void agregarArista(const std::string& origen, const std::string& destino, double peso) {
        listaAdyacencia[origen].push_back({destino, peso});

        if (listaAdyacencia.find(destino) == listaAdyacencia.end()) {
            listaAdyacencia[destino] = std::vector<Arista>();
        }
    }

    // Método para visualizar el contenido del grafo
    void imprimirGrafo() {
        for (const auto& par : listaAdyacencia) {
            std::cout << "Arista [" << par.first << "] tiene aristas salientes:\n";
            
            if (par.second.empty()) {
                std::cout << "  (No tiene aristas salientes)\n";
            } else {
                for (const auto& arista : par.second) {
                    std::cout << "  -> " << arista.destino 
                              << " (Peso: " << arista.peso << ")\n";
                }
            }
            std::cout << "----------------------------------\n";
        }
    }
    
    // Método extra para saber el información sobre el grafo
    void imprimirInformacion() {
        size_t totalAristas = 0;
        for (const auto& par : listaAdyacencia) {
            totalAristas += par.second.size();
        }
        std::cout << "\nInformacion del Grafo:\n";
        std::cout << "Total de Nodos: " << listaAdyacencia.size() << "\n";
        std::cout << "Total de Aristas (conexiones): " << totalAristas << "\n\n";
    }

    // Métricas:

    // Método para calcular el Degree Centrality de un vértice
    // normalizada: Si es true, divide el resultado entre (Total_Nodos - 1).
    // ponderada: Si es true, suma los pesos en lugar de contar las aristas.
    double calcularCentralidadGrado(const std::string& vertice, bool normalizada = true, bool ponderada = false) {
        // Validar que el vértice exista en el grafo
        if (listaAdyacencia.find(vertice) == listaAdyacencia.end()) {
            std::cerr << "Error: El vertice '" << vertice << "' no existe en el grafo.\n";
            return 0.0;
        }

        double outDegree = 0.0;
        double inDegree = 0.0;

        // Calcular el Grado de Salida
        for (const auto& arista : listaAdyacencia.at(vertice)) {
            outDegree += ponderada ? arista.peso : 1.0;
        }

        // Calcular el Grado de Entrada
        for (const auto& par : listaAdyacencia) {
            for (const auto& arista : par.second) {
                if (arista.destino == vertice) {
                    inDegree += ponderada ? arista.peso : 1.0;
                }
            }
        }

        double gradoTotal = inDegree + outDegree;

        // Normalización
        if (normalizada && !ponderada) {
            size_t numNodos = listaAdyacencia.size();
            if (numNodos > 1) {
                gradoTotal = gradoTotal / static_cast<double>(numNodos - 1);
            } else {
                gradoTotal = 0.0;
            }
        }

        return gradoTotal;
    }

    // Método para calcular el Betweenness Centrality de todos los nodos
    // normalizada: Si es true, divide el resultado entre (N-1)*(N-2)
    std::unordered_map<std::string, double> calcularBetweennessCentrality(bool normalizada = true) {
        std::unordered_map<std::string, double> betweenness;
        
        // Inicializar la centralidad de todos los nodos en 0
        for (const auto& par : listaAdyacencia) {
            betweenness[par.first] = 0.0;
        }

        // El algoritmo de Brandes itera tomando cada nodo como nodo de origen (s)
        for (const auto& nodoOrigen : listaAdyacencia) {
            std::string s = nodoOrigen.first;

            std::stack<std::string> pila;
            std::unordered_map<std::string, std::vector<std::string>> predecesores;
            std::unordered_map<std::string, double> sigma;
            std::unordered_map<std::string, double> distancias;

            for (const auto& par : listaAdyacencia) {
                sigma[par.first] = 0.0;
                distancias[par.first] = std::numeric_limits<double>::infinity();
            }
            
            sigma[s] = 1.0;
            distancias[s] = 0.0;

            // Cola de prioridad para Dijkstra: pares de (distancia, nodo)
            std::priority_queue<
                std::pair<double, std::string>, 
                std::vector<std::pair<double, std::string>>, 
                std::greater<std::pair<double, std::string>>
            > pq;

            pq.push({0.0, s});

            // 1. Encontrar los caminos más cortos (Dijkstra)
            while (!pq.empty()) {
                double distActual = pq.top().first;
                std::string v = pq.top().second;
                pq.pop();

                // Ignorar caminos obsoletos en la cola de prioridad
                if (distActual > distancias[v]) continue;

                pila.push(v);

                for (const auto& arista : listaAdyacencia.at(v)) {
                    std::string w = arista.destino;
                    double pesoArista = arista.peso;

                    // Se encontró un camino estrictamente más corto
                    if (distancias[w] > distancias[v] + pesoArista) {
                        distancias[w] = distancias[v] + pesoArista;
                        pq.push({distancias[w], w});
                        sigma[w] = sigma[v];
                        predecesores[w].clear();
                        predecesores[w].push_back(v);
                    } 
                    // Se encontró otro camino igual de corto
                    else if (distancias[w] == distancias[v] + pesoArista) {
                        sigma[w] += sigma[v];
                        predecesores[w].push_back(v);
                    }
                }
            }

            // 2. Acumulación (Fase de retroceso de Brandes)
            std::unordered_map<std::string, double> dependencia;
            for (const auto& par : listaAdyacencia) {
                dependencia[par.first] = 0.0;
            }

            while (!pila.empty()) {
                std::string w = pila.top();
                pila.pop();

                for (const std::string& v : predecesores[w]) {
                    // Cuidado con la división por cero si sigma[w] es 0
                    if (sigma[w] > 0) {
                        dependencia[v] += (sigma[v] / sigma[w]) * (1.0 + dependencia[w]);
                    }
                }

                if (w != s) {
                    betweenness[w] += dependencia[w];
                }
            }
        }

        // 3. Normalización
        if (normalizada) {
            double n = static_cast<double>(listaAdyacencia.size());
            // En un grafo dirigido, se divide por (N-1)*(N-2)
            double factorNormalizacion = (n - 1.0) * (n - 2.0);
            
            if (factorNormalizacion > 0) {
                for (auto& par : betweenness) {
                    par.second /= factorNormalizacion;
                }
            }
        }

        return betweenness;
    }
};
