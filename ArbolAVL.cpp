#include <SFML/Graphics.hpp>

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <optional>
#include <set>

using namespace std;

struct Node
{
    int valor;
    int altura;

    Node* left;
    Node* right;
    Node* parent;

    float x, y;

    Node(int v)
        : valor(v),
          altura(1),
          left(nullptr),
          right(nullptr),
          parent(nullptr),
          x(0),
          y(0)
    {
    }
};

class CBinTree
{
private:
    Node* root;
    vector<int> valores;

    Node* seleccionado;
    set<Node*> primos;

    string entrada;
    string mensaje;

    int cantidadNodos;

public:

    CBinTree()
        : root(nullptr),
          seleccionado(nullptr),
          cantidadNodos(0)
    {
    }

    ~CBinTree()
    {
        destruir(root);
    }
    // Busca todos los nodos de una rama que estén
// exactamente en el nivel indicado.
void buscarPrimosEnNivel(
    Node* nodo,
    int nivelActual,
    int nivelObjetivo
)
{
    if (nodo == nullptr)
        return;

    if (nivelActual == nivelObjetivo)
    {
        if (nodo != seleccionado)
            primos.insert(nodo);

        return;
    }

    buscarPrimosEnNivel(
        nodo->left,
        nivelActual + 1,
        nivelObjetivo
    );

    buscarPrimosEnNivel(
        nodo->right,
        nivelActual + 1,
        nivelObjetivo
    );
}


// Construir el AVL desde un array.
    void ArmarArbol(const int A[], int N)
    {
        if (A == nullptr || N <= 0)
            return;

        for (int i = 0; i < N; i++)
        {
            if (find(valores.begin(), valores.end(), A[i])
                != valores.end())
            {
                cout << "Valor repetido: " << A[i] << '\n';
                continue;
            }

            valores.push_back(A[i]);
            root = insertar(root, A[i], nullptr);
        }

        cantidadNodos = static_cast<int>(valores.size());

        cout << "Arbol AVL construido correctamente.\n";
        cout << "Cantidad de nodos: " << cantidadNodos << '\n';
    }

    // Obtener altura de un nodo.
    int altura(Node* n)
    {
        return n == nullptr ? 0 : n->altura;
    }

    // Calcular factor de equilibrio.
    int factorEquilibrio(Node* n)
    {
        if (n == nullptr)
            return 0;

        return altura(n->left) - altura(n->right);
    }

    // Actualizar altura.
    void actualizarAltura(Node* n)
    {
        if (n != nullptr)
        {
            n->altura = 1 + max(
                altura(n->left),
                altura(n->right)
            );
        }
    }

    // Rotacion derecha.
    Node* rotacionDerecha(Node* y)
    {
        Node* x = y->left;
        Node* T2 = x->right;

        x->right = y;
        y->left = T2;

        x->parent = y->parent;
        y->parent = x;

        if (T2 != nullptr)
            T2->parent = y;

        actualizarAltura(y);
        actualizarAltura(x);

        return x;
    }

    // Rotacion izquierda.
    Node* rotacionIzquierda(Node* x)
    {
        Node* y = x->right;
        Node* T2 = y->left;

        y->left = x;
        x->right = T2;

        y->parent = x->parent;
        x->parent = y;

        if (T2 != nullptr)
            T2->parent = x;

        actualizarAltura(x);
        actualizarAltura(y);

        return y;
    }

    // Insertar un nodo y balancear el AVL.
    Node* insertar(Node* nodo, int valor, Node* padre)
    {
        if (nodo == nullptr)
        {
            Node* nuevo = new Node(valor);
            nuevo->parent = padre;
            return nuevo;
        }

        if (valor < nodo->valor)
        {
            nodo->left = insertar(
                nodo->left,
                valor,
                nodo
            );
        }
        else if (valor > nodo->valor)
        {
            nodo->right = insertar(
                nodo->right,
                valor,
                nodo
            );
        }
        else
        {
            return nodo;
        }

        actualizarAltura(nodo);

        int balance = factorEquilibrio(nodo);

        // Caso LL.
        if (balance > 1 && valor < nodo->left->valor)
            return rotacionDerecha(nodo);

        // Caso RR.
        if (balance < -1 && valor > nodo->right->valor)
            return rotacionIzquierda(nodo);

        // Caso LR.
        if (balance > 1 && valor > nodo->left->valor)
        {
            nodo->left = rotacionIzquierda(nodo->left);
            nodo->left->parent = nodo;

            return rotacionDerecha(nodo);
        }

        // Caso RL.
        if (balance < -1 && valor < nodo->right->valor)
        {
            nodo->right = rotacionDerecha(nodo->right);
            nodo->right->parent = nodo;

            return rotacionIzquierda(nodo);
        }

        return nodo;
    }

