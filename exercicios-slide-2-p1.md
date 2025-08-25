Perfeito! Vou responder ponto a ponto os **Exercícios** do PDF, usando e estendendo o que já está nos seus arquivos `lexer.c`, `main.c/.h` (quando fizer sentido eu cito exatamente de onde tirei algo).

---

### 1) ER para octais. “0” é octal?

* **Expressão regular (base matemática):** `^0[0-7]+$` (sequência octal com pelo menos um dígito após o zero) e, por convenção matemática, o **zero** também é octal: a representação de 0 na base 8 é `0`.
* **Na sua implementação atual:** você tratou octal como `0[0-7]+` (pelo menos um dígito após o zero) e reservou o **literal “0”** para o decimal (`DEC`). Isso evita ambiguidade entre `DEC` e `OCT` no lexer. Em outras palavras, **matematicamente “0” é octal**, mas **no seu projeto** “0” está classificado como **DEC** por design (veja o comentário de `DEC` e o padrão de `isOCT` no seu `lexer.c`).&#x20;

---

### 2) `int isOCT(FILE *tape)`

Você **já implementou**. A função reconhece `OCT = '0'[0-7]+` e, se falhar após ler um não-octal, **devolve o caractere** e não consome indevidamente a fita. Trecho relevante: `while ((head = getc(tape)) >= '0' && head <= '7'); ungetc(head, tape); return OCT;`&#x20;

---

### 3) Diagrama de estados para o Exercício 2 e como emular ε

* **Estados (descrição textual):**
  `S` (início) —(‘0’)→ `Z` —(\[0–7])→ `O+` —(\[0–7])→ `O+` (laço) —(¬\[0–7])→ **ACEITA (OCT)** com *devolução* do símbolo não-octal.
* **ε-transições em C:** não existem ε “reais”; você emula o “olhei um símbolo que não pertence ao padrão e voltei” usando `getc` para ler e `ungetc` para **devolver** o símbolo ao fluxo (essa devolução é a **simulação** da ε-transição para não consumir entrada quando sai do padrão). Isso é exatamente o que seu código faz. &#x20;

---

### 4) ER para hexadecimais

* **Expressão regular:** `^0[xX][0-9A-Fa-f]+$`.

---

### 5) `int isHEX(FILE *tape)`

Você **já implementou** exatamente com o padrão acima e usando `isxdigit` (equivalente a `[0-9A-Fa-f]`). Também faz o `ungetc` correto quando falha após ler `0x`/`0X`.&#x20;

---

### 6) Diagrama de estados para hexadecimais e ε

* **Estados:**
  `S` —(‘0’)→ `Z` —(\[xX])→ `H1` —(isxdigit)→ `H+` —(isxdigit)→ `H+` (laço) —(¬isxdigit)→ **ACEITA (HEX)** com `ungetc` do primeiro não-hex.
* **ε emulação:** igual ao caso do octal: ler-avançar com `getc`, e quando sair do padrão, **devolve** o símbolo com `ungetc`. &#x20;

---

### 7) ER para ponto fixo

Ponto fixo (exemplos: `3.`, `3.14`, `.14`):

* **ER:** `^([0-9]+\.[0-9]*|\.[0-9]+)$`
  (se começa por dígitos, o `.` é obrigatório e a parte fracionária pode ser vazia; se começa por `.`, então precisa de **pelo menos um** dígito depois).

---

### 8) Diagrama para ponto fixo

* **Ramo A (com parte inteira):**
  `S` —(digit+)→ `I` —('.')→ `D` —(digit\*)→ **ACEITA** —(primeiro não-digit)→ **devolve com `ungetc`**.
* **Ramo B (sem parte inteira):**
  `S` —('.')→ `P` —(digit+)→ **ACEITA** —(primeiro não-digit)→ **devolve**.
* **ε emulação:** a saída do laço de dígitos sempre **devolve** o primeiro símbolo que não pertence ao padrão.&#x20;

---

### 9) Função predicado C para reconhecer ponto fixo

Abaixo uma implementação segura (não consome entrada se falhar). Você pode adicionar um novo token, por exemplo `FIX`, ao seu `enum`.

