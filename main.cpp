// ImperiumCodeCompiler.cpp - Compilador básico para el lenguaje ImperiumCode
// Basado en Warhammer 40K

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <regex>
#include <thread>
#include <chrono>

namespace Color
{
    const std::string RESET = "\033[0m";
    const std::string BLACK = "\033[30m";
    const std::string RED = "\033[31m";
    const std::string GREEN = "\033[32m";
    const std::string YELLOW = "\033[33m";
    const std::string BLUE = "\033[34m";
    const std::string MAGENTA = "\033[35m";
    const std::string CYAN = "\033[36m";
    const std::string WHITE = "\033[37m";
    const std::string BOLD = "\033[1m";
}

void animateText(const std::string &text, int delayMs = 30)
{
    for (char c : text)
    {
        std::cout << c << std::flush;
        std::this_thread::sleep_for(std::chrono::milliseconds(delayMs));
    }
    std::cout << std::endl;
}

void loadingAnimation(const std::string &message, int durationMs = 1500)
{
    const int steps = 20;
    int msPerStep = durationMs / steps;

    std::cout << message;
    for (int i = 0; i < steps; i++)
    {
        std::cout << "." << std::flush;
        std::this_thread::sleep_for(std::chrono::milliseconds(msPerStep));
    }
    std::cout << " " << Color::GREEN << "Completado" << Color::RESET << std::endl;
}

