# Heurística para o A* no cubo 2×2×2

**Como ler este documento.**

| Parte | Para quem | O que tem |
|---|---|---|
| **I — A ideia** | quem quer *entender* | explicação do zero, com analogias, sem fórmula |
| **II — Os números** | quem quer *decidir* | tudo que foi medido, tabela por tabela |
| **III — A referência** | quem vai *reimplementar* | provas, índices, contrato de código |
| **IV — Apêndices** | consulta | limites, reprodução, glossário |

Se você só quer saber por que o solver é do jeito que é, a Parte I basta.

Estudo de 2026-09-22, remedido em 2026-09-23 e implementado depois em
`include/Heuristic.hpp` / `src/Heuristic.cpp`.

---

## Resumo em meia página

O A\* precisa de um **palpite** de quantos movimentos ainda faltam para
resolver o cubo. Um palpite ruim faz a busca visitar milhões de estados; um
palpite bom faz ela visitar centenas.

O palpite escolhido aqui é: *resolva versões mais fáceis do mesmo cubo,
guarde todas as respostas numa tabela e consulte a tabela*. Versão mais fácil =
"finja que só existem 3 das 7 peças". Essa versão tem tão poucos estados que dá
para resolver **todos** eles antes da busca começar, em milissegundos, e guardar
o resultado em alguns KiB.

Quatro tabelas dessas, e o palpite é o maior valor entre elas:

- custo: **79 KiB** de memória e **33 ms** de pré-cálculo, uma vez só;
- efeito: o A\* passa de **940.059** estados visitados em média para **331,5**;
- garantia: o palpite **nunca exagera**, então a solução devolvida é sempre a
  mais curta possível.

---

# Parte I — A ideia

## 1. Por que o A* precisa de um palpite

O A\* é busca de caminho mais curto com uma dica. Para cada estado do cubo que
ele considera, calcula

```
f = g + h
```

- `g` = quantos movimentos já foram feitos para chegar aqui. É um fato, contado.
- `h` = quantos movimentos ele **chuta** que ainda faltam daqui até o cubo
  resolvido. É o palpite — a *heurística*.
- `f` = estimativa do tamanho total da solução que passa por este estado.

O A\* sempre expande primeiro o estado com menor `f`. É o GPS: entre dois
caminhos possíveis, ele explora antes aquele cuja soma "estrada já percorrida +
distância em linha reta até o destino" for menor.

O extremo sem dica é `h = 0`: aí `f = g`, o A\* explora tudo em ordem de
profundidade e vira busca em largura. Funciona, mas visita **940 mil** estados
por cubo. O extremo oposto é `h` exato: a busca vai direto ao destino, **6**
estados. Toda a engenharia está em chegar o mais perto possível do exato sem
precisar da tabela exata inteira.

## 2. As duas regras que o palpite tem que obedecer

### Regra 1 — nunca exagerar (*admissibilidade*)

O palpite pode errar para baixo à vontade. Não pode errar para cima **nunca**.

Por quê: o A\* descarta um caminho quando o `f` dele fica pior que o de outro.
Se o palpite exagerar num caminho que era o melhor, o A\* o joga fora e devolve
uma solução mais longa. Um palpite que exagera não é um palpite fraco — é um
palpite **errado**, porque quebra a única coisa que o A\* promete: otimalidade.

> Analogia: se o GPS disser que a rota boa tem 40 km quando tem 10, ele vai te
> mandar pela rota ruim e jurar que era a melhor.

### Regra 2 — não dar saltos (*consistência*)

Andar um movimento pode mudar o palpite em no máximo 1.

Por quê: com essa propriedade, quando o A\* fecha um estado ele já tem a
distância definitiva até ali e nunca precisa reabri-lo. Sem ela, o A\* ainda
devolve a resposta certa, mas refaz trabalho, e a contagem de "estados
visitados" da Parte II deixaria de medir o trabalho real.

Consistência é mais forte que admissibilidade: toda heurística consistente é
automaticamente admissível.

### E a terceira, que não é regra, é o objetivo

**Quanto maior o palpite, melhor** — desde que respeite as duas regras. Um
palpite que diz "faltam 6" corta muito mais caminhos do que um que diz "faltam
2". Por isso a busca não é por um palpite *correto* (zero é correto), é pelo
maior palpite que ainda cabe embaixo da verdade.

