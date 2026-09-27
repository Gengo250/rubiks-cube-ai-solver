# Heurística para A* no cubo 2×2×2

Estudo de 2026-09-22, revisto e remedido em 2026-09-23. Nenhum código do
simulador foi alterado: o objetivo é definir, com números medidos, qual
heurística vale a pena implementar depois.

## 1. Modelo

Antes de qualquer heurística é preciso declarar o modelo, senão "admissível"
não quer dizer nada.

- **Canto fixo.** A peça em `(0,0,0)` nunca se move. Isso elimina as 24
  rotações do cubo inteiro, que apenas renomeiam o mesmo estado físico.
- **Movimentos.** Só a camada `layer = 1` dos três eixos — exatamente Right
  (X=1), Upper (Y=1) e Front (Z=1) do README. Nenhuma mudança em
  `Cube::rotate` é necessária.
- **Métrica.** HTM: quarto de volta e meia-volta custam 1. São **9
  movimentos** por nó. Como `rotate()` gira 90°, a meia-volta é duas chamadas
  de custo total 1.
- **Objetivo.** Estado inicial de `Cube()`, a menos de rotação do cubo inteiro.

O conjunto de movimentos é fechado por inversão: o inverso de cada um dos 9
está entre os 9. Isso será usado nas provas.

O simulador não tem hoje comparação de estado resolvido — foi removida de
propósito em 2026-09-21. O solver a reintroduz como **índice canônico**, não
como método novo em `Cube`.

## 2. Representação e índice canônico

Tudo é derivado do que já existe, sem tocar no núcleo:

- **Identidade da peça.** `cubies[i]` guarda sempre a peça `i`; os adesivos
  nunca trocam de peça, e `tests/CubeTests.cpp` já verifica isso. A peça `i`
  nasce no slot `i`, com `slot = x·4 + y·2 + z`.
- **Permutação.** `perm[slot]` = peça naquele slot. Sete slots móveis →
  código de Lehmer em `0..5039`.
- **Orientação crua.** Em `rotate`, `std::swap(colors[a], colors[b])` mantém
  intacto o slot do eixo de giro. Então basta guardar **em qual slot (0/1/2)
  vive hoje o adesivo que no estado resolvido estava em `colors[1]`**. Sete
  slots → `0..2186` em base 3.

Índice do estado: `permIdx · 2187 + oriIdx`, num espaço bruto de 11.022.480.

**O rótulo é por slot, não por peça.** Essa escolha não é cosmética: é o que
torna as duas componentes independentes. Um giro leva o slot `s` para `σ(s)` e
aplica ao conteúdo dos slots da camada a transposição de rótulos
`τ = (a b)`, com `a = (eixo+1) mod 3` e `b = (eixo+2) mod 3`. Nem `σ` nem `τ`
dependem de *qual* peça está ali. Logo

```
permIdx' = permMove[permIdx][m]        oriIdx' = oriMove[oriIdx][m]
```

com duas tabelas de `5040 × 9` e `2187 × 9`, e uma transição de estado que é
uma indireção dupla. Toda a §5 depende dessa propriedade.

### O detalhe da quiralidade

A orientação crua **não** é o "twist" clássico: um giro age sobre ela como
*transposição* dos rótulos de slot, não como soma módulo 3. Só existe soma
conservada quando a ordem cíclica dos três eixos respeita a quiralidade do
vértice — o sinal de `sx·sy·sz`, positivo quando `x+y+z` é ímpar. Com a ordem
cíclica escolhida por esse sinal, a soma dos oito twists é `≡ 0 (mod 3)` em
todos os testes.

Ressalva honesta: **esse teste não distingue a mão escolhida**. Inverter a
quiralidade nega cada twist, e a soma continua `≡ 0`. As duas convenções
passam. O que realmente valida a codificação é a seção seguinte.

## 3. Verificação do modelo

| Verificação | Resultado |
|---|---|
| Modelo × `Cube::rotate` real, 200 sequências de 30 movimentos | idêntico peça a peça |
| Canto `(0,0,0)` permanece fixo sob os 9 movimentos | confirmado |
| Soma dos twists `≡ 0 (mod 3)` (ambas as quiralidades) | confirmado |
| Estados alcançados pela BFS completa | **3.674.160** de 11.022.480 |
| Profundidade máxima (número de Deus, HTM) | **11** |

