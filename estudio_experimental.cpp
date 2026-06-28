#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <chrono>
#include <cmath>
#include <iomanip>
#include "directedGraph.cpp"

using namespace std;

// Declaración e implementación de funciones auxiliares de carga de dataset
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

void cargarDesdeCsvNoDirigido(GrafoDirigido& grafoDirigido, const std::string& nombreArchivo) {
    std::ifstream archivo(nombreArchivo, std::ios::binary | std::ios::ate);
    if (!archivo.is_open()) {
        std::cerr << "Error: No se pudo abrir el archivo " << nombreArchivo << "\n";
        return;
    }

    std::streamsize tamanoTotalBytes = archivo.tellg();
    archivo.seekg(0, std::ios::beg);

    std::string linea;
    if (!std::getline(archivo, linea)) return;

    std::cout << "Iniciando la lectura del dataset CSV...\n";
    std::string origen, destino, pesoStr;
    double peso;
    long long lineasProcesadas = 0;

    while (std::getline(archivo, linea)) {
        if (linea.empty()) continue;
        std::stringstream ss(linea);
        if (std::getline(ss, origen, ',') &&
            std::getline(ss, destino, ',') &&
            std::getline(ss, pesoStr, ',')) {
            peso = std::stod(pesoStr);
            grafoDirigido.agregarArista(origen, destino, peso);
            grafoDirigido.agregarArista(destino, origen, peso);
        }
        lineasProcesadas++;
        if (lineasProcesadas % 1000 == 0) {
            std::streamsize bytesLeidos = archivo.tellg();
            double porcentaje = (static_cast<double>(bytesLeidos) / tamanoTotalBytes) * 100.0;
            mostrarProgreso(porcentaje);
        }
    }
    mostrarProgreso(100.0);
    std::cout << "\nCarga CSV completada con exito.\n";
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
    std::unordered_map<int, std::string> mapaIdsNodos;
    bool leyendoVertices = false;
    bool leyendoArcos = false;
    long long lineasProcesadas = 0;

    while (std::getline(archivo, linea)) {
        if (!linea.empty() && linea.back() == '\r') linea.pop_back();
        if (linea.empty()) continue;

        if (linea.find("*Vertices") == 0) {
            leyendoVertices = true;
            leyendoArcos = false;
            continue; 
        } else if (linea.find("*Arcs") == 0 || linea.find("*Edges") == 0) {
            leyendoVertices = false;
            leyendoArcos = true;
            continue;
        }

        if (leyendoVertices) {
            int id;
            std::istringstream iss(linea);
            iss >> id;
            size_t inicioComillas = linea.find('"');
            size_t finComillas = linea.find('"', inicioComillas + 1);
            if (inicioComillas != std::string::npos && finComillas != std::string::npos) {
                std::string nombreNodo = linea.substr(inicioComillas + 1, finComillas - inicioComillas - 1);
                mapaIdsNodos[id] = nombreNodo;
            }
        } else if (leyendoArcos) {
            int idOrigen, idDestino;
            double peso;
            std::istringstream iss(linea);
            if (iss >> idOrigen >> idDestino >> peso) {
                if (mapaIdsNodos.count(idOrigen) && mapaIdsNodos.count(idDestino)) {
                    std::string nombreOrigen = mapaIdsNodos[idOrigen];
                    std::string nombreDestino = mapaIdsNodos[idDestino];
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
    std::cout << "\nCarga Pajek completada.\n";
    return true;
}

void ejecutarMetricas(GrafoDirigido& grafo1, GrafoDirigido& grafo2, const std::string& dataset1, const std::string& dataset2) {

    // =========================================================
    // 1. CENTRALIDAD DE GRADO
    // =========================================================
    vector<double> tiempo_centralidad1;
    vector<double> tiempo_centralidad2;

    //calculo de la centralidad de grado 10 veces
    for(int i = 0; i < 10; i++){
        auto inicio_centralidad1 = chrono::high_resolution_clock::now();
        grafo1.calcularCentralidadGrado("Chile"); // vertice arbitrario
        auto fin_centralidad1 = chrono::high_resolution_clock::now();
        tiempo_centralidad1.push_back(chrono::duration<double, std::milli>(fin_centralidad1 - inicio_centralidad1).count());
        
        auto inicio_centralidad2 = chrono::high_resolution_clock::now();
        grafo2.calcularCentralidadGrado("USA");        // vertice arbitrario
        auto fin_centralidad2 = chrono::high_resolution_clock::now();
        tiempo_centralidad2.push_back(chrono::duration<double, std::milli>(fin_centralidad2 - inicio_centralidad2).count());
    }

    //calculo del promedio de tiempo de la centralidad de grado
    double promedio_centralidad1 = 0.0;
    double promedio_centralidad2 = 0.0;

    for(int i = 0; i < 10; i++){
        promedio_centralidad1 += tiempo_centralidad1[i];
        promedio_centralidad2 += tiempo_centralidad2[i];
    }

    promedio_centralidad1 = promedio_centralidad1 / 10.0;
    promedio_centralidad2 = promedio_centralidad2 / 10.0;

    //calculo de la varianza de datos calculados
    double varianza_centralidad1 = 0.0;
    double varianza_centralidad2 = 0.0;

    for(int i = 0; i < 10; i++){
        varianza_centralidad1 += pow(tiempo_centralidad1[i] - promedio_centralidad1, 2);
        varianza_centralidad2 += pow(tiempo_centralidad2[i] - promedio_centralidad2, 2);
    }

    varianza_centralidad1 = varianza_centralidad1 / 10.0;
    varianza_centralidad2 = varianza_centralidad2 / 10.0;

    //muestreo en pantalla de los resultados de tiempo
    std::cout << "Dataset 1: " << dataset1 << "         " << "Tiempo promedio de centralidad de grado: " << promedio_centralidad1 << " ms\n";
    std::cout << "Dataset 2: " << dataset2 << "        " << "Tiempo promedio de centralidad de grado: " << promedio_centralidad2 << " ms\n";
    std::cout << "Dataset 1: " << dataset1 << "         " << "Varianza de datos: " << varianza_centralidad1 << " ms\n";
    std::cout << "Dataset 2: " << dataset2 << "        " << "Varianza de datos: " << varianza_centralidad2 << " ms\n\n";

    // =========================================================
    // 2. BETWEENNESS CENTRALITY
    // =========================================================
    vector<double> tiempo_betweenness1;
    vector<double> tiempo_betweenness2;

    for(int i = 0; i < 10; i++){
        //medicion del tiempo de la centralidad de betweenness centrality
        auto inicio_betweenness1 = chrono::high_resolution_clock::now();
        grafo1.calcularBetweennessCentrality(true);
        auto fin_betweenness1 = chrono::high_resolution_clock::now();
        tiempo_betweenness1.push_back(chrono::duration<double, std::milli>(fin_betweenness1 - inicio_betweenness1).count());
        
        auto inicio_betweenness2 = chrono::high_resolution_clock::now();
        grafo2.calcularBetweennessCentrality(true);
        auto fin_betweenness2 = chrono::high_resolution_clock::now();
        tiempo_betweenness2.push_back(chrono::duration<double, std::milli>(fin_betweenness2 - inicio_betweenness2).count());
    }

    //calculo del promedio del tiempo de betweenness centrality
    double promedio_betweenness1 = 0.0;
    double promedio_betweenness2 = 0.0;
    for(int i = 0; i < 10; i++){
        promedio_betweenness1 += tiempo_betweenness1[i];
        promedio_betweenness2 += tiempo_betweenness2[i];
    }
    promedio_betweenness1 = promedio_betweenness1 / 10.0;
    promedio_betweenness2 = promedio_betweenness2 / 10.0;

    //calculo de la varianza del tiempo de betweenness centrality
    double varianza_betweenness1 = 0.0;
    double varianza_betweenness2 = 0.0;
    for(int i = 0; i < 10; i++){
        varianza_betweenness1 += pow(tiempo_betweenness1[i] - promedio_betweenness1, 2);
        varianza_betweenness2 += pow(tiempo_betweenness2[i] - promedio_betweenness2, 2);
    }
    varianza_betweenness1 = varianza_betweenness1 / 10.0;
    varianza_betweenness2 = varianza_betweenness2 / 10.0;

    //muestreo en pantalla de los resultados de tiempo
    std::cout << "Dataset 1: " << dataset1 << "         " << "Tiempo promedio de Betweenness Centrality: " << promedio_betweenness1 << " ms\n";
    std::cout << "Dataset 2: " << dataset2 << "        " << "Tiempo promedio de Betweenness Centrality: " << promedio_betweenness2 << " ms\n";
    std::cout << "Dataset 1: " << dataset1 << "         " << "Varianza de datos: " << varianza_betweenness1 << " ms\n";
    std::cout << "Dataset 2: " << dataset2 << "        " << "Varianza de datos: " << varianza_betweenness2 << " ms\n\n";

    // =========================================================
    // 3. PAGERANK
    // =========================================================
    vector<double> tiempo_pagerank1;
    vector<double> tiempo_pagerank2;

    //medicion del tiempo de pagerank
    for(int i = 0; i < 10; i++){
        auto inicio_pagerank1 = chrono::high_resolution_clock::now();
        grafo1.calcularPageRank();
        auto fin_pagerank1 = chrono::high_resolution_clock::now();
        tiempo_pagerank1.push_back(chrono::duration<double, std::milli>(fin_pagerank1 - inicio_pagerank1).count());

        auto inicio_pagerank2 = chrono::high_resolution_clock::now();
        grafo2.calcularPageRank();
        auto fin_pagerank2 = chrono::high_resolution_clock::now();
        tiempo_pagerank2.push_back(chrono::duration<double, std::milli>(fin_pagerank2 - inicio_pagerank2).count());
    }

    //calculo del promedio del tiempo de pagerank
    double promedio_pagerank1 = 0.0;
    double promedio_pagerank2 = 0.0;
    for(int i = 0; i < 10; i++){
        promedio_pagerank1 += tiempo_pagerank1[i];
        promedio_pagerank2 += tiempo_pagerank2[i];
    }
    promedio_pagerank1 = promedio_pagerank1 / 10.0;
    promedio_pagerank2 = promedio_pagerank2 / 10.0;

    //calculo de la varianza del tiempo de pagerank
    double varianza_pagerank1 = 0.0;
    double varianza_pagerank2 = 0.0;
    for(int i = 0; i < 10; i++){
        varianza_pagerank1 += pow(tiempo_pagerank1[i] - promedio_pagerank1, 2);
        varianza_pagerank2 += pow(tiempo_pagerank2[i] - promedio_pagerank2, 2);
    }
    varianza_pagerank1 = varianza_pagerank1 / 10.0;
    varianza_pagerank2 = varianza_pagerank2 / 10.0;

    //muestreo en pantalla de los resultados de tiempo
    std::cout << "Dataset 1: " << dataset1 << "         " << "Tiempo promedio de Page Rank: " << promedio_pagerank1 << " ms\n";
    std::cout << "Dataset 2: " << dataset2 << "        " << "Tiempo promedio de Page Rank: " << promedio_pagerank2 << " ms\n";
    std::cout << "Dataset 1: " << dataset1 << "         " << "Varianza de datos: " << varianza_pagerank1 << " ms\n";
    std::cout << "Dataset 2: " << dataset2 << "        " << "Varianza de datos: " << varianza_pagerank2 << " ms\n\n";

    // =========================================================
    // 4. AVERAGE SHORTEST PATH LENGTH
    // =========================================================
    vector<double> tiempo_asp1;
    vector<double> tiempo_asp2;

    //medicion del tiempo de average shortest path length
    for(int i = 0; i < 10; i++){
        auto inicio_asp1 = chrono::high_resolution_clock::now();
        grafo1.calcularAverageShortestPathLength();
        auto fin_asp1 = chrono::high_resolution_clock::now();
        tiempo_asp1.push_back(chrono::duration<double, std::milli>(fin_asp1 - inicio_asp1).count());

        auto inicio_asp2 = chrono::high_resolution_clock::now();
        grafo2.calcularAverageShortestPathLength();
        auto fin_asp2 = chrono::high_resolution_clock::now();
        tiempo_asp2.push_back(chrono::duration<double, std::milli>(fin_asp2 - inicio_asp2).count());
    }

    //calculo del promedio del tiempo de average shortest path length
    double promedio_asp1 = 0.0;
    double promedio_asp2 = 0.0;
    for(int i = 0; i < 10; i++){
        promedio_asp1 += tiempo_asp1[i];
        promedio_asp2 += tiempo_asp2[i];
    }
    promedio_asp1 = promedio_asp1 / 10.0;
    promedio_asp2 = promedio_asp2 / 10.0;

    //calculo de la varianza del tiempo de average shortest path length
    double varianza_asp1 = 0.0;
    double varianza_asp2 = 0.0;
    for(int i = 0; i < 10; i++){
        varianza_asp1 += pow(tiempo_asp1[i] - promedio_asp1, 2);
        varianza_asp2 += pow(tiempo_asp2[i] - promedio_asp2, 2);
    }
    varianza_asp1 = varianza_asp1 / 10.0;
    varianza_asp2 = varianza_asp2 / 10.0;

    //muestreo en pantalla de los resultados de tiempo
    std::cout << "Dataset 1: " << dataset1 << "         " << "Tiempo promedio de Average Shortest Path: " << promedio_asp1 << " ms\n";
    std::cout << "Dataset 2: " << dataset2 << "        " << "Tiempo promedio de Average Shortest Path: " << promedio_asp2 << " ms\n";
    std::cout << "Dataset 1: " << dataset1 << "         " << "Varianza de datos: " << varianza_asp1 << " ms\n";
    std::cout << "Dataset 2: " << dataset2 << "        " << "Varianza de datos: " << varianza_asp2 << " ms\n\n";

    // =========================================================
    // 5. DIÁMETRO
    // =========================================================
    vector<double> tiempo_diametro1;
    vector<double> tiempo_diametro2;

    //medicion del tiempo de diametro
    for(int i = 0; i < 10; i++){
        auto inicio_diametro1 = chrono::high_resolution_clock::now();
        grafo1.calcularDiametro();
        auto fin_diametro1 = chrono::high_resolution_clock::now();
        tiempo_diametro1.push_back(chrono::duration<double, std::milli>(fin_diametro1 - inicio_diametro1).count());

        auto inicio_diametro2 = chrono::high_resolution_clock::now();
        grafo2.calcularDiametro();
        auto fin_diametro2 = chrono::high_resolution_clock::now();
        tiempo_diametro2.push_back(chrono::duration<double, std::milli>(fin_diametro2 - inicio_diametro2).count());
    }

    //calculo del promedio del tiempo de diametro
    double promedio_diametro1 = 0.0;
    double promedio_diametro2 = 0.0;
    for(int i = 0; i < 10; i++){
        promedio_diametro1 += tiempo_diametro1[i];
        promedio_diametro2 += tiempo_diametro2[i];
    }
    promedio_diametro1 = promedio_diametro1 / 10.0;
    promedio_diametro2 = promedio_diametro2 / 10.0;

    //calculo de la varianza del tiempo de diametro
    double varianza_diametro1 = 0.0;
    double varianza_diametro2 = 0.0;
    for(int i = 0; i < 10; i++){
        varianza_diametro1 += pow(tiempo_diametro1[i] - promedio_diametro1, 2);
        varianza_diametro2 += pow(tiempo_diametro2[i] - promedio_diametro2, 2);
    }
    varianza_diametro1 = varianza_diametro1 / 10.0;
    varianza_diametro2 = varianza_diametro2 / 10.0;

    //muestreo en pantalla de los resultados de tiempo
    std::cout << "Dataset 1: " << dataset1 << "         " << "Tiempo promedio de Diametro: " << promedio_diametro1 << " ms\n";
    std::cout << "Dataset 2: " << dataset2 << "        " << "Tiempo promedio de Diametro: " << promedio_diametro2 << " ms\n";
    std::cout << "Dataset 1: " << dataset1 << "         " << "Varianza de datos: " << varianza_diametro1 << " ms\n";
    std::cout << "Dataset 2: " << dataset2 << "        " << "Varianza de datos: " << varianza_diametro2 << " ms\n\n";

    // =========================================================
    // 6. CLOSENESS CENTRALITY
    // =========================================================
    vector<double> tiempo_closeness1;
    vector<double> tiempo_closeness2;

    //medicion del tiempo de closeness centrality
    for(int i = 0; i < 10; i++){
        auto inicio_closeness1 = chrono::high_resolution_clock::now();
        grafo1.calcularClosenessCentrality("Chile"); // vertice arbitrario
        auto fin_closeness1 = chrono::high_resolution_clock::now();
        tiempo_closeness1.push_back(chrono::duration<double, std::milli>(fin_closeness1 - inicio_closeness1).count());

        auto inicio_closeness2 = chrono::high_resolution_clock::now();
        grafo2.calcularClosenessCentrality("USA");        // vertice arbitrario
        auto fin_closeness2 = chrono::high_resolution_clock::now();
        tiempo_closeness2.push_back(chrono::duration<double, std::milli>(fin_closeness2 - inicio_closeness2).count());
    }

    //calculo del promedio del tiempo de closeness centrality
    double promedio_closeness1 = 0.0;
    double promedio_closeness2 = 0.0;
    for(int i = 0; i < 10; i++){
        promedio_closeness1 += tiempo_closeness1[i];
        promedio_closeness2 += tiempo_closeness2[i];
    }
    promedio_closeness1 = promedio_closeness1 / 10.0;
    promedio_closeness2 = promedio_closeness2 / 10.0;

    //calculo de la varianza del tiempo de closeness centrality
    double varianza_closeness1 = 0.0;
    double varianza_closeness2 = 0.0;
    for(int i = 0; i < 10; i++){
        varianza_closeness1 += pow(tiempo_closeness1[i] - promedio_closeness1, 2);
        varianza_closeness2 += pow(tiempo_closeness2[i] - promedio_closeness2, 2);
    }
    varianza_closeness1 = varianza_closeness1 / 10.0;
    varianza_closeness2 = varianza_closeness2 / 10.0;

    //muestreo en pantalla de los resultados de tiempo
    std::cout << "Dataset 1: " << dataset1 << "         " << "Tiempo promedio de Closeness Centrality: " << promedio_closeness1 << " ms\n";
    std::cout << "Dataset 2: " << dataset2 << "        " << "Tiempo promedio de Closeness Centrality: " << promedio_closeness2 << " ms\n";
    std::cout << "Dataset 1: " << dataset1 << "         " << "Varianza de datos: " << varianza_closeness1 << " ms\n";
    std::cout << "Dataset 2: " << dataset2 << "        " << "Varianza de datos: " << varianza_closeness2 << " ms\n\n";

    // =========================================================
    // 7. EXCENTRICIDAD
    // =========================================================
    vector<double> tiempo_eccentricity1;
    vector<double> tiempo_eccentricity2;

    //medicion del tiempo de excentricidad
    for(int i = 0; i < 10; i++){
        auto inicio_eccentricity1 = chrono::high_resolution_clock::now();
        grafo1.calcularExcentricidad("Chile"); // vertice arbitrario
        auto fin_eccentricity1 = chrono::high_resolution_clock::now();
        tiempo_eccentricity1.push_back(chrono::duration<double, std::milli>(fin_eccentricity1 - inicio_eccentricity1).count());

        auto inicio_eccentricity2 = chrono::high_resolution_clock::now();
        grafo2.calcularExcentricidad("USA");        // vertice arbitrario
        auto fin_eccentricity2 = chrono::high_resolution_clock::now();
        tiempo_eccentricity2.push_back(chrono::duration<double, std::milli>(fin_eccentricity2 - inicio_eccentricity2).count());
    }

    //calculo del promedio del tiempo de excentricidad
    double promedio_eccentricity1 = 0.0;
    double promedio_eccentricity2 = 0.0;
    for(int i = 0; i < 10; i++){
        promedio_eccentricity1 += tiempo_eccentricity1[i];
        promedio_eccentricity2 += tiempo_eccentricity2[i];
    }
    promedio_eccentricity1 = promedio_eccentricity1 / 10.0;
    promedio_eccentricity2 = promedio_eccentricity2 / 10.0;

    //calculo de la varianza del tiempo de excentricidad
    double varianza_eccentricity1 = 0.0;
    double varianza_eccentricity2 = 0.0;
    for(int i = 0; i < 10; i++){
        varianza_eccentricity1 += pow(tiempo_eccentricity1[i] - promedio_eccentricity1, 2);
        varianza_eccentricity2 += pow(tiempo_eccentricity2[i] - promedio_eccentricity2, 2);
    }
    varianza_eccentricity1 = varianza_eccentricity1 / 10.0;
    varianza_eccentricity2 = varianza_eccentricity2 / 10.0;

    //muestreo en pantalla de los resultados de tiempo
    std::cout << "Dataset 1: " << dataset1 << "         " << "Tiempo promedio de Excentricidad: " << promedio_eccentricity1 << " ms\n";
    std::cout << "Dataset 2: " << dataset2 << "        " << "Tiempo promedio de Excentricidad: " << promedio_eccentricity2 << " ms\n";
    std::cout << "Dataset 1: " << dataset1 << "         " << "Varianza de datos: " << varianza_eccentricity1 << " ms\n";
    std::cout << "Dataset 2: " << dataset2 << "        " << "Varianza de datos: " << varianza_eccentricity2 << " ms\n\n";

}

int main() {
    GrafoDirigido grafo1;
    GrafoDirigido grafo2;
    
    // std::string dataset1 = "datasets/imdbActorsNetwork.csv";
    std::string dataset1 = "datasets/imports_manufactures.net";
    std::string dataset2 = "datasets/tradeNetwork2018.net";

    auto inicioCargaDataset1 = std::chrono::high_resolution_clock::now();
    cargarDesdePajek(grafo1, dataset1);
    auto finCargaDataset1 = std::chrono::high_resolution_clock::now();
    
    auto inicioCargaDataset2 = std::chrono::high_resolution_clock::now();
    cargarDesdePajek(grafo2, dataset2);
    auto finCargaDataset2 = std::chrono::high_resolution_clock::now();

    std::cout << "\n==================================================\n";
    std::cout << "Dataset 1: " << dataset1 << "         " << "Tiempo de Construccion de Grafo: " << std::chrono::duration<double, std::milli>(finCargaDataset1 - inicioCargaDataset1).count() << " ms\n";
    std::cout << "Dataset 2: " << dataset2 << "        " << "Tiempo de Construccion de Grafo: " << std::chrono::duration<double, std::milli>(finCargaDataset2 - inicioCargaDataset2).count() << " ms\n\n";
    std::cout << "Dataset 1: " << dataset1 << "         " << "Espacio en memoria: " << grafo1.obtenerUsoMemoria() << " bytes (" << std::fixed << std::setprecision(2) << (grafo1.obtenerUsoMemoria() / 1024.0) << " KB)\n";
    std::cout << "Dataset 2: " << dataset2 << "        " << "Espacio en memoria: " << grafo2.obtenerUsoMemoria() << " bytes (" << std::fixed << std::setprecision(2) << (grafo2.obtenerUsoMemoria() / 1024.0) << " KB)\n";
    std::cout << "==================================================\n\n";

    std::cout << "--- EXPERIMENTO 1: GRAFOS ORIGINALES ---\n";
    ejecutarMetricas(grafo1, grafo2, dataset1, dataset2);

        std::cout << "\n--- EXPERIMENTO 2: AGREGANDO ARISTAS (Chile - Algeria / USA - AGO) ---\n";
    grafo1.agregarArista("Chile", "Algeria", 1.0);
    grafo2.agregarArista("USA", "AGO", 1.0);
    ejecutarMetricas(grafo1, grafo2, dataset1, dataset2);

    std::cout << "\n--- EXPERIMENTO 3: AGREGANDO ARISTAS (Chile - Bangladesh / USA - BTN) ---\n";
    // Quitar aristas del experimento 2
    grafo1.eliminarArista("Chile", "Algeria");
    grafo2.eliminarArista("USA", "AGO");

    // Agregar aristas experimento 3
    grafo1.agregarArista("Chile", "Bangladesh", 1.0);
    grafo2.agregarArista("USA", "BTN", 1.0);
    ejecutarMetricas(grafo1, grafo2, dataset1, dataset2);

    std::cout << "\n--- EXPERIMENTO 4: ELIMINACION CASO 1 (Chile-China / USA-CHL) ---\n";
    // Quitando las aristas agregadas en el experimento 3
    grafo1.eliminarArista("Chile", "Bangladesh");
    grafo2.eliminarArista("USA", "BTN");
    
    // Quitando aristas existentes originalmente (Caso 1)
    grafo1.eliminarArista("Chile", "China");
    grafo2.eliminarArista("USA", "CHL");
    ejecutarMetricas(grafo1, grafo2, dataset1, dataset2);

    std::cout << "\n--- EXPERIMENTO 5: ELIMINACION CASO 2 (Brazil-Chile / USA-DEU) ---\n";
    // Restaurando aristas del caso 1 para aislar el experimento 2
    grafo1.agregarArista("Chile", "China", 1.0);
    grafo2.agregarArista("USA", "CHL", 1.0);

    // Quitando aristas existentes en otro punto de la red (Caso 2)
    grafo1.eliminarArista("Brazil", "Chile");
    grafo2.eliminarArista("USA", "DEU");
    ejecutarMetricas(grafo1, grafo2, dataset1, dataset2);

    return 0;
}