## 3. Como o cubo vira dois números

Para consultar tabela, o estado do cubo precisa virar um número. Dois conceitos
bastam:

- **peça** — um dos 8 cantinhos de plástico. As peças nunca trocam de adesivo
  entre si; `tests/CubeTests.cpp` verifica isso.
- **slot** — um dos 8 lugares onde uma peça pode estar. O endereço do slot é
  `x·4 + y·2 + z`, então slot `0` é o canto `(0,0,0)` e slot `7` é `(1,1,1)`.

> Slots são cadeiras, peças são pessoas. Um giro é uma troca de cadeiras.

Com isso, o estado é descrito por duas informações:

**(a) Quem está em cada cadeira.** Sete slots móveis, sete peças → é uma
permutação, e toda permutação de 7 elementos tem um número entre `0` e `5039`
(código de Lehmer). Chame de `permIdx`.

**(b) Como cada uma está virada.** Em cada peça existe um adesivo marcado: o que
no cubo resolvido apontava para cima. Hoje ele aponta para um dos três eixos —
`0`, `1` ou `2`. Sete slots, três possibilidades cada → um número em base 3
entre `0` e `2186`. Chame de `oriIdx`.

Estado = o par `(permIdx, oriIdx)`, ou o número único `permIdx · 2187 + oriIdx`.

### O detalhe que faz isso funcionar

O rótulo de orientação fica pendurado **no slot, não na peça**: anotamos "a
peça que está nesta cadeira tem o adesivo marcado no eixo 2", sem registrar
quem é ela.

Parece um detalhe de contabilidade, mas é o que deixa as duas metades
independentes. Um giro faz duas coisas ao mesmo tempo: manda a cadeira `s` para
a cadeira `σ(s)`, e troca dois dos três rótulos de eixo das cadeiras que
giraram. **Nenhuma das duas depende de quem está sentado.** Consequência:

```
permIdx' = tabela_perm[permIdx][movimento]      // 5040 × 9
 oriIdx' = tabela_ori [oriIdx ][movimento]      // 2187 × 9
```

Aplicar um movimento vira duas leituras de tabela. É isso que torna viável
varrer milhões de estados em menos de um segundo.

## 4. As tentativas ingênuas, e por que quase não servem

A ideia natural é contar peças fora do lugar. Como cada giro mexe 4 das 7 peças
móveis, um giro conserta no máximo 4, então `⌈peças erradas / 4⌉` nunca exagera:
é admissível e consistente, e não custa memória nenhuma.

O problema é o teto. Com 7 peças erradas — o caso comum — o palpite dá
`⌈7/4⌉ = 2`. E a distância real chega a **11**. O palpite fica travado em 2
para quase todo cubo embaralhado; dizer "faltam pelo menos 2" quando faltam 9
não elimina caminho nenhum.

Medido: corta de 940 mil para 177 mil estados. Um fator 5 num espaço de 3,6
milhões — irrelevante. **Heurística sem memória não resolve este problema.**

## 5. A ideia central: resolver um cubo mais fácil

Aqui está o truque, e é o coração do documento.

> **Pinte 4 das 7 peças móveis de cinza.** Agora você só enxerga 3 peças. Esse
> "cubo cego" é muito menor: só existem **5.670** configurações possíveis dele.
> Tão poucas que dá para resolver **todas**, de uma vez, com uma busca em
> largura que leva 1,8 ms, e guardar a resposta numa tabela de 5,5 KiB.
>
> Quando o A\* precisar de um palpite para um cubo de verdade, ele apaga
> mentalmente as 4 peças cinzas, consulta a tabela e lê: *"este cubo cego precisa
> de 6 movimentos"*.

**Por que isso nunca exagera.** Pegue a solução ótima do cubo de verdade e
aplique-a olhando só para as 3 peças visíveis. As peças cinzas continuam se
movendo junto — você só parou de prestar atenção nelas. Então essa mesma
sequência também resolve o cubo cego. Ou seja: **existe** uma solução do cubo
cego com o comprimento da solução verdadeira. E a tabela guarda a *menor*
solução do cubo cego. Logo

```
tabela ≤ solução do cubo cego com os movimentos verdadeiros = solução verdadeira
```

