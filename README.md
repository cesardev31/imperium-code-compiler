# ImperiumCodeCompiler

¡Por la gloria del Emperador! Este es un compilador temático inspirado en Warhammer 40K, diseñado para traducir un lenguaje personalizado (inspirado en el Imperio) a C++ y ejecutarlo.

## Características
- Arte ASCII y banners temáticos del Imperio y el Adeptus Mechanicus
- Reconoce comandos como `HONOR_THE_EMPEROR` y `TRANSMIT`
- Traduce archivos `.emperor` a C++ y ejecutables
- Mensajes y errores con estilo 40K

## Uso

1. Escribe tu código en un archivo `.emperor`:

```plaintext
HONOR_THE_EMPEROR {
    TRANSMIT("¡Por el Emperador y por Ultramar!");
    TRANSMIT("¡La disciplina y el deber son nuestro escudo!");
}
```

2. Compila el compilador:
```bash
g++ -std=c++17 main.cpp -o ImperiumCodeCompiler
```

3. Usa el compilador para traducir tu archivo `.emperor`:
```bash
./ImperiumCodeCompiler batalla.emperor batalla.cpp
```

4. Compila y ejecuta el código generado:
```bash
g++ -std=c++17 batalla.cpp -o batalla
./batalla
```

## Créditos
- Inspirado en Warhammer 40K
- Arte ASCII y mensajes personalizados para fans del Imperio

---
¡El Emperador protege!
