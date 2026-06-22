#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <string>
#include <iomanip>
#include "directedGraph.cpp"

void mostrarProgreso(double porcentaje) {
    int anchoBarra = 40;
    int progresoActual = static_cast<int>((porcentaje / 100.0) * anchoBarra);

    std::cout << "\rCargando dataset: [";
    for (int i = 0; i < anchoBarra; ++i) {
        if (i < progresoActual) std::cout << "=";
        else if (i == progresoActual) std::cout << ">";
        else std::cout << " ";
    }
    std::cout << "] " << std::fixed << std::setprecision(1) << porcentaje << "%" << std::flush;
}

// Está diseñado específicamente para leer grafos no dirigidos, es decir (A->B y B->A)
void cargarDesdeCsvNoDirigido(GrafoDirigido& grafoDirigido, const std::string& nombreArchivo) {
    // Abrir el archivo
    std::ifstream archivo(nombreArchivo, std::ios::binary | std::ios::ate);
    if (!archivo.is_open()) {
        std::cerr << "Error: No se pudo abrir el archivo " << nombreArchivo << "\n";
        return;
    }

    std::streamsize tamanoTotalBytes = archivo.tellg();
    archivo.seekg(0, std::ios::beg);

    std::string linea;
    
    // Descartar la línea del encabezado
    if (!std::getline(archivo, linea)) {
        std::cerr << "Error: El archivo esta vacio.\n";
        return;
    }

    std::cout << "Iniciando la lectura del dataset...\n";

    // Variables para el parseo
    std::string origen, destino, pesoStr;
    double peso;
    
    long long lineasProcesadas = 0;

    // Bucle de lectura línea por línea
    while (std::getline(archivo, linea)) {
        if (linea.empty()) continue;

        std::stringstream ss(linea);
        
        // Separar por comas
        if (std::getline(ss, origen, ',') &&
            std::getline(ss, destino, ',') &&
            std::getline(ss, pesoStr, ',')) {
            
            peso = std::stod(pesoStr);

            // Insertar de manera NO dirigida (A->B y B->A)
            grafoDirigido.agregarArista(origen, destino, peso);
            grafoDirigido.agregarArista(destino, origen, peso);
        }

        lineasProcesadas++;

        // Actualizar la pantalla cada 1000 líneas para optimizar la velocidad de procesamiento
        if (lineasProcesadas % 1000 == 0) {
            std::streamsize bytesLeidos = archivo.tellg();
            double porcentaje = (static_cast<double>(bytesLeidos) / tamanoTotalBytes) * 100.0;
            mostrarProgreso(porcentaje);
        }
    }

    mostrarProgreso(100.0);
    std::cout << "\n\nCarga completada con exito\n";
}