O palpite nunca passa da verdade. Isso é uma **pattern database** (PDB), e a
demonstração formal está em §12.

**Por que é tão melhor que contar peças.** A PDB não estima: ela sabe a resposta
*exata* de um problema parecido. Onde contar peças trava em 2, a PDB de grupo
chega a 8 — 73% da distância real, na média.

### Quatro tabelas, e pega o maior

Se `h₁` e `h₂` nunca exageram, `max(h₁, h₂)` também não: o maior dos dois ainda
é menor que a verdade. E o maior é sempre um palpite pelo menos tão bom quanto
qualquer um dos dois. Então dá para empilhar tabelas de graça (de graça em
qualidade; memória cada uma custa).

As quatro implementadas:

| tabela | o que ela enxerga | o que ela apaga |
|---|---|---|
| grupo `{1,2,3}` | posição **e** giro das peças 1, 2, 3 | as outras quatro |
| grupo `{4,5,6,7}` | posição **e** giro das peças 4, 5, 6, 7 | as outras três |
| permutação | onde estão as **sete** peças | todos os giros |
| orientação | o giro das **sete** peças | quem é quem |

Note que as duas últimas são o mesmo truque na outra direção: em vez de apagar
peças, apagam *um aspecto* de todas as peças.

**E elas não são redundantes.** Era de se esperar que as tabelas de grupo, muito
maiores, soubessem tudo que as pequenas sabem. Não é o caso: em **3,53%** dos
estados a PDB de permutação dá um palpite *maior* que as de grupo. Faz sentido —
ela enxerga as sete peças de uma vez, enquanto cada grupo enxerga no máximo
quatro. Juntar as quatro custa 7 KiB a mais e nunca piora.

## 6. Por que pegar o maior e não somar

Somar parece melhor: "o grupo A precisa de 5 e o grupo B precisa de 6, então o
cubo precisa de 11". Está errado.

O motivo: **todo giro mexe 4 das 7 peças móveis**, então quase todo giro mexe
peças do grupo A *e* do grupo B ao mesmo tempo. Aquele único giro adianta os dois
grupos. Somando, você cobra o mesmo giro duas vezes.

Somar só seria legítimo se cada movimento pudesse ser cobrado a um grupo só — é a
condição das *PDBs aditivas*, e ela não vale aqui.

O argumento é conhecido, mas neste estudo ele foi **medido**: a soma das duas
PDBs de grupo exagera em **3.412.553 dos 3.674.160 estados (92,9%)**, chegando a
dizer 15 onde a distância real é 11. Um A\* com ela devolveria soluções que não
são as mais curtas. **A soma não é uma heurística fraca, é uma heurística
errada.**

## 7. O modelo: quais são as regras do jogo

Falta declarar sobre que cubo tudo isso vale, senão "nunca exagera" não quer
dizer nada.

- **Um canto fica parado.** A peça em `(0,0,0)` nunca se move. Girar o cubo
  inteiro não muda nada fisicamente, só renomeia as faces; fixar um canto
  elimina essas 24 renomeações e faz cada estado ter um nome só.
- **Nove movimentos.** Só a camada `layer = 1` de cada eixo — exatamente o
  Right, o Upper e o Front do README. Três eixos × (horário, anti-horário,
  meia-volta) = 9. Nenhuma mudança em `Cube::rotate` foi necessária.
- **Métrica HTM.** Quarto de volta e meia-volta custam 1 cada. Como `rotate()`
  gira 90°, a meia-volta são duas chamadas de custo total 1.
- **Objetivo.** O estado inicial de `Cube()`.

Esses 9 movimentos são **fechados por inversão**: o inverso de cada um está
entre os 9. Isso é usado duas vezes — para provar a consistência (§13) e para
poder construir as tabelas com uma busca *a partir do* objetivo em vez de *até*
ele (§14).

**E o `shuffle()`?** Ele usa as seis camadas, então move a peça que deveria
ficar parada. Por isso o solver primeiro gira o **cubo inteiro** até essa peça
voltar para casa (`heuristic::homingSpins`, no máximo 3 giros). Isso não resolve
nada e não conta como movimento da solução: só coloca o modelo de canto fixo em
vigor.

---

# Parte II — Os números

Tudo aqui foi medido, não estimado. Onde diz "todos os estados", são os
3.674.160 estados alcançáveis, um por um — não amostra.