Os 3.674.160 são `7! × 3⁶` — o fator 3 que falta em relação ao espaço bruto é
exatamente o invariante de twist, **medido e não assumido**. A distribuição
reproduz a conhecida para o 2×2 em HTM, o que é a evidência mais forte de que
a indexação é bijetora:

| d | estados | d | estados |
|---:|---:|---:|---:|
| 0 | 1 | 6 | 50.136 |
| 1 | 9 | 7 | 227.536 |
| 2 | 54 | 8 | 870.072 |
| 3 | 321 | 9 | 1.887.748 |
| 4 | 1.847 | 10 | 623.800 |
| 5 | 9.992 | 11 | 2.644 |

## 4. Heurísticas candidatas

Seja `c` a contagem de slots errados entre os 7 móveis.

1. **`h₀ = 0`** — linha de base; A* degenera em Dijkstra.
2. **Peças erradas / 4** — `h = ⌈c/4⌉`, com slot errado = peça errada **ou**
   orientação errada.
3. **Mal orientados / 4** — mesma forma, contando só orientação.
4. **PDB de permutação** — distância exata ignorando orientação (5.040).
5. **PDB de orientação** — distância exata ignorando permutação (2.187).
6. **`max` das duas PDBs acima.**
7. **PDB de grupos** — posição *e* orientação exatas de `{1,2,3}` e de
   `{4,5,6,7}`, tomando o **máximo**.
8. **`max` das quatro tabelas** — 7 junto com 6.
9. **Tabela exata `h*`** — BFS reversa completa; é o teto, não uma candidata.

## 5. A heurística, formalmente

As candidatas 4 a 8 são todas o mesmo objeto com parâmetros diferentes, e é
esse objeto que precisa estar definido para alguém implementar.

### 5.1 Abstração

Um estado é o par de funções

```
peça  : slot → peça          rótulo : slot → {0,1,2}
```

sobre os sete slots móveis. Escolha um conjunto de peças `G ⊆ {1,…,7}` e
defina a projeção

```
φ_G(s) = ( (posição de p, rótulo de p) : p ∈ G )
```

isto é, `φ_G` apaga tudo que não seja a posição e a orientação das peças de
`G`. As demais candidatas são casos degenerados da mesma ideia: a PDB de
permutação apaga todos os rótulos e guarda as sete posições; a PDB de
orientação apaga as peças e guarda os sete rótulos por slot.

**Lema (homomorfismo).** Para todo movimento `m` existe `m_G` tal que
`φ_G(m(s)) = m_G(φ_G(s))`.

*Prova.* Pela §2, `m` leva o slot `s` para `σ_m(s)` e substitui o rótulo `ℓ`
do conteúdo de `s` por `τ_m(ℓ)` quando `s` está na camada girada, e por `ℓ`
caso contrário. Ambos dependem apenas de `s`, nunca de qual peça ocupa `s`.
Logo a nova posição e o novo rótulo de cada `p ∈ G` são função apenas da
posição e do rótulo antigos de `p`. Isso define `m_G`. ∎

Denote por `d_G` a distância ao objetivo no espaço abstrato, sob os mesmos 9
movimentos e o mesmo custo 1, e defina

```
h_G(s) = d_G(φ_G(s))
```

### 5.2 Admissibilidade

Seja `s = s₀ → s₁ → … → s_d` uma solução ótima, `d = h*(s)`. Aplicando `φ_G`
a cada estado e usando o lema, `φ_G(s₀) → φ_G(s₁) → … → φ_G(s_d)` é um caminho
legítimo no espaço abstrato, de comprimento `d`, terminando no objetivo
abstrato. Como `d_G` é a distância mínima, `h_G(s) = d_G(φ_G(s)) ≤ d = h*(s)`.
∎

### 5.3 Consistência

Sejam `s` e `s' = m(s)` vizinhos. Pelo lema, `φ_G(s')` é vizinho de `φ_G(s)`
no espaço abstrato, e o movimento inverso `m⁻¹` também está no conjunto de 9.
Então `d_G(φ_G(s)) ≤ d_G(φ_G(s')) + 1`, ou seja `h_G(s) ≤ 1 + h_G(s')`. Como
o custo de uma aresta é 1, isso é exatamente a desigualdade triangular. ∎

