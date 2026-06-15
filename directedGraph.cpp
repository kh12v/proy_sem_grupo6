#include <iostream>
#include <vector>
#include <unordered_map>
#include <string>

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
};
