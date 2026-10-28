#include <SFML/Graphics.hpp>

#include <iostream>
#include <algorithm>
#include <cmath>
#include <cstddef>
#include <optional>
#include <stdexcept>
#include <string>

struct Node
{
    long long value;
    Node* nodes[2];

    int inicio;
    int fin;

    float x;
    float y;

    Node(long long v, int i, int f)
        : value(v), nodes{nullptr, nullptr},
          inicio(i), fin(f), x(0), y(0)
    {
    }
};

class CBinTree
{
public:
    CBinTree()
        : root(nullptr), N(0), niveles(0)
    {
    }

    ~CBinTree()
    {
        destruir(root);
    }

    // Construye el arbol usando el intervalo [A, fin)
    void ArmarSuma(const int* A, const int* fin)
    {
        if (A == nullptr || fin == nullptr || fin <= A)
        {
            throw std::invalid_argument(
                "El arreglo debe contener elementos."
            );
        }

        std::size_t cantidad = fin - A;

        // Comprobar que N sea potencia de dos
        if ((cantidad & (cantidad - 1)) != 0)
        {
            throw std::invalid_argument(
                "La cantidad de elementos debe ser potencia de dos."
            );
        }

        destruir(root);
        root = nullptr;

        N = cantidad;
        niveles = 0;

        for (std::size_t i = N; i > 0; i >>= 1)
        {
            niveles++;
        }

        root = construir(A, 0, static_cast<int>(N) - 1);
    }

    void Draw()
    {
        if (root == nullptr)
        {
            std::cout << "Primero construye el arbol.\n";
            return;
        }

        // Resolucion disponible
        sf::VideoMode escritorio = sf::VideoMode::getDesktopMode();

        unsigned int pantallaW = escritorio.size.x;
        unsigned int pantallaH = escritorio.size.y;

        // Ajustar el tamano de la ventana al arbol y a la pantalla
        unsigned int deseadoW = static_cast<unsigned int>(
            std::min<std::size_t>(N * 75, 20000)
        );

        unsigned int ancho = std::min(
            std::max(600u, deseadoW),
            pantallaW > 100 ? pantallaW - 100 : pantallaW
        );

        unsigned int deseadoH = static_cast<unsigned int>(
            std::min(100u * static_cast<unsigned int>(niveles) + 100u, 20000u)
        );

        unsigned int alto = std::min(
            std::max(500u, deseadoH),
            pantallaH > 100 ? pantallaH - 100 : pantallaH
        );

        sf::RenderWindow ventana(
            sf::VideoMode({ancho, alto}),
            "Arbol binario de sumas",
            sf::Style::Titlebar |
            sf::Style::Close |
            sf::Style::Resize
        );

        ventana.setFramerateLimit(60);

        // Cargar fuente
        sf::Font fuente;

        bool fuenteCargada = fuente.openFromFile(
            "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf"
        );

        if (!fuenteCargada)
        {
            fuenteCargada = fuente.openFromFile("arial.ttf");
        }

        if (!fuenteCargada)
        {
            std::cerr << "No se pudo cargar la fuente.\n";
            return;
        }

        while (ventana.isOpen())
        {
            // Eventos en SFML 3
            while (const std::optional<sf::Event> evento =
                       ventana.pollEvent())
            {
                if (evento->is<sf::Event::Closed>())
                {
                    ventana.close();
                }
            }

            ventana.clear(sf::Color(25, 25, 30));

            // Consultar el tamano actual por si se redimensiona
            sf::Vector2u tamano = ventana.getSize();

            float margenX = 35.0f;
            float margenY = 45.0f;

            float espacioX =
                (tamano.x - 2.0f * margenX) / N;

            float espacioY = 0.0f;

            if (niveles > 1)
            {
                espacioY =
                    (tamano.y - margenY - 40.0f) / (niveles - 1);
            }

            posicionar(
                root,
                0,
                static_cast<int>(N) - 1,
                0,
                margenX,
                margenY,
                espacioX,
                espacioY
            );

            // Radio adaptable
            float radio = std::min(22.0f, espacioX * 0.30f);

            if (espacioY > 0)
            {
                radio = std::min(radio, espacioY * 0.22f);
            }

            radio = std::max(2.0f, radio);

            // Primero las ramas, despues los nodos
            dibujarRamas(ventana, root);

            dibujarNodos(
                ventana,
                root,
                fuente,
                radio
            );

            ventana.display();
        }
    }

private:
    Node* root;
    std::size_t N;
    int niveles;