void displayAsciiArt()
{
    // Logo del Imperio (Aquila Imperial)
    std::cout << Color::YELLOW << R"(
  ________          ________
 ______/ /* ||  \ \______
  _____\___\\//___/_____
     ////|\//\\/|\\\\
           //\\
          <>\/<>
)" << Color::RESET << std::endl;

    std::string aquila = R"(
                      )" +
                         Color::YELLOW + R"(  /\                /\  )" + Color::RESET + R"(
                      )" +
                         Color::YELLOW + R"( /  \              /  \ )" + Color::RESET + R"(
                      )" +
                         Color::YELLOW + R"(/    \            /    \)" + Color::RESET + R"(
                     )" + Color::YELLOW +
                         R"(/      \          /      \)" + Color::RESET + R"(
                    )" + Color::YELLOW +
                         R"(/        \        /        \)" + Color::RESET + R"(
                   )" + Color::YELLOW +
                         R"(/          \      /          \)" + Color::RESET + R"(
                  )" + Color::YELLOW +
                         R"(/            \    /            \)" + Color::RESET + R"(
                 )" + Color::YELLOW +
                         R"(/              \  /              \)" + Color::RESET + R"(
                )" + Color::YELLOW +
                         R"(/                \/                \)" + Color::RESET + R"(
               )" + Color::YELLOW +
                         R"(/                  \                 \)" + Color::RESET + R"(
               )" + Color::RED +
                         R"(     _________________\__________________)" + Color::RESET + R"(
               )" + Color::RED +
                         R"(    /                                    \)" + Color::RESET + R"(
               )" + Color::RED +
                         R"(   /                                      \)" + Color::RESET + R"(
    )";

    std::istringstream iss(aquila);
    std::string line;
    while (std::getline(iss, line))
    {
        std::cout << line << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
}

class Lexer
{
private:
    std::string input;
    size_t position;
    std::vector<std::pair<std::string, std::string>> tokens;

    std::map<std::string, std::string> keywords = {
        {"HONOR_THE_EMPEROR", "MAIN"},
        {"DEPLOY", "FUNCTION"},
        {"PURGE", "DELETE"},
        {"EXTERMINATUS", "RETURN"},
        {"CRUSADE", "FOR"},
        {"SIEGE", "WHILE"},
        {"INQUISITION", "IF"},
        {"ELSE_DECREE", "ELSE"},
        {"HERESY_DETECTED", "TRY_CATCH"},
        {"TRANSMIT", "PRINT"},
        {"IMPERIAL_FAVOR", "RANDOM"},
        {"SERVO_SKULL_READ", "INPUT"},
        {"PRIMARCH", "INT"},
        {"GUARDSMAN", "FLOAT"},
        {"HERETIC", "BOOL"},
        {"VOXCAST", "STRING"},
        {"SQUAD", "ARRAY"},
        {"CHAPTER", "STRUCT"}};

public:
    Lexer(const std::string &source) : input(source), position(0) {}

    void tokenize()
    {
        while (position < input.length())
        {
            // Ignore whitespace
            if (isspace(input[position]))
            {
                position++;
                continue;
            }

            // Handle comments
            if (input[position] == '/' && position + 1 < input.length())
            {
                if (input[position + 1] == '/')
                { // Single line comment
                    size_t endOfLine = input.find('\n', position);
                    position = (endOfLine == std::string::npos) ? input.length() : endOfLine + 1;
                    continue;
                }
                else if (input[position + 1] == '*')
                { // Multi-line comment
                    size_t endComment = input.find("*/", position + 2);
                    position = (endComment == std::string::npos) ? input.length() : endComment + 2;
                    continue;
                }
            }

            // Handle identifiers and keywords
            if (isalpha(input[position]) || input[position] == '_')
            {
                size_t start = position;
                while (position < input.length() && (isalnum(input[position]) || input[position] == '_'))
                {
                    position++;
                }

                std::string word = input.substr(start, position - start);

                // Check if it's a keyword
                if (keywords.find(word) != keywords.end())
                {
                    tokens.push_back({keywords[word], word});
                }
                else
                {
                    tokens.push_back({"IDENTIFIER", word});
                }
                continue;
            }

            // Handle numbers
            if (isdigit(input[position]))
            {
                size_t start = position;
                bool isFloat = false;

                while (position < input.length() && (isdigit(input[position]) || input[position] == '.'))
                {
                    if (input[position] == '.')
                    {
                        if (isFloat)
                        { // Second decimal point is not part of this number
                            break;
                        }
                        isFloat = true;
                    }
                    position++;
                }

                std::string number = input.substr(start, position - start);
                tokens.push_back({isFloat ? "FLOAT_LITERAL" : "INT_LITERAL", number});
                continue;
            }

            // Handle strings
            if (input[position] == '"')
            {
                size_t start = position;
                position++; // Skip opening quote

                while (position < input.length() && input[position] != '"')
                {
                    if (input[position] == '\\' && position + 1 < input.length())
                    {
                        position += 2; // Skip escape sequence
                    }
                    else
                    {
                        position++;
                    }
                }

                if (position < input.length())
                {
                    position++; // Skip closing quote
                }

                std::string str = input.substr(start, position - start);
                tokens.push_back({"STRING_LITERAL", str});
                continue;
            }

            // Handle operators and punctuation
            std::string punct = "+-*/=<>!&|(){}[];,";
            if (punct.find(input[position]) != std::string::npos)
            {
                // Handle multi-character operators
                if (position + 1 < input.length())
                {
                    std::string op = input.substr(position, 2);
                    if (op == "==" || op == "!=" || op == "<=" || op == ">=" || op == "&&" || op == "||")
                    {
                        tokens.push_back({"OPERATOR", op});
                        position += 2;
                        continue;
                    }
                }

                // Single character operator or punctuation
                tokens.push_back({"OPERATOR", std::string(1, input[position])});
                position++;
                continue;
            }

            // If we get here, we have an unrecognized character
            std::cerr << Color::RED << "Error: Unrecognized character '" << input[position] << "' at position " << position << Color::RESET << std::endl;
            position++;
        }

        // Add EOF token
        tokens.push_back({"EOF", ""});
    }

    const std::vector<std::pair<std::string, std::string>> &getTokens() const
    {
        return tokens;
    }
};

class Parser
{
private:
    std::vector<std::pair<std::string, std::string>> tokens;
    size_t position;
    std::string cppOutput;

    void match(const std::string &expected)
    {
        if (position < tokens.size() && tokens[position].first == expected)
        {
            position++;
        }
        else
        {
            std::string found = position < tokens.size() ? tokens[position].first : "EOF";
            std::cerr << Color::RED << "Error: Expected " << expected << " but found " << found << Color::RESET << std::endl;
        }
    }