## 8. O modelo confere com o cubo real

| Verificação | Resultado |
|---|---|
| Modelo × `Cube::rotate` real, 200 sequências de 30 movimentos | idêntico peça a peça |
| Canto `(0,0,0)` permanece fixo sob os 9 movimentos | confirmado |
| Soma dos twists `≡ 0 (mod 3)` | confirmado |
| Estados alcançados pela BFS completa | **3.674.160** de 11.022.480 |
| Profundidade máxima (número de Deus, HTM) | **11** |

Os 3.674.160 são exatamente `7! × 3⁶`. O índice bruto tem espaço para
11.022.480 = `7! × 3⁷`; o fator 3 que sobra é o invariante clássico de
orientação (a sétima peça é determinada pelas outras seis). Ele apareceu na
**medição**, não foi assumido.

A distribuição por profundidade reproduz a conhecida para o 2×2 em HTM, que é a
evidência mais forte de que a indexação está correta:

| d | estados | d | estados |
|---:|---:|---:|---:|
| 0 | 1 | 6 | 50.136 |
| 1 | 9 | 7 | 227.536 |
| 2 | 54 | 8 | 870.072 |
| 3 | 321 | 9 | 1.887.748 |
| 4 | 1.847 | 10 | 623.800 |
| 5 | 9.992 | 11 | 2.644 |

Note a forma: metade dos estados está a distância 9. Cubo embaralhado quase
sempre está longe, e nunca a mais de 11.

## 9. O que cada tabela custa

Um byte por entrada.

| tabela | entradas | alcançáveis | memória | pré-cálculo |
|---|---:|---:|---:|---:|
| PDB permutação | 5.040 | 5.040 | 4,9 KiB | 0,1 ms |
| PDB orientação | 2.187 | 729 | 2,1 KiB | < 0,1 ms |
| PDB grupo `{1,2,3}` | 5.670 | 5.670 | 5,5 KiB | 1,8 ms |
| PDB grupo `{4,5,6,7}` | 68.040 | 68.040 | 66,4 KiB | 31,3 ms |
| **as duas de grupo** | **73.710** | **73.710** | **72,0 KiB** | **33 ms** |
| **`max` das quatro** | **80.937** | — | **79,0 KiB** | **33 ms** |
| tabela exata `h*` | 3.674.160 | 3.674.160 | 3,5 MiB | 348 ms |

Duas observações:

- A PDB de orientação gasta 2.187 endereços para 729 estados reais — o
  invariante de twist sobrevive à projeção, porque ela mantém os sete slots. Um
  índice denso de 729 entradas existe e não vale o trabalho neste tamanho.
- Nas tabelas de grupo **todas** as entradas são alcançáveis: o invariante vale
  para os oito cantos juntos, não para um subconjunto.

## 10. Qualidade do palpite, sobre todos os estados

A coluna "exagera" checa `h > h*` estado a estado. `h/h* médio` é o quanto o
palpite chega perto da verdade — 1,000 seria perfeito.

| heurística | h máx | h médio | h/h* médio | exagera |
|---|---:|---:|---:|:--:|
| `h₀ = 0` | 0 | 0,000 | 0,000 | não |
| peças erradas / 4 | 2 | 1,995 | 0,231 | não |
| mal orientados / 4 | 2 | 1,556 | 0,180 | não |
| PDB permutação | 7 | 4,862 | 0,560 | não |
| PDB orientação | 6 | 4,436 | 0,512 | não |
| `max`(perm, orient) | 7 | 5,144 | 0,593 | não |
| **PDB grupos (`max`)** | **8** | **6,384** | **0,736** | **não** |
| **`max` das quatro** | **8** | **6,422** | **0,741** | **não** |
| soma das PDBs de grupo | 15 | 11,390 | 1,314 | **sim, 92,9%** |
| tabela exata `h*` | 11 | 8,756 | 1,000 | não |

A linha da soma é o `h máx = 15` contra distância real 11 de §6.

### As tabelas de grupo não dominam as pequenas

| relação | estados | fração |
|---|---:|---:|
| grupos `>` max(perm, orient) | 2.876.360 | 78,29% |
| grupos `=` max(perm, orient) | 668.210 | 18,19% |
| grupos `<` max(perm, orient) | **129.590** | **3,53%** |