Consistência importa na prática: com ela o A* nunca reabre um nó fechado, e a
contagem de nós expandidos da §6 é a contagem real de trabalho.

### 5.4 Máximo, e por que não soma

Se `h₁` e `h₂` são admissíveis, `max(h₁,h₂) ≤ h*` porque cada uma o é. Se são
consistentes, `hᵢ(s) ≤ 1 + hᵢ(s')` para `i = 1,2`, logo o máximo das duas
satisfaz a mesma desigualdade. Então `max` preserva as duas propriedades.

`soma` não preserva. PDBs aditivas exigem que cada movimento seja cobrado a no
máximo um grupo; aqui todo giro HTM move 4 dos 7 slots e portanto toca peças
dos dois grupos ao mesmo tempo. O argumento é conhecido, mas neste estudo ele
foi **medido**: somar as duas PDBs de grupos superestima `h*` em
**3.412.553 dos 3.674.160 estados (92,9%)**, com valor máximo 15 contra
distância real 11. A soma não é uma heurística ruim, é uma heurística errada.

### 5.5 Índice e contrato

Para um grupo de `k` peças, o estado abstrato é `k` posições distintas entre
7 slots mais `k` rótulos em base 3:

```
idx = posIdx · 3ᵏ + Σ rótuloᵢ · 3ⁱ ,   posIdx ∈ [0, 7·6·…·(8−k) )
```

com `posIdx` obtido por ranqueamento da injeção (a cada peça, a posição do seu
slot na lista dos slots ainda livres). Isso é denso: `k = 3` dá 5.670 entradas
e `k = 4` dá 68.040, e a medição confirma que **todas** são alcançáveis — a
restrição de twist vale para os oito cantos, não para um subconjunto próprio.

O contrato para quem escrever o A* é:

```
build()            // BFS reversa em cada abstração, uma vez, no arranque
h(estado) -> int   // max sobre as tabelas; 0 no objetivo; nunca > h*
```

Cada tabela é `uint8_t` por entrada, preenchida por uma BFS a partir do
objetivo abstrato usando os mesmos 9 movimentos — legítimo porque o conjunto é
fechado por inversão, então distância a partir do objetivo é igual a distância
até o objetivo.

### 5.6 As heurísticas sem memória

`⌈c/4⌉` é admissível porque cada movimento troca o conteúdo de exatamente 4
dos 7 slots móveis — a camada girada nunca contém o slot `0`, que tem as três
coordenadas nulas. Logo um movimento corrige no máximo 4 slots e
`⌈c/4⌉ ≤ d`. Como `c` varia em no máximo 4 por movimento, `⌈c/4⌉` varia em no
máximo 1, então também é consistente. O mesmo vale contando só orientação.

## 6. Resultados medidos

### 6.1 Custo das tabelas

Um byte por entrada. A soma anterior deste documento, "66 KB", era o tamanho
de **apenas uma** das duas tabelas de grupo; o par custa 72 KiB.

| tabela | entradas | alcançáveis | memória | pré-cálculo |
|---|---:|---:|---:|---:|
| PDB permutação | 5.040 | 5.040 | 4,9 KiB | 0,1 ms |
| PDB orientação | 2.187 | 729 | 2,1 KiB | < 0,1 ms |
| PDB grupo `{1,2,3}` | 5.670 | 5.670 | 5,5 KiB | 1,8 ms |
| PDB grupo `{4,5,6,7}` | 68.040 | 68.040 | 66,4 KiB | 31,3 ms |
| **PDB de grupos (as duas)** | **73.710** | **73.710** | **72,0 KiB** | **33 ms** |
| **`max` das quatro tabelas** | **80.937** | — | **79,0 KiB** | **33 ms** |
| tabela exata `h*` | 3.674.160 | 3.674.160 | 3,5 MiB | 348 ms |

A PDB de orientação usa 2.187 posições de índice para 729 estados reais: o
invariante de twist sobrevive à projeção, porque ela mantém os sete slots. Um
índice denso de 729 entradas existe, e não vale o trabalho a esse tamanho. A
tabela exata ocupa 3,5 MiB com índice denso sobre os estados alcançáveis; com
o índice bruto `permIdx · 2187 + oriIdx`, 10,5 MiB.