    void parseProgram()
    {
        cppOutput += "#include <iostream>\n#include <string>\n#include <vector>\n#include <cstdlib>\n#include <ctime>\n\n";
        cppOutput += "// Definiciones de tipos de ImperiumCode\ntypedef int PRIMARCH;\ntypedef float GUARDSMAN;\n";
        cppOutput += "typedef bool HERETIC;\ntypedef std::string VOXCAST;\ntemplate<typename T> using SQUAD = std::vector<T>;\n\n";
        cppOutput += "// Funciones del sistema ImperiumCode\nfloat IMPERIAL_FAVOR() { return static_cast<float>(rand()) / RAND_MAX; }\n";
        cppOutput += "template<typename T> void TRANSMIT(T message) { std::cout << message << std::endl; }\n";
        cppOutput += "template<typename T> T SERVO_SKULL_READ() { T input; std::cin >> input; return input; }\n\n";

        // Buscar el punto de entrada principal (HONOR_THE_EMPEROR)
        while (position < tokens.size() - 1)
        {
            if (tokens[position].first == "MAIN" && tokens[position].second == "HONOR_THE_EMPEROR")
            {
                parseMainFunction();
                break;
            }
            else
            {
                // Analizar otras declaraciones globales (funciones, estructuras, etc.)
                if (tokens[position].first == "FUNCTION" && tokens[position].second == "DEPLOY")
                {
                    parseFunction();
                }
                else if (tokens[position].first == "STRUCT" && tokens[position].second == "CHAPTER")
                {
                    parseStruct();
                }
                else
                {
                    position++; // Saltarse tokens no reconocidos en el nivel global
                }
            }
        }

        // Asegurarse de que encontramos el punto de entrada principal
        if (position >= tokens.size() - 1)
        {
            std::cerr << Color::RED << "Error: No se encontró el punto de entrada principal HONOR_THE_EMPEROR" << Color::RESET << std::endl;
        }
    }

    void parseMainFunction()
    {
        match("MAIN");
        cppOutput += "int main() {\n";
        cppOutput += "    srand(static_cast<unsigned int>(time(nullptr)));\n";

        // Esperar un bloque de código
        if (position < tokens.size() && tokens[position].first == "OPERATOR" && tokens[position].second == "{")
        {
            match("OPERATOR");
            parseBlock();
        }
        else
        {
            std::cerr << Color::RED << "Error: Se esperaba un bloque de código después de HONOR_THE_EMPEROR" << Color::RESET << std::endl;
        }

        cppOutput += "    return 0;\n}\n";
    }

    void parseFunction()
    {
        match("FUNCTION");
        if (position < tokens.size() && tokens[position].first == "IDENTIFIER")
        {
            std::string funcName = tokens[position].second;
            match("IDENTIFIER");

            // Parsear parámetros y cuerpo de la función
            if (position < tokens.size() && tokens[position].first == "OPERATOR" && tokens[position].second == "(")
            {
                match("OPERATOR");
                // Aquí iría la lógica para parsear los parámetros
                while (position < tokens.size() && !(tokens[position].first == "OPERATOR" && tokens[position].second == ")"))
                {
                    position++;
                }
                if (position < tokens.size() && tokens[position].first == "OPERATOR" && tokens[position].second == ")")
                {
                    match("OPERATOR");
                }
            }

            // Parsear el cuerpo de la función
            if (position < tokens.size() && tokens[position].first == "OPERATOR" && tokens[position].second == "{")
            {
                match("OPERATOR");
                parseBlock();
            }
        }
    }

    void parseBlock(const std::string &indent = "    ")
    {
        while (position < tokens.size() && !(tokens[position].first == "OPERATOR" && tokens[position].second == "}"))
        {
            parseStatement(indent);
        }

        // Cerrar el bloque
        if (position < tokens.size() && tokens[position].first == "OPERATOR" && tokens[position].second == "}")
        {
            match("OPERATOR");
        }
        else
        {
            std::cerr << "Error: Se esperaba '}' para cerrar el bloque" << std::endl;
        }
    }