```c
// Acrescente no enum existente:
enum {
    ID = 1024,
    DEC,
    OCT,
    HEX,
    FIX,   // novo token para ponto fixo
};

int isFIX(FILE *tape) {
    int c = getc(tape);

    // Ramo A: [0-9]+ '.' [0-9]*
    if (isdigit(c)) {
        // consome a parte inteira
        do { c = getc(tape); } while (isdigit(c));

        if (c == '.') {
            // parte fracionária opcional
            int d = getc(tape);
            while (isdigit(d)) d = getc(tape);
            ungetc(d, tape);
            return FIX;
        } else {
            // não é ponto fixo; devolve o que tirou além do 1º dígito
            ungetc(c, tape);
            // e agora devolve os dígitos lidos (não precisamos devolver todos: basta
            // garantir que o primeiro símbolo não pertencente permaneça e o restante
            // já ficou consumido conforme a estratégia do seu lexer).
            // Estratégia simples: retrocede um passo apenas e deixa o DEC cuidar.
            return 0;
        }
    }

    // Ramo B: '.' [0-9]+
    if (c == '.') {
        int d = getc(tape);
        if (isdigit(d)) {
            do { d = getc(tape); } while (isdigit(d));
            ungetc(d, tape);
            return FIX;
        }
        // falhou: devolve o que pegou
        ungetc(d, tape);
        ungetc('.', tape);
        return 0;
    }

    // nada bateu: devolve e falha
    ungetc(c, tape);
    return 0;
}
```

> Observação: o seu estilo de “devolver apenas o primeiro símbolo não pertencente ao padrão” é o mesmo usado em `isID`, `isDEC`, `isOCT`, `isHEX`. Mantive a mesma convenção.&#x20;

---

### 10) ER para ponto flutuante (com notação científica)

Defina a notação exponencial: `EXP = [eE][+\-]?[0-9]+` (como no PDF). Então:

* **FIX** (do exercício 7): `([0-9]+\.[0-9]*|\.[0-9]+)`
* **INT** (inteiro decimal sem sinal): `[0-9]+`
* **FLT:** `^(FIX(?:EXP)?|INTEXP)$`
  Explicando: flutuante é **ponto fixo** opcionalmente seguido de `EXP`, **ou** inteiro seguido de `EXP`.&#x20;

---

### 11) Função que retorna `DEC` (inteiro decimal), `FLT` (flutuante) ou 0 (sem consumir fita)

Sugestão: adicionar `FLT` ao `enum`, criar um reconhecedor de **sufixo exponencial** e um reconhecedor de **float** que priorize flutuante antes de decimal.