É o 3,53% citado em §5: em um estado a cada 28, as tabelas pequenas sabem algo
que as grandes não sabem.

## 11. O que interessa: estados visitados pelo A*

Amostra de 220 estados, 20 por profundidade de 1 a 11, mesma implementação de
A\* e mesma amostra em todas as linhas. Todas as buscas devolveram solução
ótima, conferida contra `h*`.

| heurística | estados visitados (média) | ganho sobre `h₀` |
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

- **Sem memória, quase nada acontece.** Fator 2 a 5 num espaço de 3,6 milhões.
  O teto de `h = 2` de §4 é o culpado.
- **O salto é pagar memória.** 2 KiB de PDB de orientação já valem 36×; 72 KiB
  de PDBs de grupo valem 2.193×.
- **Posição e giro são quase independentes.** Cada uma sozinha alcança ~0,53 da
  distância real; o `max` das duas alcança 0,59 e **triplica** o corte. Um ganho
  desproporcional ao aumento do palpite, e o sinal disso é que elas erram em
  estados diferentes — que é exatamente quando `max` rende.
- **O último `max` é barato.** Acrescentar as duas tabelas pequenas às de grupo
  custa 7 KiB e corta mais 23% dos estados. Melhor negócio por byte da tabela, e
  também o menor salto: quem preferir um mecanismo só fica nas de grupo.

## 12. Recomendação

Usar `h = max` das **quatro** tabelas: grupo `{1,2,3}`, grupo `{4,5,6,7}`,
permutação e orientação.

- 79 KiB, 33 ms de pré-cálculo no arranque;
- 331,5 estados visitados em média, contra 940.059 sem palpite;
- nunca exagera e nunca salta — provado em §13-§14 e verificado estado a estado
  nos 3.674.160;
- é uma pattern database de verdade, e a técnica transfere para o 3×3, onde a
  tabela exata é impossível. É o ponto pedagógico do exercício.

Alternativa de um mecanismo só: apenas as duas PDBs de grupo — 72 KiB, 428,7
estados. A diferença entre as duas opções é pequena diante da diferença para
qualquer coisa sem memória.

A tabela exata `h*` fica **só como oráculo de teste**, conferindo que o A\*
devolve o mesmo comprimento que a BFS. Usá-la como heurística de produção
tornaria a busca trivial (6,0 estados) e esvaziaria o exercício.

---

# Parte III — A referência formal

Esta parte define o objeto com precisão suficiente para reimplementá-lo. Tudo
aqui foi explicado informalmente na Parte I.

## 13. A abstração e o lema do homomorfismo

Um estado é o par de funções sobre os sete slots móveis

```
peça  : slot → peça          rótulo : slot → {0,1,2}
```

Escolha um conjunto de peças `G ⊆ {1,…,7}` e defina a projeção

```
φ_G(s) = ( (posição de p, rótulo de p) : p ∈ G )
```

isto é, `φ_G` apaga tudo que não seja posição e orientação das peças de `G`. As
tabelas de permutação e de orientação são casos degenerados da mesma ideia: a
primeira apaga todos os rótulos e guarda as sete posições; a segunda apaga as
peças e guarda os sete rótulos por slot.

**Lema.** Para todo movimento `m` existe `m_G` tal que `φ_G(m(s)) = m_G(φ_G(s))`.

*Prova.* Por §3, `m` leva o slot `s` para `σ_m(s)` e substitui o rótulo `ℓ` do
conteúdo de `s` por `τ_m(ℓ)` quando `s` está na camada girada, e por `ℓ` caso
contrário; `τ = (a b)` com `a = (eixo+1) mod 3` e `b = (eixo+2) mod 3`. Ambos
dependem apenas de `s`, nunca de qual peça ocupa `s`. Logo a nova posição e o
novo rótulo de cada `p ∈ G` são função apenas da posição e do rótulo antigos de
`p`. Isso define `m_G`. ∎

Com `d_G` a distância ao objetivo no espaço abstrato, sob os mesmos 9 movimentos
e o mesmo custo 1, defina `h_G(s) = d_G(φ_G(s))`.

### Admissibilidade