### 6.2 Qualidade, sobre todos os 3.674.160 estados

Não é amostra. A coluna "viola" checa `h > h*` estado a estado.

| heurística | h máx | h médio | h/h* médio | viola |
|---|---:|---:|---:|:--:|
| `h₀ = 0` | 0 | 0,000 | 0,000 | não |
| peças erradas / 4 | 2 | 1,995 | 0,231 | não |
| mal orientados / 4 | 2 | 1,556 | 0,180 | não |
| PDB permutação | 7 | 4,862 | 0,560 | não |
| PDB orientação | 6 | 4,436 | 0,512 | não |
| `max`(perm, orient) | 7 | 5,144 | 0,593 | não |
| **PDB grupos (`max`)** | **8** | **6,384** | **0,736** | **não** |
| **`max` das quatro** | **8** | **6,422** | **0,741** | **não** |
| soma das PDBs de grupos | 15 | 11,390 | 1,314 | **sim, 92,9%** |
| tabela exata `h*` | 11 | 8,756 | 1,000 | não |

### 6.3 Dominância

A PDB de grupos **não domina** `max`(perm, orient), ao contrário do que o
tamanho da tabela sugere:

| relação | estados | fração |
|---|---:|---:|
| grupos `>` max(perm, orient) | 2.876.360 | 78,29% |
| grupos `=` max(perm, orient) | 668.210 | 18,19% |
| grupos `<` max(perm, orient) | **129.590** | **3,53%** |

Em 3,53% dos estados as tabelas pequenas sabem algo que as de grupo não sabem.
A PDB de permutação enxerga as sete peças de uma vez; cada grupo enxerga no
máximo quatro. Como `max` de consistentes é consistente, combinar as quatro
custa 7 KiB e nunca piora.

### 6.4 Nós expandidos pelo A*

Amostra de 220 estados, 20 por profundidade de 1 a 11, mesma implementação de
A* e mesma amostra em todas as linhas. Todas as buscas devolveram solução
ótima, conferida contra `h*`.

| heurística | nós expandidos (média) | ganho sobre `h₀` |
|---|---:|---:|
| `h₀ = 0` | 940.059,2 | 1× |
| mal orientados / 4 | 410.590,8 | 2,3× |
| peças erradas / 4 | 176.791,3 | 5,3× |
| PDB orientação | 25.979,0 | 36× |
| PDB permutação | 8.106,6 | 116× |
| `max`(perm, orient) | 2.728,7 | 345× |
| PDB grupos (`max`) | 428,7 | 2.193× |
| **`max` das quatro tabelas** | **331,5** | **2.836×** |
| tabela exata `h*` | 6,0 | 156.677× |

Quatro leituras que os números impõem:

- **As heurísticas sem memória são quase inúteis.** Saturam em `h = 2` porque
  7 peças erradas dão `⌈7/4⌉ = 2`, enquanto a distância real chega a 11. Elas
  cortam um fator de 2 a 5 num espaço de 3,6 milhões — irrelevante.
- **O salto está em pagar memória.** 2 KiB de PDB de orientação já valem 36×;
  72 KiB de PDB de grupos valem 2.193×.
- **Orientação e permutação são quase independentes.** Cada uma sozinha dá
  ~0,53 de `h*`; o `max` das duas dá 0,59 e triplica o corte — sinal de que
  elas erram em estados diferentes, que é justamente o que faz `max` render.
- **O último `max` é barato e pequeno.** Somar as duas tabelas pequenas às de
  grupo custa 7 KiB e 23% menos nós. É o melhor negócio por byte da tabela,
  mas é também o menor salto: quem quiser um só mecanismo fica nas de grupo.

## 7. Recomendação

Implementar `h = max` das **quatro** tabelas: PDB do grupo `{1,2,3}`, PDB do
grupo `{4,5,6,7}`, PDB de permutação e PDB de orientação.

- 79 KiB no total, 33 ms de pré-cálculo por BFS reversa no arranque;
- 331,5 nós expandidos em média, contra 940.059 sem heurística;
- admissível e consistente pelas provas da §5, e verificada estado a estado
  nos 3.674.160 alcançáveis;
