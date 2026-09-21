# rubiks-cube-ai-solver

Para os colaboradores deste projeto, não é possível fazer push diretamente
na `main`. Crie uma branch de desenvolvimento e faça uma Pull Request para
a `main`.


## Estrutura

O estado do cubo é um conjunto de **oito peças**. Cada `Cubie` guarda:

- `position`: coordenadas inteiras `[x, y, z]`, sempre `0` ou `1`;
- `colors`: três adesivos, um voltado para fora em cada eixo, usando `Color` da Raylib.

Cada peça permanece no mesmo elemento do array. Um giro altera sua posição
e a orientação dos seus adesivos, sem transferir cores entre peças.
O lado para o qual cada adesivo aponta é determinado pela coordenada naquele eixo.

`Cube::rotate(axis, layer, clockwise)` gira as quatro peças de uma camada.
Os eixos são `0 = X`, `1 = Y`, `2 = Z`; o sentido horário é visto de fora
da face que está girando. Por exemplo, `cube.rotate(2, 1, true)` gira a frente
no sentido horário. `shuffle()` aplica vinte giros aleatórios.

| Arquivo | Responsabilidade |
|---|---|
| `include/Cube.hpp`, `src/Cube.cpp` | Peças, giros e embaralhamento |
| `include/Menu.hpp`, `src/Menu.cpp` | Janela Raylib, fonte, comandos e desenho |
| `src/main.cpp` | Inicia o menu |

`include/Menu.hpp` é a interface pública do módulo de menu. Para utilizá-lo,
inclua `"Menu.hpp"`, crie um `Menu` e chame `run()`. A implementação fica em
`src/Menu.cpp`, compilado pelo CMake; a Raylib é usada pelo próprio módulo.

A interface lê as peças diretamente. A planificação existe apenas no desenho:
não há matriz de faces, snapshot intermediário, enums de cores ou checagem
de resolução. O núcleo usa o tipo `Color` da Raylib e não abre uma janela.

## Compilar e executar

Requisitos: compilador C++20, CMake 3.20 ou superior e Raylib 6.0 instalada.

```sh
cmake -S . -B build
cmake --build build
./build/rubiks_cube_ai_solver
```

Use setas ou W/S para navegar; Enter, espaço ou clique para confirmar.
As opções permitem exibir o cubo, embaralhar e girar as seis faces nos dois sentidos.
Escape, fechar a janela ou `Finish` encerra o simulador.

```sh
ctest --test-dir build --output-on-failure
```

Os testes não abrem janela: verificam os sentidos das seis faces, movimentos
inversos, quatro giros, integridade das peças e uma sequência de mil movimentos.

## Representação das faces

O cubo 2×2×2 é exibido de forma planificada. Cada face possui quatro adesivos, organizados em duas linhas e duas colunas.

```text
        W W
        W W

O O     G G     R R     B B
O O     G G     R R     B B

        Y Y
        Y Y
```

| Face | Posição | Cor no estado resolvido | Símbolo |
|---|---|---|---|
| `Upper` | Y = 1 | Branco | W |
| `Lower` | Y = 0 | Amarelo | Y |
| `Front` | Z = 1 | Verde | G |
| `Rear` | Z = 0 | Azul | B |
| `Right` | X = 1 | Vermelho | R |
| `Left` | X = 0 | Laranja | O |

Na linha central, as faces aparecem na ordem: esquerda, frontal, direita e traseira. A superior e a inferior ficam alinhadas com a frontal.

Os pares de cores opostas são branco–amarelo, verde–azul e vermelho–laranja.
Essa associação descreve o estado inicial: durante os movimentos, as peças
viajam com seus adesivos, enquanto os nomes das faces permanecem fixos.