Seja `s = s₀ → s₁ → … → s_d` uma solução ótima, `d = h*(s)`. Aplicando `φ_G` a
cada estado e usando o lema, `φ_G(s₀) → … → φ_G(s_d)` é um caminho legítimo no
espaço abstrato, de comprimento `d`, terminando no objetivo abstrato. Como `d_G`
é a distância mínima, `h_G(s) = d_G(φ_G(s)) ≤ d = h*(s)`. ∎

### Consistência

Sejam `s` e `s' = m(s)` vizinhos. Pelo lema, `φ_G(s')` é vizinho de `φ_G(s)` no
espaço abstrato, e o movimento inverso `m⁻¹` também está no conjunto de 9. Então
`d_G(φ_G(s)) ≤ d_G(φ_G(s')) + 1`, ou seja `h_G(s) ≤ 1 + h_G(s')`. Como o custo
de uma aresta é 1, isso é exatamente a desigualdade triangular. ∎

### `max` preserva as duas, `soma` não

Se `h₁` e `h₂` são admissíveis, `max(h₁,h₂) ≤ h*` porque cada uma o é. Se são
consistentes, `hᵢ(s) ≤ 1 + hᵢ(s')` para `i = 1,2`, logo o máximo satisfaz a
mesma desigualdade.

A soma exigiria PDBs aditivas — cada movimento cobrado a no máximo um grupo. Aqui
todo giro HTM move 4 dos 7 slots e portanto toca os dois grupos. Consequência
medida em §6 e §10.

### As heurísticas sem memória

`⌈c/4⌉`, com `c` = slots errados entre os 7 móveis, é admissível porque a camada
girada nunca contém o slot `0` (as três coordenadas nulas), então cada movimento
troca o conteúdo de exatamente 4 dos 7 slots móveis e corrige no máximo 4. Como
`c` varia em no máximo 4 por movimento, `⌈c/4⌉` varia em no máximo 1, e portanto
também é consistente. O mesmo vale contando só orientação.

### Nota sobre quiralidade

A orientação crua **não** é o "twist" clássico: um giro age sobre ela como
transposição dos rótulos de slot, não como soma módulo 3. Só existe soma
conservada quando a ordem cíclica dos três eixos respeita a quiralidade do
vértice — o sinal de `sx·sy·sz`, positivo quando `x+y+z` é ímpar. Com essa
ordem, a soma dos oito twists é `≡ 0 (mod 3)` em todos os testes.

Ressalva honesta: **esse teste não distingue a mão escolhida.** Inverter a
quiralidade nega cada twist e a soma continua `≡ 0`; as duas convenções passam.
O que realmente valida a codificação é a verificação de §8 contra o `rotate()`
real.

## 14. Índice e contrato de implementação

Para um grupo de `k` peças, o estado abstrato é `k` posições distintas entre 7
slots mais `k` rótulos em base 3:

```
idx = posIdx · 3ᵏ + Σ rótuloᵢ · 3ⁱ ,   posIdx ∈ [0, 7·6·…·(8−k) )
```

com `posIdx` obtido por ranqueamento da injeção: para cada peça, a posição do
seu slot na lista dos slots ainda livres. O índice é denso — `k = 3` dá 5.670
entradas, `k = 4` dá 68.040 — e a medição confirma que todas são alcançáveis.

Contrato:

```
build()            // BFS reversa em cada abstração, uma vez, no arranque
h(estado) -> int   // max sobre as tabelas; 0 no objetivo; nunca > h*
```

Cada tabela é `uint8_t` por entrada, preenchida por uma BFS **a partir do**
objetivo abstrato usando os mesmos 9 movimentos. Isso é legítimo porque o
conjunto é fechado por inversão, então distância a partir do objetivo é igual a
distância até o objetivo.

## 15. O que está no repositório

`include/Heuristic.hpp` e `src/Heuristic.cpp` implementam o índice canônico e a
heurística; `include/Solver.hpp` e `src/Solver.cpp`, o A\* que a consome. Duas
notas de quem implementou:

- **A reorientação deixou de ser pendência.** `Cube::shuffle()` move a peça
  fixa, então `heuristic::homingSpins` acha, por BFS sobre as 24 orientações, os
  giros do cubo inteiro que a trazem de volta — no máximo 3. Como a peça 0 volta
  à posição *e* à orientação originais, o cubo termina exatamente no estado de
  `Cube()`, não a menos de rotação.
