
O projeto utiliza um modelo físico baseado em **espaço vetorial tridimensional**, no qual cada peça do cubo é representada individualmente dentro de um sistema de coordenadas `X`, `Y` e `Z`.

Essa abordagem substitui a representação tradicional baseada em **seis matrizes 2D independentes**, permitindo que a movimentação, identificação das peças e verificação do estado do cubo sejam realizadas diretamente sobre sua estrutura espacial.



 1. Evolução da Arquitetura

Arquitetura Legada — Orientada a Faces 2D

Na primeira versão do projeto, o estado do cubo era representado de maneira predominantemente gráfica.

Cada face possuía uma matriz independente, identificada por:

* `Upper`
* `Lower`
* `Front`
* `Rear`
* `Right`
* `Left`

Para realizar uma rotação, o motor precisava manipular simultaneamente diferentes matrizes e transportar as cores entre as bordas das faces.


Para rotacionar uma camada, era necessário **costurar as bordas de quatro matrizes diferentes**, utilizando diversas variáveis temporárias.

Esse processo aumentava a quantidade de operações e tornava a implementação das rotações mais complexa.

Problema estrutural

Uma única peça física do cubo, como uma peça de quina, encontrava-se **fragmentada em três posições de memória diferentes**, uma em cada face correspondente.

Isso gerava alguns problemas:

* Maior risco de sobrescrita acidental;
* Maior complexidade nas rotações;
* Maior dificuldade para manter a consistência do estado;
* Maior complexidade na identificação das peças;
* Cálculo de heurísticas menos eficiente;
* Necessidade de cruzar informações entre diferentes matrizes.

---

 2. Arquitetura Atual — Espaço Vetorial 3D

A arquitetura atual abandona a ideia de representar o cubo como **seis paredes independentes**.

O cubo passa a ser tratado como um **objeto físico único**, composto por:

> **8 blocos independentes (`Cubies`)**

Cada `Cubie` representa uma peça física real do Cubo Mágico 2x2 e possui sua própria posição e orientação dentro do espaço tridimensional.

Dessa maneira, a movimentação deixa de ser uma simples alteração de cores em matrizes e passa a representar uma **transformação geométrica das próprias peças**.

### Estrutura conceitual

```text
                    Y
                    ↑
                    │
              ┌─────┼─────┐
             /│     │    /│
            / │     │   / │
           ┌──┼─────┼──┐  │
           │  │     │  │  │
           │  └─────┼──┼──┘ → X
           │ /      │  │ /
           │/       │  │/
           └────────┼──┘
                    │
                    ↓
                    Z
```

O espaço tridimensional é formado por uma malha binária, pois cada eixo possui somente duas posições possíveis.

---

 Espaço Vetorial e Identidade das Peças

O cubo utiliza um sistema estrito de coordenadas cartesianas:

* `X` → largura
* `Y` → altura
* `Z` → profundidade

Como o objeto possui apenas duas posições em cada dimensão, cada coordenada pode assumir somente os valores `0` ou `1`.

### Sistema de coordenadas

| Eixo                 | `0`             | `1`             |
| -------------------- | --------------- | --------------- |
| **X — Largura**      | Camada Esquerda | Camada Direita  |
| **Y — Altura**       | Camada Inferior | Camada Superior |
| **Z — Profundidade** | Camada Traseira | Camada Frontal  |

Assim, as oito peças podem ser identificadas diretamente pelas combinações:

```text
(0,0,0)  (1,0,0)
(0,1,0)  (1,1,0)

(0,0,1)  (1,0,1)
(0,1,1)  (1,1,1)
```

Cada combinação representa uma única posição espacial possível para um `Cubie`.

---

3.1 Anatomia de um Cubie

Cada peça é representada por uma estrutura semelhante a:

```cpp
struct Cubie {
    int x;
    int y;
    int z;

    Color colorX;
    Color colorY;
    Color colorZ;
};
```

O `Cubie` mantém permanentemente:

* Sua posição `(x, y, z)`;
* A cor associada ao eixo `X`;
* A cor associada ao eixo `Y`;
* A cor associada ao eixo `Z`.

A identidade da peça não depende da face em que ela está localizada.

Em vez disso, sua identidade é determinada pela combinação entre **posição espacial e orientação de suas cores**.

### Exemplo

Considere a peça:

```text
(x, y, z) = (0, 0, 1)
```

Ela representa a quina:

```text
Inferior + Esquerda + Frontal
```

Inicialmente, ela pode possuir:

```text
colorX = Laranja
colorY = Amarelo
colorZ = Verde
```

Durante uma rotação, o `Cubie` inteiro é deslocado dentro do espaço.

As cores permanecem associadas à peça e acompanham sua transformação geométrica.

Isso elimina a necessidade de reconstruir manualmente uma peça a partir de diferentes faces.

---

 4. Lógica de Movimentação

A movimentação do cubo também foi modificada.

Na arquitetura anterior, uma rotação consistia principalmente em copiar e substituir valores entre matrizes.

Na arquitetura atual, uma rotação representa uma **transformação geométrica sobre um conjunto específico de Cubies**.

O processo pode ser resumido em:

```text
Estado atual
     │
     ▼
Selecionar camada
     │
     ▼
Selecionar 4 Cubies
     │
     ▼
Transformar coordenadas
     │
     ▼
Reorientar cores
     │
     ▼
Novo estado
```

---

 5. Redução do Espaço de Movimentos

