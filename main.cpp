#include "directedGraph.cpp"

int main() {
    GrafoDirigido redComercio;

    // Simulando los datos reales del dataset Trade Network (.net)
    // Formato: Origen, Destino, Peso (Flujo de comercio)
    
    // Conexiones salientes de Albania (ALB)
    redComercio.agregarArista("ALB", "ARE", 1197.07);
    redComercio.agregarArista("ALB", "ARG", 162.55);
    redComercio.agregarArista("ALB", "ARM", 147.91);
    
    // Conexiones salientes de Argentina (ARG)
    // Nota que el peso de ARG a ALB es distinto, confirmando que es dirigido.
    redComercio.agregarArista("ARG", "ALB", 2.20); 
    redComercio.agregarArista("ARG", "AGO", 15365.50);

    // Mostramos la estructura
    redComercio.imprimirGrafo();
    
    // Mostramos las estadísticas de tamaño
    redComercio.imprimirEstadisticas();

    return 0;
}