    // Buscar un número en el AVL.
    Node* buscar(Node* nodo, int valor)
    {
        if (nodo == nullptr || nodo->valor == valor)
            return nodo;

        if (valor < nodo->valor)
            return buscar(nodo->left, valor);

        return buscar(nodo->right, valor);
    }

    // Encontrar primos cercanos y lejanos del nodo seleccionado.
    void encontrarPrimos(Node* nodo)
    {
        primos.clear();
        seleccionado = nodo;

        if (nodo == nullptr)
            return;

        // Nivel del nodo seleccionado (la raiz esta en el nivel 0).
        int nivelSeleccionado = 0;
        Node* actual = nodo;

        while (actual->parent != nullptr)
        {
            nivelSeleccionado++;
            actual = actual->parent;
        }

        // Subimos por todos los antepasados del seleccionado.
        // En cada nivel buscamos la rama del hermano del antepasado.
        Node* ancestro = nodo->parent;

        while (ancestro != nullptr)
        {
            Node* abuelo = ancestro->parent;

            if (abuelo != nullptr)
            {
                Node* hermanoDelAncestro = nullptr;

                if (abuelo->left == ancestro)
                    hermanoDelAncestro = abuelo->right;
                else
                    hermanoDelAncestro = abuelo->left;

                if (hermanoDelAncestro != nullptr)
                {
                    // Calculamos el nivel absoluto de esa rama.
                    int nivelHermano = 0;
                    Node* temporal = hermanoDelAncestro;

                    while (temporal->parent != nullptr)
                    {
                        nivelHermano++;
                        temporal = temporal->parent;
                    }

                    // Solo son primos los descendientes que quedan
                    // a la misma profundidad que el seleccionado.
                    buscarPrimosEnNivel(
                        hermanoDelAncestro,
                        nivelHermano,
                        nivelSeleccionado
                    );
                }
            }

            ancestro = ancestro->parent;
        }
    }

// Seleccionar un número.
    void seleccionarNumero()
    {
        if (entrada.empty())
            return;

        int numero = stoi(entrada);

        Node* encontrado = buscar(root, numero);

        if (encontrado == nullptr)
        {
            seleccionado = nullptr;
            primos.clear();

            mensaje = "El numero no existe en el arbol.";
        }
        else
        {
            encontrarPrimos(encontrado);

            mensaje = "Numero seleccionado: "
                    + to_string(numero)
                    + " | Primos encontrados: "
                    + to_string(primos.size());
        }

        entrada.clear();
    }

    // Calcular posiciones mediante recorrido inorden.
    void posicionar(
        Node* nodo,
        int profundidad,
        int& indice,
        float ancho
    )
    {
        if (nodo == nullptr)
            return;

        posicionar(
            nodo->left,
            profundidad + 1,
            indice,
            ancho
        );

        float espacio = ancho / (cantidadNodos + 1);

        nodo->x = espacio * (indice + 1);
        nodo->y = 110 + profundidad * 120;

        indice++;

        posicionar(
            nodo->right,
            profundidad + 1,
            indice,
            ancho
        );
    }

    // Dibujar las ramas.
    void dibujarRamas(
        sf::RenderWindow& ventana,
        Node* nodo
    )
    {
        if (nodo == nullptr)
            return;

        if (nodo->left != nullptr)
        {
            sf::Vertex linea[2];

            linea[0].position = sf::Vector2f(
                nodo->x, nodo->y
            );

            linea[1].position = sf::Vector2f(
                nodo->left->x,
                nodo->left->y
            );

            linea[0].color = sf::Color::White;
            linea[1].color = sf::Color::White;

            ventana.draw(
                linea,
                2,
                sf::PrimitiveType::Lines
            );

            dibujarRamas(ventana, nodo->left);
        }

        if (nodo->right != nullptr)
        {
            sf::Vertex linea[2];

            linea[0].position = sf::Vector2f(
                nodo->x, nodo->y
            );

            linea[1].position = sf::Vector2f(
                nodo->right->x,
                nodo->right->y
            );

            linea[0].color = sf::Color::White;
            linea[1].color = sf::Color::White;

            ventana.draw(
                linea,
                2,
                sf::PrimitiveType::Lines
            );

            dibujarRamas(ventana, nodo->right);
        }
    }