- o núcleo é uma pattern database de verdade — a técnica transfere para o 3×3,
  onde a tabela exata é impossível.

Se a preferência for um mecanismo único e não uma coleção, use só a PDB de
grupos: 72 KiB, 428,7 nós. A diferença entre as duas opções é pequena diante
da diferença para qualquer coisa sem memória.

Manter a tabela exata `h*` **apenas como oráculo de teste**, verificando que o
A* devolve o mesmo comprimento que a BFS. Usá-la como heurística de produção
tornaria o A* trivial (6,0 nós) e esvaziaria o exercício.

## 8. Limites

- **O tamanho do problema é o limite pedagógico.** Com 3,6 milhões de estados,
  a tabela exata cabe em 3,5 MiB e resolve tudo. O valor deste trabalho está na
  comparação entre heurísticas, não em desempenho absoluto — para o qual busca
  bidirecional ou tabela exata seriam a resposta.
- **`h` só está definida com o canto `(0,0,0)` em casa.** `Cube::shuffle()` usa
  as seis camadas e portanto move esse canto. Quem for implementar precisa
  normalizar o estado por rotação do cubo inteiro antes de indexar. O estudo
  não precisou disso, pois gera estados pelos 9 movimentos, que cobrem
  exatamente o espaço.
- **HTM, não QTM.** Em QTM seriam 6 movimentos e profundidade máxima 14; os
  números acima não se transferem.
- **A amostra do A* é estratificada, não uniforme, e isso *superestima* o
  custo.** 20 estados por profundidade dão peso 1/11 à profundidade 11, que no
  espaço real é 0,07% dos estados e é justamente a mais cara. Medindo
  `max` das quatro tabelas por profundidade — 1,0 nó em `d=1`, 34,2 em `d=8`,
  190,1 em `d=9`, 500,9 em `d=10` e 2.886,1 em `d=11` — e reponderando pela
  distribuição real da §3, a média cai de 331,5 para **193,8 nós**. A cauda
  cara domina a média estratificada. A comparação entre heurísticas continua
  válida porque todas pagam o mesmo viés.
- **A escolha dos grupos não foi otimizada.** `{1,2,3}` e `{4,5,6,7}` é a
  partição óbvia; nenhuma outra foi medida.

## 9. Implementação

A heurística recomendada está em `include/Heuristic.hpp` e `src/Heuristic.cpp`;
o A* que a consome está em `include/Solver.hpp` e `src/Solver.cpp`. Duas notas
de quem implementou:

- **A reorientação deixou de ser pendência.** `Cube::shuffle()` move a peça
  fixa, então `heuristic::homingSpins` acha, por BFS sobre as 24 orientações,
  os giros do cubo inteiro que a trazem de volta — no máximo 3. Isso não
  resolve nada: apenas põe o modelo de canto fixo em vigor. Como a peça 0
  volta à posição *e* à orientação originais, o cubo termina exatamente no
  estado de `Cube()`, não a menos de rotação.
- **Custo real de uso.** Sobre 300 cubos de `shuffle()`: 140,0 nós expandidos
  em média, máximo 1.114, 0,34 ms por busca incluindo as tabelas. Fica abaixo
  dos 193,8 porque `shuffle()` aplica 20 giros aleatórios e não amostra o
  espaço uniformemente — produz 34% de estados em `d=9` onde o uniforme daria
  51%.

`tests/SolverTests.cpp` verifica consistência (`|Δh| ≤ 1` em 20.000
movimentos), otimalidade contra BFS até profundidade 5, e que aplicar a
solução a 60 cubos embaralhados devolve o cubo resolvido.

## 10. Reprodução

A medição foi feita por um harness descartável em C++, linkado contra
`librubiks_cube_core` e sem abrir janela — mesmo esquema de `cube_tests` —,
de modo que valida o `rotate()` real e não uma reimplementação. Ele não faz
parte do repositório. Roteiro: construir as tabelas de movimento, verificar o
modelo contra `Cube`, rodar a BFS completa, gerar as PDBs, varrer os
3.674.160 estados e rodar o A* com cada heurística. A varredura leva menos de
1 s; a bateria de A* leva ~73 s, dominada pelas 220 buscas com `h₀`.
