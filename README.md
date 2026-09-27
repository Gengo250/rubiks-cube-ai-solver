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
| `include/Heuristic.hpp`, `src/Heuristic.cpp` | Índice canônico do estado e a heurística admissível |
| `include/Solver.hpp`, `src/Solver.cpp` | Busca A* usando essa heurística |
| `include/Menu.hpp`, `src/Menu.cpp` | Janela Raylib, fonte, telas e desenho |
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
Escape, fechar a janela ou `Finish` encerra o simulador.

O menu tem três telas:

- **inicial** — `Solve by Yourself` ou `Solve with AI`;
- **`Solve by Yourself`** — exibir o cubo, embaralhar e girar as seis faces nos
  dois sentidos, como antes;
- **`Solve with AI`** — embaralhar e escolher a busca. `Depth-First Search` e
  `Breadth-First Search` aparecem na lista mas respondem que não estão
  implementadas; **`A* Search`** resolve de fato, e a solução é aplicada ao
  cubo um movimento por vez, com a notação, o número de nós expandidos e o
  tempo no painel da direita.

```sh
ctest --test-dir build --output-on-failure
```

Os testes não abrem janela. `cube_tests` verifica os sentidos das seis faces,
movimentos inversos, quatro giros, integridade das peças e uma sequência de mil
movimentos. `solver_tests` verifica a consistência da heurística, a otimalidade
do A* contra uma BFS e que aplicar a solução devolve o cubo resolvido.

## Solver

A busca usa o modelo descrito em [`docs/heuristica-a-estrela.md`](docs/heuristica-a-estrela.md):
a peça em `(0,0,0)` fica parada e só as camadas `layer = 1` giram, o que dá
nove movimentos (R, U, F nos dois sentidos e meia-volta) na métrica HTM.

Como `shuffle()` move essa peça, o solver primeiro gira o **cubo inteiro** até
ela voltar para casa. Essa reorientação não resolve nada e não conta como
movimento da solução; ela apenas coloca o modelo em vigor, e garante que o
cubo termine exatamente no estado inicial de `Cube()`.

A heurística é o máximo de quatro pattern databases — os grupos de peças
`{1,2,3}` e `{4,5,6,7}`, a permutação e a orientação —, 79 KiB construídos por
BFS reversa no primeiro uso. Ela é admissível e consistente, então o A*
devolve sempre a solução mais curta: para cubos embaralhados, 140 nós
expandidos em média e menos de 1 ms por busca.

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