    // Dibujar los nodos.
    void dibujarNodos(
        sf::RenderWindow& ventana,
        Node* nodo,
        sf::Font& fuente
    )
    {
        if (nodo == nullptr)
            return;

        float radio = 30;

        sf::CircleShape circulo(radio);

        circulo.setOrigin({radio, radio});
        circulo.setPosition({nodo->x, nodo->y});

        // Colores de los nodos.
        if (nodo == seleccionado)
        {
            circulo.setFillColor(sf::Color::Red);
        }
        else if (primos.count(nodo) > 0)
        {
            circulo.setFillColor(sf::Color(255, 140, 0));
        }
        else
        {
            circulo.setFillColor(sf::Color(45, 100, 180));
        }

        circulo.setOutlineColor(sf::Color::White);
        circulo.setOutlineThickness(2);

        ventana.draw(circulo);

        sf::Text texto(
            fuente,
            to_string(nodo->valor),
            22
        );

        texto.setFillColor(sf::Color::White);

        sf::FloatRect limites = texto.getLocalBounds();

        texto.setOrigin({
            limites.position.x + limites.size.x / 2,
            limites.position.y + limites.size.y / 2
        });

        texto.setPosition({nodo->x, nodo->y});

        ventana.draw(texto);

        dibujarNodos(ventana, nodo->left, fuente);
        dibujarNodos(ventana, nodo->right, fuente);
    }

    // Liberar memoria.
    void destruir(Node* nodo)
    {
        if (nodo == nullptr)
            return;

        destruir(nodo->left);
        destruir(nodo->right);

        delete nodo;
    }

    // Dibujar ventana.
    void Draw()
    {
        if (root == nullptr)
            return;

        unsigned int ancho = max(
            1000u,
            static_cast<unsigned int>(cantidadNodos * 100 + 100)
        );

        unsigned int alto = max(
            650u,
            static_cast<unsigned int>(altura(root) * 130 + 200)
        );

        sf::RenderWindow ventana(
            sf::VideoMode({ancho, alto}),
            "Arbol AVL - Primos Familiares",
            sf::Style::Titlebar | sf::Style::Close
        );

        ventana.setFramerateLimit(60);

        sf::Font fuente;

        if (!fuente.openFromFile(
            "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf"
        ))
        {
            cerr << "No se pudo cargar la fuente.\n";
            return;
        }

        int indice = 0;

        posicionar(
            root,
            0,
            indice,
            static_cast<float>(ancho)
        );

        while (ventana.isOpen())
        {
            while (auto evento = ventana.pollEvent())
            {
                if (evento->is<sf::Event::Closed>())
                {
                    ventana.close();
                }

                // Leer los caracteres escritos.
                if (const auto* texto =
                    evento->getIf<sf::Event::TextEntered>())
                {
                    if (texto->unicode >= U'0' &&
                        texto->unicode <= U'9')
                    {
                        entrada += static_cast<char>(
                            texto->unicode
                        );
                    }
                }

                if (const auto* tecla =
                    evento->getIf<sf::Event::KeyPressed>())
                {
                    if (tecla->code ==
                        sf::Keyboard::Key::Backspace)
                    {
                        if (!entrada.empty())
                            entrada.pop_back();
                    }

                    if (tecla->code ==
                        sf::Keyboard::Key::Enter)
                    {
                        seleccionarNumero();
                    }
                }
            }

            ventana.clear(sf::Color(25, 25, 30));

            dibujarRamas(ventana, root);

            dibujarNodos(
                ventana,
                root,
                fuente
            );

            // Campo de entrada.
            sf::Text textoEntrada(
                fuente,
                "Selecciona un numero: " + entrada,
                22
            );

            textoEntrada.setPosition({20, 15});
            textoEntrada.setFillColor(sf::Color::White);

            ventana.draw(textoEntrada);

            // Mensaje de resultado.
            sf::Text textoMensaje(
                fuente,
                mensaje,
                17
            );

            textoMensaje.setPosition({20, 50});
            textoMensaje.setFillColor(sf::Color::Yellow);

            ventana.draw(textoMensaje);

            // Leyenda.
            sf::Text leyenda(
                fuente,
                "Azul: Otros nodos | Rojo: Seleccionado | "
                "Naranja: Primos",
                16
            );

            leyenda.setPosition({
                20.0f,
                static_cast<float>(alto - 35)
            });
            leyenda.setFillColor(sf::Color::White);

            ventana.draw(leyenda);

            ventana.display();
        }
    }
};

int main()
{
    // Array de valores unicos.
    int A[] = {
        20, 10, 50, 5, 15, 30, 60, 3, 7, 11, 17, 40, 70
    };

    int N = sizeof(A) / sizeof(A[0]);

    CBinTree Arbol;

    Arbol.ArmarArbol(A, N);

    Arbol.Draw();

    return 0;
}

// g++ ArbolAVL.cpp -o ArbolAVL -lsfml-graphics -lsfml-window -lsfml-system
// ./ArbolAVL
