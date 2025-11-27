🧩 Funcionalidades
------------------

### 🔽 **1\. Programação Orientada a Objetos (OOP em C)**

<details> <summary><strong>Clique para expandir</strong></summary>

Mesmo sem suporte nativo a OOP, o projeto usa:

-   **Structs como objetos**\
    `Jogador`, `Peca`, `Partida`, `Estatistica`

-   **Ponteiros como métodos**\
    `moverPeca(&partida)`\
    `salvarEstatisticas(&jogador)`

-   **Encapsulamento de estado**\
    Cada struct armazena e manipula seus próprios dados.

Estrutura fiel aos princípios de OOP aplicada em C.

</details>

* * * * *

### 🔽 **2\. Sistema de Login e Autenticação**

<details> <summary><strong>Clique para expandir</strong></summary>

-   Cadastro e login pelo console

-   Armazenamento em `login.txt` (`usuario:senha`)

-   **Senha oculta** com `*`

-   Dados individuais de cada conta são salvos e carregados automaticamente

</details>

* * * * *

### 🔽 **3\. Ranking e Estatísticas**

<details> <summary><strong>Clique para expandir</strong></summary>

Cada jogador possui:

-   Partidas jogadas

-   Vitórias

-   Empates

-   Derrotas

Formato salvo em `ranking.txt`:

`Thayse, 10, 6, 3, 1;`

Estatísticas atualizadas ao final de cada partida.

</details>

* * * * *

### 🔽 **4\. Modo Convidado (Sem Login)**

<details> <summary><strong>Clique para expandir</strong></summary>

Permite jogar sem criar conta:

-   Jogadores identificados como\
    `Convidado X`, `Convidado O`

-   Estatísticas também são registradas

-   Controladas separadamente dos jogadores logados

</details>

* * * * *

### 🔽 **5\. Salvar e Continuar Partidas**

<details> <summary><strong>Clique para expandir</strong></summary>

O sistema salva partidas não terminadas em:

`partida_salva.txt`

Salva:

-   estado completo do tabuleiro

-   turno atual

-   peças capturadas

-   variáveis internas da partida

Permite retomar exatamente de onde parou.

</details>

* * * * *

### 🔽 **6\. Visualização do Ranking**

<details> <summary><strong>Clique para expandir</strong></summary>

-   Lista todos os jogadores (logados e convidados)

-   Mostra partidas, vitórias, empates, derrotas e winrate

-   Interface limpa e colorida no console

</details>

* * * * *

### 🔽 **7\. Interface com Cores (ANSI Colors)**

<details> <summary><strong>Clique para expandir</strong></summary>

-   Peças coloridas

-   Destaque para erros e alertas

-   Indicação visual de xeque e xeque-mate

-   Melhor legibilidade e experiência no terminal

</details>

* * * * *

🛠️ Tecnologias Utilizadas
--------------------------

-   **C (C99/C11)**

-   **GCC/Clang**

-   **Structs e ponteiros**

-   **Arquivos de texto (.txt)**

-   **ANSI Escape Codes para cores**

* * * * *

📂 Arquivos do Sistema
----------------------

| Arquivo | Finalidade | Exemplo |
| --- | --- | --- |
| `login.txt` | Usuários cadastrados | `usuario:senha` |
| `ranking.txt` | Estatísticas | `Arthur, 15, 8, 4, 3;` |
| `partida_salva.txt` | Estado da partida | tabuleiro + turno |

* * * * *

▶️ Compilação e Execução
------------------------

### **Compilar**

`gcc Xadrez.c -o xadrez -lm`

### **Executar**

`./xadrez`

-   Se os arquivos não existirem, o sistema os cria automaticamente.
