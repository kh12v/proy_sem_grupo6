#include <iostream>
#include <vector>
#include <unordered_map>
#include <string>

// 1. Definimos la estructura de la arista (conexión)
struct Arista {
    std::string destino;
    double peso;
};

// 2. Definimos la clase del Grafo Dirigido
class GrafoDirigido {
private:
    // La Lista de Adyacencia
    // Llave: Nodo de origen (ej. "ALB")
    // Valor: Una lista (vector) de todas sus conexiones salientes
    std::unordered_map<std::string, std::vector<Arista>> listaAdyacencia;

public:
    // Método para agregar una arista
    void agregarArista(const std::string& origen, const std::string& destino, double peso) {
        
        // Al ser DIRIGIDO, la conexión solo fluye de origen a destino.
        // Agregamos el destino a la lista del origen.
        listaAdyacencia[origen].push_back({destino, peso});

        // Este paso extra es una buena práctica: 
        // Nos aseguramos de que el nodo destino exista en el grafo principal, 
        // incluso si no tiene aristas salientes propias.
        if (listaAdyacencia.find(destino) == listaAdyacencia.end()) {
            listaAdyacencia[destino] = std::vector<Arista>();
        }
    }

    // Método para visualizar el contenido del grafo
    void imprimirGrafo() {
        for (const auto& par : listaAdyacencia) {
            std::cout << "El pais [" << par.first << "] exporta a:\n";
            
            if (par.second.empty()) {
                std::cout << "  (Nadie)\n";
            } else {
                for (const auto& arista : par.second) {
                    std::cout << "  -> " << arista.destino 
                              << " (Volumen: " << arista.peso << ")\n";
                }
            }
            std::cout << "----------------------------------\n";
        }
    }
    
    // Método extra para saber el tamaño real en memoria
    void imprimirEstadisticas() {
        size_t totalAristas = 0;
        for (const auto& par : listaAdyacencia) {
            totalAristas += par.second.size();
        }
        std::cout << "\nEstadisticas del Grafo:\n";
        std::cout << "Total de Nodos: " << listaAdyacencia.size() << "\n";
        std::cout << "Total de Aristas (conexiones): " << totalAristas << "\n\n";
    }
};