    void parseStruct()
    {
        match("STRUCT");
        if (position < tokens.size() && tokens[position].first == "IDENTIFIER")
        {
            std::string structName = tokens[position].second;
            match("IDENTIFIER");

            // Parsear el cuerpo de la estructura
            if (position < tokens.size() && tokens[position].first == "OPERATOR" && tokens[position].second == "{")
            {
                match("OPERATOR");
                // Aquí iría la lógica para parsear los miembros y métodos
                while (position < tokens.size() && !(tokens[position].first == "OPERATOR" && tokens[position].second == "}"))
                {
                    position++;
                }
                if (position < tokens.size() && tokens[position].first == "OPERATOR" && tokens[position].second == "}")
                {
                    match("OPERATOR");
                }
            }
        }
    }

    void parseMethod(const std::string &structName)
    {
        match("FUNCTION");

        // Obtener el nombre del método
        if (position < tokens.size() && tokens[position].first == "IDENTIFIER")
        {
            std::string methodName = tokens[position].second;
            match("IDENTIFIER");

            // Procesar parámetros
            if (position < tokens.size() && tokens[position].first == "OPERATOR" && tokens[position].second == "(")
            {
                match("OPERATOR");
                cppOutput += "    auto " + methodName + "(";

                // Manejar los parámetros
                bool firstParam = true;
                while (position < tokens.size() && !(tokens[position].first == "OPERATOR" && tokens[position].second == ")"))
                {
                    if (!firstParam)
                    {
                        cppOutput += ", ";
                    }
                    firstParam = false;

                    // Procesar el tipo del parámetro
                    if (position < tokens.size() && tokens[position].first.find("_") != std::string::npos)
                    {
                        std::string type = tokens[position].first;
                        match(type);

                        // Manejar referencias
                        bool isRef = false;
                        if (position < tokens.size() && tokens[position].first == "OPERATOR" && tokens[position].second == "&")
                        {
                            isRef = true;
                            match("OPERATOR");
                        }

                        // Obtener el nombre del parámetro
                        if (position < tokens.size() && tokens[position].first == "IDENTIFIER")
                        {
                            std::string paramName = tokens[position].second;
                            match("IDENTIFIER");

                            cppOutput += type + (isRef ? "& " : " ") + paramName;
                        }
                    }

                    // Manejar comas entre parámetros
                    if (position < tokens.size() && tokens[position].first == "OPERATOR" && tokens[position].second == ",")
                    {
                        match("OPERATOR");
                    }
                }

                match("OPERATOR"); // Cerrar paréntesis
                cppOutput += ") {\n";

                // Procesar cuerpo del método
                if (position < tokens.size() && tokens[position].first == "OPERATOR" && tokens[position].second == "{")
                {
                    match("OPERATOR");
                    parseBlock("        "); // Indentación adicional para métodos
                }

                cppOutput += "    }\n";
            }
        }
    }