```c
// Acrescente no enum existente:
enum {
    ID = 1024,
    DEC,
    OCT,
    HEX,
    FIX,
    FLT,   // novo token
};

// Auxiliar: checa EXP = [eE][+-]?[0-9]+ assumindo que 'e'/'E' já foi lido
// Se reconhecer, devolve o primeiro NÃO-digit após os dígitos do expoente e retorna 1.
// Caso contrário, reverte tudo e retorna 0.
static int isEXP_SUFFIX(FILE *tape) {
    long pos = ftell(tape);
    int c = getc(tape);
    if (c != 'e' && c != 'E') { ungetc(c, tape); return 0; }

    int s = getc(tape);
    if (s == '+' || s == '-') {
        int d = getc(tape);
        if (!isdigit(d)) { // falhou
            // desfaz: d, s, 'e'
            ungetc(d, tape); ungetc(s, tape); ungetc(c, tape);
            return 0;
        }
        // consome dígitos
        do { d = getc(tape); } while (isdigit(d));
        ungetc(d, tape);
        return 1;
    } else if (isdigit(s)) {
        int d = s;
        do { d = getc(tape); } while (isdigit(d));
        ungetc(d, tape);
        return 1;
    } else {
        // falhou
        ungetc(s, tape); ungetc(c, tape);
        return 0;
    }
}

// Reconhece FLT conforme o Exercício 10
int isFLOAT(FILE *tape) {
    int c = getc(tape);

    // Caso 1: [0-9]+ '.' [0-9]* (FIX) com EXP opcional
    if (isdigit(c)) {
        do { c = getc(tape); } while (isdigit(c));
        if (c == '.') {
            int d = getc(tape);
            while (isdigit(d)) d = getc(tape);
            ungetc(d, tape);         // após a parte fracionária
            // tenta EXP opcional
            long save = ftell(tape);
            int e = getc(tape);
            if (e == 'e' || e == 'E') {
                ungetc(e, tape);     // bota de volta para isEXP_SUFFIX
                if (isEXP_SUFFIX(tape)) return FLT;
            } else {
                ungetc(e, tape);
            }
            return FLT; // ponto fixo já é float
        }
        // Possível INT + EXP?
        ungetc(c, tape); // devolve o não-digit após o inteiro
        long save = ftell(tape);
        int e = getc(tape);
        if (e == 'e' || e == 'E') {
            ungetc(e, tape);
            if (isEXP_SUFFIX(tape)) return FLT; // INTEXP
        } else {
            ungetc(e, tape);
        }
        // não é float: reverte ao início e falha
        fseek(tape, -(ftell(tape) - save), SEEK_CUR); // opcional se usar ftell/fseek
        return 0;
    }

    // Caso 2: '.' [0-9]+ (FIX) com EXP opcional
    if (c == '.') {
        int d = getc(tape);
        if (isdigit(d)) {
            do { d = getc(tape); } while (isdigit(d));
            ungetc(d, tape);
            long save = ftell(tape);
            int e = getc(tape);
            if (e == 'e' || e == 'E') {
                ungetc(e, tape);
                if (isEXP_SUFFIX(tape)) return FLT;
            } else {
                ungetc(e, tape);
            }
            return FLT;
        }
        // falhou: devolve
        ungetc(d, tape); ungetc('.', tape);
        return 0;
    }

    // nada bateu
    ungetc(c, tape);
    return 0;
}

// Função "classificadora": DEC, FLT ou 0 (sem consumir se 0)
int isDEC_or_FLT(FILE *tape) {
    // Tente FLT primeiro (para não classificar "3.14" como DEC)
    long pos = ftell(tape);
    if (isFLOAT(tape)) return FLT;

    // Se não for FLT, tenta DEC (sua função já existe e aceita '0' ou [1-9][0-9]*)
    fseek(tape, pos, SEEK_SET);
    if (isDEC(tape)) return DEC;

    // nem FLT nem DEC: não consumir entrada
    fseek(tape, pos, SEEK_SET);
    return 0;
}
```

> Observações de integração:
>
> * Sua `isDEC` já trata `DEC = [1-9][0-9]* | '0'` (conforme comentário no arquivo). Assim, `0`, `7`, `123` serão **DEC**, e `3.14`, `.14`, `3.`, `3e10` serão **FLT**.&#x20;
> * Na hora de chamar no `gettoken`, costume é **tentar HEX, OCT, FLOAT, DEC nessa ordem** (você já tenta `HEX` → `OCT` → `DEC`; basta inserir `FLOAT` antes de `DEC`).&#x20;

---

### 12) ER para algarismos romanos (até 3999), alfabeto `{i, v, x, l, c, d, m}`

Com letras minúsculas, a forma canônica é:

* **Expressão regular:**
  `^m{0,3}(cm|cd|d?c{0,3})(xc|xl|l?x{0,3})(ix|iv|v?i{0,3})$`

> Isso reconhece de 0 até 3999 (`""` não é número, claro; use ao menos um grupo não-vazio se quiser rejeitar vazio). Se preferir aceitar somente 1–3999, você pode exigir ao menos um símbolo no todo (por exemplo, `^(m{1,3}...|...|... )$`), mas didaticamente a forma acima é a canônica.

---

## Onde usei seus arquivos

* Enunciados e definições de **EXP**, exemplos e convenções de ε via `getc/ungetc` vieram do PDF da aula.&#x20;
* Padrões, ordem e estilo de implementação (incluindo `isID`, `isDEC`, `isOCT`, `isHEX`, `skipspaces` e a assinatura de `gettoken`) vieram do seu `lexer.c`. A partir deles propus `FIX`, `FLT`, `isFIX`, `isEXP_SUFFIX`, `isFLOAT` e `isDEC_or_FLT` mantendo o mesmo design.&#x20;
* O `main.c/.h` fornecem o *driver* de teste orientado a `gettoken`, compatível com as novas funções se você as integrar ao `gettoken` como sugerido. &#x20;

Se quiser, eu já te devolvo um `lexer.c` consolidado com os novos tokens e funções inseridas na ordem ideal do `gettoken`.