    Node* construir(const int* A, int inicio, int fin)
    {
        // Una hoja representa un elemento del arreglo
        if (inicio == fin)
        {
            return new Node(A[inicio], inicio, fin);
        }

        int medio = inicio + (fin - inicio) / 2;

        Node* izquierdo = construir(A, inicio, medio);
        Node* derecho = construir(A, medio + 1, fin);

        // El padre almacena la suma de sus hijos
        Node* padre = new Node(
            izquierdo->value + derecho->value,
            inicio,
            fin
        );

        padre->nodes[0] = izquierdo;
        padre->nodes[1] = derecho;

        return padre;
    }

    void destruir(Node* n)
    {
        if (n == nullptr)
        {
            return;
        }

        destruir(n->nodes[0]);
        destruir(n->nodes[1]);

        delete n;
    }

    void posicionar(
        Node* n,
        int inicio,
        int fin,
        int profundidad,
        float margenX,
        float margenY,
        float espacioX,
        float espacioY
    )
    {
        if (n == nullptr)
        {
            return;
        }

        // Centrar el nodo en su intervalo
        float centro = (inicio + fin + 1) / 2.0f;

        n->x = margenX + centro * espacioX;
        n->y = margenY + profundidad * espacioY;

        if (inicio == fin)
        {
            return;
        }

        int medio = inicio + (fin - inicio) / 2;

        posicionar(
            n->nodes[0],
            inicio,
            medio,
            profundidad + 1,
            margenX,
            margenY,
            espacioX,
            espacioY
        );

        posicionar(
            n->nodes[1],
            medio + 1,
            fin,
            profundidad + 1,
            margenX,
            margenY,
            espacioX,
            espacioY
        );
    }

    void dibujarRamas(sf::RenderWindow& ventana, Node* n)
    {
        if (n == nullptr)
        {
            return;
        }

        for (int i = 0; i < 2; i++)
        {
            Node* hijo = n->nodes[i];

            if (hijo == nullptr)
            {
                continue;
            }

            // En SFML 3, asignamos los atributos de cada vertice
            sf::Vertex linea[2];

            linea[0].position = {n->x, n->y};
            linea[0].color = sf::Color::Yellow;

            linea[1].position = {hijo->x, hijo->y};
            linea[1].color = sf::Color::Yellow;

            ventana.draw(linea, 2, sf::PrimitiveType::Lines);

            dibujarRamas(ventana, hijo);
        }
    }

    void dibujarNodos(
        sf::RenderWindow& ventana,
        Node* n,
        sf::Font& fuente,
        float radio
    )
    {
        if (n == nullptr)
        {
            return;
        }

        sf::CircleShape circulo(radio);

        circulo.setOrigin({radio, radio});
        circulo.setPosition({n->x, n->y});

        circulo.setFillColor(sf::Color(35, 35, 40));
        circulo.setOutlineColor(sf::Color::Yellow);
        circulo.setOutlineThickness(
            std::max(1.0f, radio * 0.10f)
        );

        ventana.draw(circulo);

        // Mostrar el valor si el nodo tiene espacio suficiente
        if (radio >= 8.0f)
        {
            unsigned int tamano = static_cast<unsigned int>(
                std::clamp(radio * 0.85f, 8.0f, 20.0f)
            );

            // En SFML 3, el constructor recibe la fuente
            sf::Text texto(
                fuente,
                std::to_string(n->value),
                tamano
            );

            texto.setFillColor(sf::Color::White);

            sf::FloatRect limites = texto.getLocalBounds();

            texto.setOrigin({
                limites.position.x + limites.size.x / 2.0f,
                limites.position.y + limites.size.y / 2.0f
            });

            texto.setPosition({n->x, n->y});

            ventana.draw(texto);
        }

        dibujarNodos(
            ventana,
            n->nodes[0],
            fuente,
            radio
        );

        dibujarNodos(
            ventana,
            n->nodes[1],
            fuente,
            radio
        );
    }
};

int main()
{
    int A[] = {1,5,8,3,20,11,7,8};

    CBinTree Arbol;

    try
    {
        Arbol.ArmarSuma(A, A + std::size(A));
        Arbol.Draw();
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << '\n';
        return 1;
    }

    return 0;
}