bool cargarDesdePajek(GrafoDirigido& grafoDirigido, const std::string& nombreArchivo) {
    std::ifstream archivo(nombreArchivo, std::ios::binary | std::ios::ate);
    if (!archivo.is_open()) {
        std::cerr << "Error: No se pudo abrir " << nombreArchivo << "\n";
        return false;
    }

    std::streamsize tamanoTotal = archivo.tellg();
    archivo.seekg(0, std::ios::beg);

    std::string linea;
    
    // Diccionario para mapear el ID numérico del archivo al ISO
    std::unordered_map<int, std::string> mapaIdsNodos;
    
    // Banderas de estado
    bool leyendoVertices = false;
    bool leyendoArcos = false;
    long long lineasProcesadas = 0;

    while (std::getline(archivo, linea)) {
        if (!linea.empty() && linea.back() == '\r') {
            linea.pop_back();
        }
        if (linea.empty()) continue;

        // Detectar en qué sección del archivo estamos
        if (linea.find("*Vertices") == 0) {
            leyendoVertices = true;
            leyendoArcos = false;
            continue; 
        } else if (linea.find("*Arcs") == 0 || linea.find("*Edges") == 0) {
            leyendoVertices = false;
            leyendoArcos = true;
            continue;
        }

        // Procesar la línea dependiendo del bloque activo
        if (leyendoVertices) {
            // Formato esperado: 1 "AGO" 0.5496 0.5622 0.5000
            int id;
            std::istringstream iss(linea);
            iss >> id; // Captura el número inicial

            // Buscar el texto entre las primeras comillas
            size_t inicioComillas = linea.find('"');
            size_t finComillas = linea.find('"', inicioComillas + 1);
            
            if (inicioComillas != std::string::npos && finComillas != std::string::npos) {
                std::string nombreNodo = linea.substr(inicioComillas + 1, finComillas - inicioComillas - 1);
                mapaIdsNodos[id] = nombreNodo;
                // Todo lo que sigue en la línea (coordenadas) se ignora
            }
        } 
        else if (leyendoArcos) {
            // Formato esperado: 1 2 2.709
            int idOrigen, idDestino;
            double peso;
            std::istringstream iss(linea);
            
            if (iss >> idOrigen >> idDestino >> peso) {
                // Validamos que los IDs existan en el mapa
                if (mapaIdsNodos.count(idOrigen) && mapaIdsNodos.count(idDestino)) {
                    std::string nombreOrigen = mapaIdsNodos[idOrigen];
                    std::string nombreDestino = mapaIdsNodos[idDestino];
                    
                    // Insertamos usando los nombres
                    grafoDirigido.agregarArista(nombreOrigen, nombreDestino, peso);
                }
            }
        }

        lineasProcesadas++;
        if (lineasProcesadas % 500 == 0) {
            double porcentaje = (static_cast<double>(archivo.tellg()) / tamanoTotal) * 100.0;
            mostrarProgreso(porcentaje);
        }
    }

    mostrarProgreso(100.0);
    std::cout << "\n\nCarga Pajek completada.\n";
    return true;
}

int main() {
    GrafoDirigido grafo;
    std::string nombreArchivoCsv = "datasets/imdbActorsNetwork.csv";
    std::string nombreArchivoPajek = "datasets/tradeNetwork2018.net";
    std::string nombreDummyCsv = "datasets/dummy.csv";
    std::string nombreDummyPajek = "datasets/dummy.net";

    cargarDesdePajek(grafo, nombreDummyPajek);
    
    // ===== Centralidad de grado =====

    std::cout << "Centralidad de grado: " << grafo.calcularCentralidadGrado("A", false, true) << std::endl;

    // Mostramos las estadísticas de tamaño
    grafo.imprimirInformacion();

    // ===== Betweenness Centrality =====

    auto betweenness = grafo.calcularBetweennessCentrality(false);

    for (const auto& par : betweenness) {
        std::cout << "Nodo [" << par.first << "]: " << par.second << "\n";
    }

    // ===== PageRank =====

    auto pagerank = grafo.calcularPageRank();

    // Variable para verificar que la suma de todos los PR es aproximadamente 1.0
    double sumaTotal = 0.0;

    for (const auto& par : pagerank) {
        std::cout << "Nodo [" << par.first << "]: " << par.second << "\n";
        sumaTotal += par.second;
    }

    std::cout << "---------------------------\n";
    std::cout << "Suma total de PageRank: " << sumaTotal << " (Debe ser ~1.0)\n";

    // ===== Average Shortest Path Length =====
    std::cout << "Average Shortest Path Length: " << grafo.calcularAverageShortestPathLength() << std::endl;

    // ===== Diametro =====
    std::cout << "Diametro del grafo: " << grafo.calcularDiametro() << std::endl;

    // ===== Closeness Centrality =====
    auto closeness = grafo.calcularClosenessCentrality(0.5, 0.5);
    std::cout << "\nCloseness Centrality (Aproximado, epsilon=0.5, delta=0.5):\n";
    for (const auto& par : closeness) {
        std::cout << "Nodo [" << par.first << "]: " << par.second << "\n";
    }

    // ===== Excentricidad =====
    auto excentricidades = grafo.calcularExcentricidad();
    std::cout << "\nExcentricidades:\n";
    for (const auto& par : excentricidades) {
        std::cout << "Nodo [" << par.first << "]: " << par.second << "\n";
    }

    return 0;
}