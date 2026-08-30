# rubiks-cube-ai-solver

Para os colaboradores deste projeto, não é possível fazer push diretamente
na `main`. Crie uma branch de desenvolvimento e faça uma Pull Request para
a `main`.


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
| `Upper` | Superior | Branco | W |
| `Lower` | Inferior | Amarelo | Y |
| `Front` | Frontal | Verde | G |
| `Rear` | Traseira | Azul | B |
| `Right` | Direita | Vermelho | R |
| `Left` | Esquerda | Laranja | O |

Na linha central, as faces aparecem na ordem: esquerda, frontal, direita e traseira. A superior e a inferior ficam alinhadas com a frontal.

Os pares de cores opostas são branco–amarelo, verde–azul e vermelho–laranja. Essa associação descreve o estado resolvido: durante os movimentos, as cores mudam de posição, mas os nomes das faces permanecem fixos.'