Um Cubo Mágico 2x2 possui seis faces e, considerando os dois sentidos de rotação, teríamos:

```text
6 faces × 2 sentidos = 12 movimentos
```

Entretanto, o projeto utiliza uma representação espacial que permite **fixar uma orientação do cubo**, reduzindo o conjunto de movimentos necessários para a busca.

A métrica adotada fixa o vértice **inferior-traseiro-esquerdo** no espaço.

Com isso, são utilizados apenas seis movimentos:

| Movimento | Descrição               |
| --------- | ----------------------- |
| `U`       | Upper Clockwise         |
| `U'`      | Upper Counter-Clockwise |
| `R`       | Right Clockwise         |
| `R'`      | Right Counter-Clockwise |
| `F`       | Front Clockwise         |
| `F'`      | Front Counter-Clockwise |

Portanto:

```text
12 movimentos
      ↓
Fixação da orientação espacial
      ↓
6 movimentos
```

Essa redução é especialmente importante para a Inteligência Artificial, pois diminui o **fator de ramificação da árvore de busca**.



 6. Mecânica Matemática da Rotação

As rotações são implementadas através de transformações geométricas sobre as coordenadas.

Considere o movimento:

```text
R
```

que representa a rotação da camada direita.

## 6.1 Filtro espacial

Primeiramente, o algoritmo percorre os oito Cubies e seleciona somente aqueles pertencentes à camada:

```text
x == 1
```

Portanto:

```text
             x = 0       x = 1
               │           │
               ▼           ▼
             ignorar     rotacionar
```

Exatamente quatro peças satisfazem essa condição.

As outras quatro permanecem inalteradas.

---

## 6.2 Órbita 2D

Como o eixo `X` representa o eixo de rotação, sua coordenada permanece constante:

```text
x' = x
```

As coordenadas `Y` e `Z` são transformadas.

Como existem somente duas posições possíveis em cada eixo, `0` e `1`, a transformação pode ser realizada utilizando a inversão:

```text
1 - coordenada
```

Assim, para uma determinada direção de rotação:

```text
y' = 1 - z
z' = y
```

ou, para o sentido contrário:

```text
y' = z
z' = 1 - y
```

O resultado é uma órbita discreta das quatro peças ao redor do eixo `X`.

---

## 6.3 Tombamento das cores

A alteração de posição não afeta somente as coordenadas.

As cores também precisam acompanhar a orientação física da peça.

Como a rotação ocorre ao redor do eixo `X`, a cor associada ao eixo de rotação permanece:

```text
colorX
```

Enquanto as cores associadas aos eixos que sofreram transformação são trocadas.

Conceitualmente:

```text
colorX → permanece
colorY ↔ colorZ
```

Essa operação pode ser realizada diretamente através de:

```cpp
std::swap(cubie.colorY, cubie.colorZ);
```

Dessa maneira, **posição e orientação são atualizadas simultaneamente**.

---

 7. Verificação do Estado Resolvido

Uma das vantagens da arquitetura espacial é que o estado resolvido não precisa necessariamente ser comparado com uma matriz ou estado-gabarito previamente armazenado.

A verificação utiliza o conceito de:

> **Unanimidade de projeção direcional**

Cada eixo representa duas extremidades do cubo:

```text
X = 0 → Left
X = 1 → Right

Y = 0 → Lower
Y = 1 → Upper

Z = 0 → Rear
Z = 1 → Front
```

---

## 7.1 Exemplo — Face Direita

Para verificar se a face direita está resolvida:

### 1. Selecionar os Cubies

São selecionadas todas as peças que possuem:

```text
x == 1
```

Existem quatro Cubies nessa camada.

### 2. Verificar a projeção

Em seguida, o sistema verifica o valor:

```text
colorX
```

de cada uma das quatro peças.

Se:

```text
colorX₁ == colorX₂
colorX₂ == colorX₃
colorX₃ == colorX₄
```

então existe unanimidade na projeção daquela camada.

Por exemplo:

```text
┌─────────┐
│ Vermelho│
│ Vermelho│
│ Vermelho│
│ Vermelho│
└─────────┘
```

A camada está estruturalmente consistente.

---

8. Condição Final de Vitória

A solução completa ocorre quando a unanimidade é satisfeita simultaneamente nas seis extremidades do espaço.

O sistema verifica:

```text
X = 0
X = 1

Y = 0
Y = 1

Z = 0
Z = 1
```

Ou seja:

```text
       X
      / \
     /   \
   X=0   X=1

       Y
      / \
     /   \
   Y=0   Y=1

       Z
      / \
     /   \
   Z=0   Z=1
```

Cada camada deve possuir uma única cor projetada para sua respectiva direção.

O estado pode ser representado conceitualmente como:

```text
X = 0 → mesma colorX
X = 1 → mesma colorX

Y = 0 → mesma colorY
Y = 1 → mesma colorY

Z = 0 → mesma colorZ
Z = 1 → mesma colorZ
```

Quando todas as seis condições são verdadeiras:

```text
            ┌───────────────┐
            │ Cubo Resolvido│
            └───────┬───────┘
                    ✓
       ┌────────────┼────────────┐
       ▼            ▼            ▼
      Eixo X       Eixo Y       Eixo Z
       ✓ ✓          ✓ ✓          ✓ ✓
```

o motor considera o cubo solucionado.

Essa abordagem elimina a necessidade de realizar cruzamentos complexos entre diferentes matrizes de faces.