- **Custo real de uso.** Sobre 300 cubos de `shuffle()`: 140,0 estados visitados
  em média, máximo 1.114, 0,34 ms por busca incluindo as tabelas. Fica abaixo
  dos 193,8 de §16 porque `shuffle()` aplica 20 giros aleatórios e não amostra o
  espaço uniformemente — produz 34% de estados em `d = 9` onde o uniforme daria
  51%.

`tests/SolverTests.cpp` verifica consistência (`|Δh| ≤ 1` em 20.000 movimentos),
otimalidade contra BFS até profundidade 5, e que aplicar a solução a 60 cubos
embaralhados devolve o cubo resolvido.

---

# Parte IV — Apêndices

## 16. Limites e ressalvas

- **O tamanho do problema é o limite pedagógico.** Com 3,6 milhões de estados, a
  tabela exata cabe em 3,5 MiB e resolve tudo. O valor deste trabalho está na
  comparação entre heurísticas, não em desempenho absoluto — para o qual busca
  bidirecional ou tabela exata seriam a resposta.
- **`h` só está definida com o canto `(0,0,0)` em casa.** Quem for reimplementar
  precisa normalizar o estado por rotação do cubo inteiro antes de indexar (§7,
  §15). O estudo não precisou disso, pois gera estados pelos 9 movimentos, que
  cobrem exatamente o espaço.
- **HTM, não QTM.** Em QTM seriam 6 movimentos e profundidade máxima 14; os
  números acima não se transferem.
- **A amostra do A\* superestima o custo.** 20 estados por profundidade dão peso
  1/11 à profundidade 11, que no espaço real é 0,07% dos estados e é justamente a
  mais cara. Medindo o `max` das quatro tabelas por profundidade — 1,0 em `d=1`,
  34,2 em `d=8`, 190,1 em `d=9`, 500,9 em `d=10`, 2.886,1 em `d=11` — e
  reponderando pela distribuição de §8, a média cai de 331,5 para **193,8**. A
  cauda cara domina a média estratificada. A comparação entre heurísticas
  continua válida porque todas pagam o mesmo viés.
- **A escolha dos grupos não foi otimizada.** `{1,2,3}` e `{4,5,6,7}` é a
  partição óbvia; nenhuma outra foi medida.
- **A soma anterior deste documento, "66 KB", era o tamanho de apenas uma** das
  duas tabelas de grupo. O par custa 72 KiB.

## 17. Como os números foram produzidos

A medição foi feita por um harness descartável em C++, linkado contra
`librubiks_cube_core` e sem abrir janela — mesmo esquema de `cube_tests` — de
modo que valida o `rotate()` real e não uma reimplementação. Ele não faz parte do
repositório.

Roteiro: construir as tabelas de movimento, verificar o modelo contra `Cube`,
rodar a BFS completa, gerar as PDBs, varrer os 3.674.160 estados e rodar o A\*
com cada heurística. A varredura leva menos de 1 s; a bateria de A\* leva ~73 s,
dominada pelas 220 buscas com `h₀`.

## 18. Glossário

| Termo | Significado aqui |
|---|---|
| **peça** | um dos 8 cantinhos de plástico; carrega seus 3 adesivos para sempre |
| **slot** | um dos 8 lugares onde uma peça pode estar, endereçado por `x·4+y·2+z` |
| **`g`** | movimentos já feitos até o estado atual — fato |
| **`h`** | palpite de movimentos que ainda faltam — a heurística |
| **`h*`** | a verdade: distância real até o cubo resolvido |
| **admissível** | `h ≤ h*` sempre; o palpite nunca exagera |
| **consistente** | `h` muda no máximo 1 por movimento; implica admissível |
| **abstração / projeção** | versão simplificada do cubo, obtida apagando informação |
| **PDB** | *pattern database*: tabela com a distância exata de **todos** os estados de uma abstração |
| **aditiva** | PDBs que podem ser somadas sem exagerar; **não** é o caso aqui |
| **HTM** | *half-turn metric*: quarto e meia-volta custam 1 cada |
| **número de Deus** | maior distância existente; 11 neste modelo |
| **permutação / Lehmer** | quem está em cada slot, codificado em `0..5039` |
| **orientação / rótulo** | para qual eixo aponta o adesivo marcado, por slot, em base 3 |
