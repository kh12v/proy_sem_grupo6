#include <iostream>
#include <vector>
#include <unordered_map>
#include <string>
#include <queue>
#include <limits>
#include <stack>
#include <cmath>
#include <random>


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
    // Complejidad: O(1)
    void agregarArista(const std::string& origen, const std::string& destino, double peso) {
        listaAdyacencia[origen].push_back({destino, peso});

        if (listaAdyacencia.find(destino) == listaAdyacencia.end()) {
            listaAdyacencia[destino] = std::vector<Arista>();
        }
    }

    // Método para eliminar una arista
    // Complejidad: O(E) donde E es el grado de salida del nodo
    void eliminarArista(const std::string& origen, const std::string& destino) {
        if (listaAdyacencia.find(origen) != listaAdyacencia.end()) {
            auto& aristas = listaAdyacencia[origen];
            for (auto it = aristas.begin(); it != aristas.end(); ++it) {
                if (it->destino == destino) {
                    aristas.erase(it);
                    break;
                }
            }
        }
    }

    // Devuelve una estimación exacta del uso total de memoria en bytes (RAM)
    size_t obtenerUsoMemoria() const {
        size_t totalBytes = sizeof(*this);
        totalBytes += listaAdyacencia.bucket_count() * sizeof(void*);

        for (const auto& par : listaAdyacencia) {
            totalBytes += sizeof(par.first) + par.first.capacity();
            totalBytes += sizeof(par.second) + (par.second.capacity() * sizeof(Arista));
            for (const auto& arista : par.second) {
                totalBytes += arista.destino.capacity();
            }
            totalBytes += sizeof(void*) * 3; // Overhead por nodo del hash map
        }
        return totalBytes;
    }

    // Devuelve una lista con los nombres/IDs de todos los nodos presentes en el grafo
    std::vector<std::string> obtenerNombresNodos() const {
    std::vector<std::string> nodos;
    nodos.reserve(listaAdyacencia.size());
    for (const auto& par : listaAdyacencia) {
        nodos.push_back(par.first);
    }
    return nodos;
}


    // Calcula las distancias mínimas desde un nodo usando Dijkstra.
    // Devuelve solo distancias finitas hacia nodos alcanzables.
    // Complejidad: O(V + E log(V))
    std::unordered_map<std::string, double> calcularDistanciasMinimasDesde(const std::string& origen) const {
        std::unordered_map<std::string, double> distancias;

        if (listaAdyacencia.find(origen) == listaAdyacencia.end()) {
            return distancias;
        }

        for (const auto& par : listaAdyacencia) {
            distancias[par.first] = std::numeric_limits<double>::infinity();
        }

        using NodoDistancia = std::pair<double, std::string>;
        std::priority_queue<NodoDistancia, std::vector<NodoDistancia>, std::greater<NodoDistancia>> pq;

        distancias[origen] = 0.0;
        pq.push({0.0, origen});

        while (!pq.empty()) {
            double distanciaActual = pq.top().first;
            std::string nodoActual = pq.top().second;
            pq.pop();

            if (distanciaActual > distancias[nodoActual]) {
                continue;
            }

            for (const auto& arista : listaAdyacencia.at(nodoActual)) {
                double nuevaDistancia = distancias[nodoActual] + arista.peso;
                if (nuevaDistancia < distancias[arista.destino]) {
                    distancias[arista.destino] = nuevaDistancia;
                    pq.push({nuevaDistancia, arista.destino});
                }
            }
        }

        return distancias;
    }

    // Método para visualizar el contenido del grafo
    // Complejidad: O(V + E)
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
    // Complejidad: O(V)
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
    // Complejidad: O(V + E)
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
    // Complejidad: O(V^2 + V*E*log(V))
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

    // Método para calcular el PageRank ponderado de todos los vértices
    // d: Damping factor (Factor de amortiguación), default: 0.85
    // maxIteraciones: Límite de seguridad por si el grafo no converge
    // tolerancia: Margen de error para decidir que los valores ya se estabilizaron
    // Complejidad: O(I * (V + E)), donde I es el número de iteraciones (maxIteraciones)
    std::unordered_map<std::string, double> calcularPageRank(double d = 0.85, int maxIteraciones = 100, double tolerancia = 1e-6) {
        size_t N = listaAdyacencia.size();
        std::unordered_map<std::string, double> pr;
        
        if (N == 0) return pr;

        std::unordered_map<std::string, double> outWeight;

        // Inicialización y pre-cálculo
        // Todos los nodos inician con el mismo PageRank: 1/N
        double valorInicial = 1.0 / static_cast<double>(N);
        for (const auto& par : listaAdyacencia) {
            const std::string& nodo = par.first;
            pr[nodo] = valorInicial;
            
            // Calculamos el peso total de salida de cada nodo
            double sumaPesos = 0.0;
            for (const auto& arista : par.second) {
                sumaPesos += arista.peso;
            }
            outWeight[nodo] = sumaPesos;
        }

        // Proceso Iterativo
        for (int iter = 0; iter < maxIteraciones; ++iter) {
            std::unordered_map<std::string, double> nuevoPr;
            double danglingSum = 0.0;

            // Base del PageRank (el (1-d)% de salto aleatorio)
            double baseAleatoria = (1.0 - d) / static_cast<double>(N);
            for (const auto& par : listaAdyacencia) {
                nuevoPr[par.first] = baseAleatoria;
            }

            // Transferencia de influencia
            for (const auto& par : listaAdyacencia) {
                const std::string& u = par.first;
                
                if (outWeight[u] > 0) {
                    // Si el nodo tiene salidas, reparte su PR proporcionalmente al peso de cada arista
                    for (const auto& arista : par.second) {
                        const std::string& v = arista.destino;
                        double proporcion = arista.peso / outWeight[u];
                        nuevoPr[v] += d * pr[u] * proporcion;
                    }
                } else {
                    // Nodo sumidero (sin salidas): se acumula su PR para repartirlo a todos
                    danglingSum += pr[u];
                }
            }

            // Repartir el PR de los nodos sumidero (Dangling nodes) equitativamente
            if (danglingSum > 0) {
                double porcionDangling = (d * danglingSum) / static_cast<double>(N);
                for (auto& par : nuevoPr) {
                    par.second += porcionDangling;
                }
            }

            // Comprobar Convergencia
            double error = 0.0;
            for (const auto& par : listaAdyacencia) {
                error += std::abs(nuevoPr[par.first] - pr[par.first]);
            }

            // Actualizamos los valores para la siguiente iteración
            pr = nuevoPr;

            // Si el cambio total es menor que la tolerancia, terminamos prematuramente
            if (error < tolerancia) {
                break;
            }
        }

        return pr;
    }

    // Average Shortest Path Length (ASP): promedio de todas las distancias mínimas finitas.
    // En grafos desconectados se promedian solo los pares alcanzables.
    // Complejidad: O(V^2 + V*E*log(V))
    double calcularAverageShortestPathLength() const {
        if (listaAdyacencia.empty()) {
            return 0.0;
        }

        double sumaDistancias = 0.0;
        long long cantidadPares = 0;

        for (const auto& par : listaAdyacencia) {
            const std::string& origen = par.first;
            auto distancias = calcularDistanciasMinimasDesde(origen);

            for (const auto& distancia : distancias) {
                if (distancia.first != origen && std::isfinite(distancia.second)) {
                    sumaDistancias += distancia.second;
                    cantidadPares++;
                }
            }
        }

        if (cantidadPares == 0) {
            return 0.0;
        }

        return sumaDistancias / static_cast<double>(cantidadPares);
    }

    // Diámetro del grafo: mayor distancia mínima encontrada entre pares alcanzables.
    // Si el grafo está desconectado, devuelve el máximo finito observado.
    // Complejidad: O(V^2 + V*E*log(V))
    double calcularDiametro() const {
        if (listaAdyacencia.empty()) {
            return 0.0;
        }

        double diametro = 0.0;

        for (const auto& par : listaAdyacencia) {
            const std::string& origen = par.first;
            auto distancias = calcularDistanciasMinimasDesde(origen);

            for (const auto& distancia : distancias) {
                if (distancia.first != origen && std::isfinite(distancia.second)) {
                    if (distancia.second > diametro) {
                        diametro = distancia.second;
                    }
                }
            }
        }

        return diametro;
    }

    // Método para calcular el Closeness Centrality de un vértice específico
    // Formulación Matemática del informe: C_c(u) = (n - 1) / sum(d(u, v))
    // Si el grafo es disconexo desde el nodo (existe algún d(u,v) = inf), C_c(u) = 0.
    // Complejidad: O(V + E log V)
    double calcularClosenessCentrality(const std::string& vertice) const {
        if (listaAdyacencia.find(vertice) == listaAdyacencia.end()) {
            std::cerr << "Error: El vertice '" << vertice << "' no existe en el grafo.\n";
            return 0.0;
        }

        double n = static_cast<double>(listaAdyacencia.size());
        if (n <= 1) return 0.0;

        auto distancias = calcularDistanciasMinimasDesde(vertice);

        double sumaDistancias = 0.0;
        bool esDisconexo = false;
        long long contNodos = 0;

        for (const auto& distPar : distancias) {
            if (distPar.first == vertice) continue;

            if (std::isinf(distPar.second)) {
                esDisconexo = true;
                break;
            }
            sumaDistancias += distPar.second;
            contNodos++;
        }

        if (!esDisconexo && contNodos == static_cast<long long>(n - 1) && sumaDistancias > 0) {
            return (n - 1.0) / sumaDistancias;
        } else {
            return 0.0;
        }
    }

    // Método para calcular la Excentricidad de un vértice específico
    // Formulación Matemática del informe: e(u) = max_{v in V} d(u, v)
    // Si el grafo es disconexo desde el nodo, su excentricidad se define como infinito (std::numeric_limits<double>::infinity())
    // Complejidad: O(V + E log V)
    double calcularExcentricidad(const std::string& vertice) const {
        if (listaAdyacencia.find(vertice) == listaAdyacencia.end()) {
            std::cerr << "Error: El vertice '" << vertice << "' no existe en el grafo.\n";
            return 0.0;
        }

        auto distancias = calcularDistanciasMinimasDesde(vertice);
        double maxDistancia = 0.0;

        for (const auto& distPar : distancias) {
            if (std::isinf(distPar.second)) {
                return std::numeric_limits<double>::infinity();
            }
            if (distPar.second > maxDistancia) {
                maxDistancia = distPar.second;
            }
        }

        return maxDistancia;
    }
};