    void parseStatement(const std::string &indent)
    {
        // Variables
        if (position < tokens.size() && (tokens[position].first == "INT" ||
                                         tokens[position].first == "FLOAT" ||
                                         tokens[position].first == "BOOL" ||
                                         tokens[position].first == "STRING" ||
                                         tokens[position].first == "ARRAY"))
        {
            std::string type = tokens[position].first;
            match(type);

            if (position < tokens.size() && tokens[position].first == "IDENTIFIER")
            {
                std::string varName = tokens[position].second;
                match("IDENTIFIER");

                cppOutput += indent + type + " " + varName;

                // Inicialización
                if (position < tokens.size() && tokens[position].first == "OPERATOR" && tokens[position].second == "=")
                {
                    match("OPERATOR");
                    cppOutput += " = ";

                    // Analizar la expresión de inicialización
                    parseExpression();
                }

                cppOutput += ";\n";

                // Punto y coma
                if (position < tokens.size() && tokens[position].first == "OPERATOR" && tokens[position].second == ";")
                {
                    match("OPERATOR");
                }
            }
        }
        // If-Else (INQUISITION-ELSE_DECREE)
        else if (position < tokens.size() && tokens[position].first == "IF")
        {
            match("IF");

            cppOutput += indent + "if (";

            // Condición
            if (position < tokens.size() && tokens[position].first == "OPERATOR" && tokens[position].second == "(")
            {
                match("OPERATOR");
                parseExpression();
                if (position < tokens.size() && tokens[position].first == "OPERATOR" && tokens[position].second == ")")
                {
                    match("OPERATOR");
                }
            }

            cppOutput += ") {\n";

            // Bloque Then
            if (position < tokens.size() && tokens[position].first == "OPERATOR" && tokens[position].second == "{")
            {
                match("OPERATOR");
                parseBlock(indent + "    ");
            }

            // Bloque Else (opcional)
            if (position < tokens.size() && tokens[position].first == "ELSE")
            {
                match("ELSE");
                cppOutput += indent + "} else {\n";

                if (position < tokens.size() && tokens[position].first == "OPERATOR" && tokens[position].second == "{")
                {
                    match("OPERATOR");
                    parseBlock(indent + "    ");
                }
            }

            if (!cppOutput.empty() && cppOutput.back() != '\n')
            {
                cppOutput += "\n";
            }
            cppOutput += indent + "}\n";
        }
        // Bucle For (CRUSADE)
        else if (position < tokens.size() && tokens[position].first == "FOR")
        {
            match("FOR");

            cppOutput += indent + "for (";

            // Parámetros del bucle
            if (position < tokens.size() && tokens[position].first == "OPERATOR" && tokens[position].second == "(")
            {
                match("OPERATOR");

                // Inicialización
                if (position < tokens.size() && tokens[position].first == "INT")
                {
                    match("INT");
                    if (position < tokens.size() && tokens[position].first == "IDENTIFIER")
                    {
                        std::string varName = tokens[position].second;
                        match("IDENTIFIER");

                        cppOutput += "int " + varName;

                        if (position < tokens.size() && tokens[position].first == "OPERATOR" && tokens[position].second == "=")
                        {
                            match("OPERATOR");
                            cppOutput += " = ";
                            parseExpression();
                        }
                    }
                }

                cppOutput += "; ";

                // Condición
                if (position < tokens.size() && tokens[position].first == "OPERATOR" && tokens[position].second == ";")
                {
                    match("OPERATOR");
                    parseExpression();
                }

                cppOutput += "; ";

                // Incremento
                if (position < tokens.size() && tokens[position].first == "OPERATOR" && tokens[position].second == ";")
                {
                    match("OPERATOR");
                    parseExpression();
                }

                if (position < tokens.size() && tokens[position].first == "OPERATOR" && tokens[position].second == ")")
                {
                    match("OPERATOR");
                }
            }

            cppOutput += ") {\n";

            // Bloque del bucle
            if (position < tokens.size() && tokens[position].first == "OPERATOR" && tokens[position].second == "{")
            {
                match("OPERATOR");
                parseBlock(indent + "    ");
            }

            cppOutput += indent + "}\n";
        }
        // Bucle While (SIEGE)
        else if (position < tokens.size() && tokens[position].first == "WHILE")
        {
            match("WHILE");

            cppOutput += indent + "while (";

            // Condición
            if (position < tokens.size() && tokens[position].first == "OPERATOR" && tokens[position].second == "(")
            {
                match("OPERATOR");
                parseExpression();
                if (position < tokens.size() && tokens[position].first == "OPERATOR" && tokens[position].second == ")")
                {
                    match("OPERATOR");
                }
            }

            cppOutput += ") {\n";

            // Bloque del bucle
            if (position < tokens.size() && tokens[position].first == "OPERATOR" && tokens[position].second == "{")
            {
                match("OPERATOR");
                parseBlock(indent + "    ");
            }

            cppOutput += indent + "}\n";
        }
        // Return (EXTERMINATUS)
        else if (position < tokens.size() && tokens[position].first == "RETURN")
        {
            match("RETURN");

            cppOutput += indent + "return ";
            parseExpression();
            cppOutput += ";\n";

            // Punto y coma
            if (position < tokens.size() && tokens[position].first == "OPERATOR" && tokens[position].second == ";")
            {
                match("OPERATOR");
            }
        }
        // Imprimir (TRANSMIT)
        else if (position < tokens.size() && tokens[position].first == "PRINT")
        {
            match("PRINT");

            cppOutput += indent + "std::cout << ";

            // Parámetros
            if (position < tokens.size() && tokens[position].first == "OPERATOR" && tokens[position].second == "(")
            {
                match("OPERATOR");
                parseExpression();
                if (position < tokens.size() && tokens[position].first == "OPERATOR" && tokens[position].second == ")")
                {
                    match("OPERATOR");
                }
            }

            cppOutput += " << std::endl;\n";

            // Punto y coma
            if (position < tokens.size() && tokens[position].first == "OPERATOR" && tokens[position].second == ";")
            {
                match("OPERATOR");
            }
        }
        // Llamada a función
        else if (position < tokens.size() && tokens[position].first == "IDENTIFIER")
        {
            std::string funcName = tokens[position].second;
            match("IDENTIFIER");

            // Verificar si es una llamada a función
            if (position < tokens.size() && tokens[position].first == "OPERATOR" && tokens[position].second == "(")
            {
                cppOutput += indent + funcName + "(";
                match("OPERATOR");

                // Parámetros
                bool firstParam = true;
                while (position < tokens.size() && !(tokens[position].first == "OPERATOR" && tokens[position].second == ")"))
                {
                    if (!firstParam)
                    {
                        cppOutput += ", ";
                    }
                    firstParam = false;

                    parseExpression();

                    // Coma entre parámetros
                    if (position < tokens.size() && tokens[position].first == "OPERATOR" && tokens[position].second == ",")
                    {
                        match("OPERATOR");
                    }
                }

                if (position < tokens.size() && tokens[position].first == "OPERATOR" && tokens[position].second == ")")
                {
                    match("OPERATOR");
                }

                cppOutput += ");\n";

                // Punto y coma
                if (position < tokens.size() && tokens[position].first == "OPERATOR" && tokens[position].second == ";")
                {
                    match("OPERATOR");
                }
            }
            // Asignación a variable
            else if (position < tokens.size() && tokens[position].first == "OPERATOR" && tokens[position].second == "=")
            {
                cppOutput += indent + funcName + " = ";
                match("OPERATOR");

                parseExpression();

                cppOutput += ";\n";

                // Punto y coma
                if (position < tokens.size() && tokens[position].first == "OPERATOR" && tokens[position].second == ";")
                {
                    match("OPERATOR");
                }
            }
        }
        else
        {
            position++; // Saltarse tokens no reconocidos
        }
    }

    void parseExpression()
    {
        // Implementación simple para expresiones
        // En un compilador real, esto sería mucho más complejo para manejar precedencia, etc.

        // Literales
        if (position < tokens.size())
        {
            if (tokens[position].first == "INT_LITERAL" || tokens[position].first == "FLOAT_LITERAL")
            {
                cppOutput += tokens[position].second;
                position++;
            }
            else if (tokens[position].first == "STRING_LITERAL")
            {
                cppOutput += tokens[position].second;
                position++;
            }
            else if (tokens[position].first == "IDENTIFIER")
            {
                cppOutput += tokens[position].second;
                position++;

                // Llamada a método o acceso a miembro
                while (position < tokens.size() && tokens[position].first == "OPERATOR" &&
                       (tokens[position].second == "." || tokens[position].second == "->"))
                {
                    cppOutput += tokens[position].second;
                    position++;

                    if (position < tokens.size() && tokens[position].first == "IDENTIFIER")
                    {
                        cppOutput += tokens[position].second;
                        position++;
                    }

                    // Llamada a método
                    if (position < tokens.size() && tokens[position].first == "OPERATOR" && tokens[position].second == "(")
                    {
                        cppOutput += "(";
                        position++;

                        // Parámetros
                        bool firstParam = true;
                        while (position < tokens.size() && !(tokens[position].first == "OPERATOR" && tokens[position].second == ")"))
                        {
                            if (!firstParam)
                            {
                                cppOutput += ", ";
                            }
                            firstParam = false;

                            parseExpression();

                            // Coma entre parámetros
                            if (position < tokens.size() && tokens[position].first == "OPERATOR" && tokens[position].second == ",")
                            {
                                position++;
                            }
                        }

                        if (position < tokens.size() && tokens[position].first == "OPERATOR" && tokens[position].second == ")")
                        {
                            cppOutput += ")";
                            position++;
                        }
                    }
                }
            }
            // Llamada a función
            else if (tokens[position].first == "RANDOM" || tokens[position].first == "INPUT")
            {
                std::string funcName = tokens[position].second;
                position++;

                cppOutput += funcName;

                if (position < tokens.size() && tokens[position].first == "OPERATOR" && tokens[position].second == "(")
                {
                    cppOutput += "(";
                    position++;

                    // Parámetros
                    bool firstParam = true;
                    while (position < tokens.size() && !(tokens[position].first == "OPERATOR" && tokens[position].second == ")"))
                    {
                        if (!firstParam)
                        {
                            cppOutput += ", ";
                        }
                        firstParam = false;

                        parseExpression();

                        // Coma entre parámetros
                        if (position < tokens.size() && tokens[position].first == "OPERATOR" && tokens[position].second == ",")
                        {
                            position++;
                        }
                    }

                    if (position < tokens.size() && tokens[position].first == "OPERATOR" && tokens[position].second == ")")
                    {
                        cppOutput += ")";
                        position++;
                    }
                }
            }
            // Operadores
            else if (tokens[position].first == "OPERATOR")
            {
                cppOutput += tokens[position].second;
                position++;

                // Para operadores binarios, parsear el segundo operando
                if (tokens[position - 1].second == "+" || tokens[position - 1].second == "-" ||
                    tokens[position - 1].second == "*" || tokens[position - 1].second == "/" ||
                    tokens[position - 1].second == "<" || tokens[position - 1].second == ">" ||
                    tokens[position - 1].second == "==" || tokens[position - 1].second == "!=" ||
                    tokens[position - 1].second == "<=" || tokens[position - 1].second == ">=" ||
                    tokens[position - 1].second == "&&" || tokens[position - 1].second == "||")
                {
                    parseExpression();
                }
            }
        }
    }

public:
    Parser(const std::vector<std::pair<std::string, std::string>> &tokens) : tokens(tokens), position(0) {}

    std::string parse()
    {
        parseProgram();
        return cppOutput;
    }
};

class Compiler
{
private:
    std::string sourcePath;
    std::string outputPath;

public:
    Compiler(const std::string &sourcePath, const std::string &outputPath)
        : sourcePath(sourcePath), outputPath(outputPath) {}

    bool compile()
    {
        try
        {
            // Verificar la extensión del archivo
            if (sourcePath.length() < 8 || sourcePath.substr(sourcePath.length() - 8) != ".emperor")
            {
                std::cerr << Color::RED << "Error: El archivo fuente debe tener la extensión '.emperor'" << std::endl;
                std::cerr << "Para la gloria del Omnissiah, usa la extensión correcta." << Color::RESET << std::endl;
                return false;
            }

            // Leer el archivo fuente
            std::ifstream sourceFile(sourcePath);
            if (!sourceFile.is_open())
            {
                std::cerr << Color::RED << "Error: No se pudo abrir el archivo fuente " << sourcePath << std::endl;
                std::cerr << "¡El Emperador protege los archivos válidos!" << Color::RESET << std::endl;
                return false;
            }

            std::string source((std::istreambuf_iterator<char>(sourceFile)),
                               std::istreambuf_iterator<char>());
            sourceFile.close();

            // Mostrar mensaje temático de inicio de compilación
            std::cout << Color::YELLOW << Color::BOLD;
            std::cout << "====================================" << std::endl;
            std::cout << "| COMPILADOR DEL ADEPTUS MECHANICUS |" << std::endl;
            std::cout << "| Bendiciones del Omnissiah         |" << std::endl;
            std::cout << "====================================" << Color::RESET << std::endl;

            // Mostrar arte ASCII animado del águila imperial
            displayAsciiArt();

            // Análisis léxico
            std::cout << Color::CYAN << ">> Iniciando análisis léxico";
            loadingAnimation("", 1500);
            Lexer lexer(source);
            lexer.tokenize();
            animateText(Color::GREEN + ">> Los Servocráneos han completado el análisis léxico." + Color::RESET, 15);

            // Análisis sintáctico y generación de código C++
            std::cout << Color::CYAN << ">> Iniciando traducción a binario sagrado";
            loadingAnimation("", 2000);
            Parser parser(lexer.getTokens());
            std::string cppCode = parser.parse();
            animateText(Color::GREEN + ">> Traducción completada. La máquina-espíritu está complacida." + Color::RESET, 15);

            // Escribir el código C++ generado
            std::ofstream outputFile(outputPath);
            if (!outputFile.is_open())
            {
                std::cerr << Color::RED << "Error: No se pudo abrir el archivo de salida " << outputPath << std::endl;
                std::cerr << "¡Herejía tecnológica detectada!" << Color::RESET << std::endl;
                return false;
            }

            outputFile << cppCode;
            outputFile.close();

            animateText(Color::GREEN + ">> Código C++ generado con éxito en " + outputPath + Color::RESET, 15);

            // Compilar el código C++ generado con g++
            std::string binaryPath = sourcePath.substr(0, sourcePath.find_last_of('.'));
            std::string compileCommand = "g++ -std=c++17 " + outputPath + " -o " + binaryPath;

            std::cout << Color::CYAN << ">> Invocando los ritos de compilación: " << compileCommand;
            loadingAnimation("", 2500);
            int result = std::system(compileCommand.c_str());

            if (result == 0)
            {
                std::cout << Color::GREEN << Color::BOLD;
                std::cout << "===============================================" << std::endl;
                std::cout << "| ¡COMPILACIÓN EXITOSA! ¡EL EMPERADOR PROTEGE! |" << std::endl;
                std::cout << "===============================================" << Color::RESET << std::endl;
                animateText(Color::YELLOW + "Ejecutable generado: " + binaryPath + Color::RESET, 20);

                // Mostrar mensaje de bendición final
                std::this_thread::sleep_for(std::chrono::milliseconds(300));
                animateText(Color::MAGENTA + "¡Que el Omnissiah bendiga este binario!" + Color::RESET, 30);
                return true;
            }
            else
            {
                std::cerr << Color::RED << Color::BOLD;
                std::cerr << "==============================================" << std::endl;
                std::cerr << "| ¡HEREJÍA DETECTADA EN EL CÓDIGO FUENTE!    |" << std::endl;
                std::cerr << "| Los Tecnosacerdotes no pudieron compilar.  |" << std::endl;
                std::cerr << "==============================================" << Color::RESET << std::endl;
                return false;
            }
        }
        catch (const std::exception &e)
        {
            std::cerr << Color::RED << "¡Error durante la compilación!" << std::endl;
            std::cerr << "Mensaje del Mechanicus: " << e.what() << std::endl;
            std::cerr << "¡Purga tu código de impurezas!" << Color::RESET << std::endl;
            return false;
        }
    }
};

int main(int argc, char *argv[])
{
    // Limpiar la pantalla
    std::system("clear");

    std::cout << Color::YELLOW << Color::BOLD;
    animateText("==============================================", 5);
    animateText("|       COMPILADOR IMPERIUMCODE v1.0.40K     |", 20);
    animateText("| Para la gloria del Emperador de la Tierra  |", 20);
    animateText("==============================================", 5);
    std::cout << Color::RESET;

    if (argc < 3)
    {
        std::cout << Color::CYAN << "Uso: ImperiumCodeCompiler <archivo_fuente.emperor> <archivo_salida.cpp>" << std::endl;
        std::cout << "Ejemplo: ImperiumCodeCompiler batalla.emperor batalla.cpp" << std::endl
                  << std::endl;
        std::cout << Color::RED << "El Adeptus Mechanicus requiere los archivos correctos para proceder." << Color::RESET << std::endl;
        return 1;
    }

    std::string sourcePath = argv[1];
    std::string outputPath = argv[2];

    Compiler compiler(sourcePath, outputPath);
    bool success = compiler.compile();

    if (!success)
    {
        std::cout << Color::RED << "El Emperador observa tu fracaso. Intenta de nuevo." << Color::RESET << std::endl;
    }

    return success ? 0 : 1;
}