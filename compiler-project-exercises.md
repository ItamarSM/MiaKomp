# Path to Building a Compiler / Programming Language — Exercise Ladder

Five projects, in order, each building the skills needed for the next. The end
goal: a real pipeline of source → tokens → AST → bytecode → execution.

---

## 1. Expression Calculator

**Goal:** Build a command-line calculator that reads an expression as a string
and evaluates it correctly, respecting operator precedence and parentheses.

**Requirements (in order of difficulty):**
1. Support `+`, `-`, `*`, `/` on two numbers: `3 + 4` → `7`
2. Support chained operations: `3 + 4 - 2` → `5`
3. Respect precedence: `3 + 4 * 2` → `11` (not `14`)
4. Support parentheses: `(3 + 4) * 2` → `14`
5. Support negative numbers: `-3 + 4` → `1`
6. (Stretch) Support decimals: `3.5 * 2` → `7`
7. (Stretch) Support `^` for exponents, higher precedence than `*`/`/`

**Architecture — build it in two clear stages, don't skip the split:**

### Stage 1: Tokenizer (Lexer)
Write a function that takes the raw string `"3 + 4 * 2"` and turns it into a
list of tokens, e.g.:
```
[NUMBER(3), PLUS, NUMBER(4), STAR, NUMBER(2)]
```
- Define a `Token` struct: a type (enum: `NUMBER`, `PLUS`, `MINUS`, `STAR`,
  `SLASH`, `LPAREN`, `RPAREN`, `END`) plus a value (for numbers).
- Walk the string character by character, skip whitespace, group consecutive
  digits into a number token.

### Stage 2: Parser + Evaluator (recursive descent)
This is the core concept. Structure it as layered functions, one per
precedence level, each calling the next level down:

```
parseExpression()   // handles + and -
    → parseTerm()    // handles * and /
        → parseFactor()  // handles numbers and (parenthesized expressions)
```

- `parseExpression()` calls `parseTerm()` for its left side, then loops: if it
  sees `+` or `-`, consume it and call `parseTerm()` again for the right side,
  combining results.
- `parseTerm()` does the same thing one level down, but for `*` and `/`,
  calling `parseFactor()`.
- `parseFactor()` handles the base case: if it sees a number, return it; if it
  sees `(`, recursively call `parseExpression()` inside, then expect `)`.

This layering is *why* precedence works automatically — lower-precedence
operators sit "higher" in the call chain, so they get evaluated last.

**Why this works — trace of `3 + 4 * 2`:**
`parseExpression()` calls `parseTerm()` for the left side. `parseTerm()` calls
`parseFactor()`, gets `3`, checks for `*`/`/` — sees `+` instead, so it just
returns `3` immediately without consuming it. Back in `parseExpression()`, it
sees the `+`, consumes it, calls `parseTerm()` again for the right side. *This*
call to `parseTerm()` gets `4`, then sees `*`, so it consumes it, calls
`parseFactor()` for `2`, and returns `4 * 2 = 8`. Back in `parseExpression()`:
`3 + 8 = 11`. The multiplication got "absorbed" one level down before
addition ever saw it.

**Why parentheses reset everything:**
When `parseFactor()` sees `(`, it calls `parseExpression()` — the top-level
function — recursively. Inside the parens, precedence rules apply fresh, as
if it were a brand new expression. That's the whole trick: parentheses just
mean "recurse back to the top."

**A note on state:** your parser needs some notion of "current position in
the token list" that persists across these calls — usually either an index
variable passed around, or a class holding the token list + current index as
member state (cleaner in C++, since these functions call each other
constantly).

**Suggested build order:**
1. Get the tokenizer working standalone first — print out the token list for
   a few test strings before writing any parsing logic.
2. Implement just `parseFactor()` + `parseExpression()` (no `*`/`/` yet) —
   get `3 + 4 - 2` working.
3. Add `parseTerm()` and wire it in — get precedence working.
4. Add parentheses support.
5. Add negative number handling (unary minus) — this one's a little tricky,
   think about where it fits in the chain.

---

## 2. JSON Parser

**Goal:** Parse a JSON string into an in-memory data structure you can
inspect/print, e.g. turn `{"name": "Alice", "age": 30}` into something
queryable in your program.

**Requirements (build in this order):**
1. Parse literals: `true`, `false`, `null`
2. Parse numbers: `42`, `3.14`, `-7` (integers and decimals, negatives)
3. Parse strings: `"hello"` (handle basic escape sequences like `\"` and
   `\\` if you want to go further)
4. Parse arrays: `[1, 2, 3]`, including nested arrays: `[1, [2, 3], 4]`
5. Parse objects: `{"key": "value", "key2": 42}`, including nested
   objects/arrays as values
6. Whitespace handling: JSON allows arbitrary whitespace/newlines between
   tokens — your tokenizer needs to skip it

**Architecture — same two-stage split as the calculator:**

### Stage 1: Tokenizer
Turn the raw string into tokens: `LBRACE`, `RBRACE`, `LBRACKET`, `RBRACKET`,
`COLON`, `COMMA`, `STRING(value)`, `NUMBER(value)`, `TRUE`, `FALSE`, `NULL`,
`END`.

### Stage 2: The data structure — this is the new/hard part
Unlike the calculator (where everything was just a number), JSON values can
be one of several *different types*, and they nest inside each other. You
need a type that can represent: a string, a number, a bool, null, an array
of JsonValues, or a map of string→JsonValue — all under one umbrella type.

This is your first real encounter with needing a **tagged union / variant**
in C++. Two ways to do it:
- Use `std::variant<std::monostate, bool, double, std::string,
  std::vector<JsonValue>, std::map<std::string, JsonValue>>` (modern C++,
  recommended — forces you to learn `std::variant` and `std::visit`)
- Or hand-roll it with an enum tag + a union/struct (more control, more
  C-like, good if you want the harder version)

Note the recursion in the type itself: `JsonValue` contains a
`vector<JsonValue>` and a `map<string, JsonValue>` — the type is defined in
terms of itself. This is normal for tree-shaped data and will click once you
see it compile.

### Stage 3: Parser — mirrors your calculator's structure

```
parseValue():
    if current token is STRING: return it
    if current token is NUMBER: return it
    if current token is TRUE/FALSE/NULL: return it
    if current token is LBRACKET: return parseArray()
    if current token is LBRACE: return parseObject()

parseArray():
    consume LBRACKET
    values = []
    while current token isn't RBRACKET:
        values.push(parseValue())      // recursion!
        if current token is COMMA: consume it
    consume RBRACKET
    return values

parseObject():
    consume LBRACE
    map = {}
    while current token isn't RBRACE:
        key = parse STRING
        consume COLON
        map[key] = parseValue()        // recursion!
        if current token is COMMA: consume it
    consume RBRACE
    return map
```

The key difference from the calculator: here the recursion is `parseValue()`
calling `parseArray()`/`parseObject()`, which call `parseValue()` again for
each element — recursing on *nesting depth*, not operator precedence.

**Suggested build order:**
1. Tokenizer first — print tokens for a few sample JSON strings, confirm
   they look right.
2. Get literals + numbers + strings parsing into a working `JsonValue` (no
   arrays/objects yet) — test with just `"hello"` or `42`.
3. Add array parsing — test with `[1, 2, 3]`, then `[1, [2, 3]]`.
4. Add object parsing — test with `{"a": 1}`, then nested ones.
5. Write a `print()`/`toString()` function for `JsonValue` so you can
   visually confirm your parser worked — this doubles as a good exercise in
   `std::visit` if you used `std::variant`.

Test files to try once basic parsing works: a small config-style JSON with
mixed nesting (object containing an array of objects) — that's the real
stress test for the recursion.

---

## 3. Stack-Based Virtual Machine

**Goal:** Build a VM that executes a fixed instruction set from a stack — no
parsing involved, you hand-write instruction sequences directly and focus
entirely on execution.

**Requirements (build in this order):**
1. Arithmetic: `PUSH`, `ADD`, `SUB`, `MUL`, `DIV` — enough to compute
   `3 + 4 * 2` as hardcoded bytecode
2. Output: `PRINT` — pop and print the top of the stack
3. Variables: `STORE`, `LOAD` — a way to save a value to a named/indexed
   slot and load it back later
4. Control flow: `JUMP`, `JUMP_IF_FALSE` — unconditional and conditional
   jumps to a given instruction index
5. Comparison ops: `EQ`, `LT`, `GT` — needed to make `JUMP_IF_FALSE`
   actually useful (loops, conditionals)
6. (Stretch) `CALL`/`RET` — function calls, which need a second stack (call
   stack) to track return addresses

**Architecture:**

### Stage 1: The instruction representation
Define an `Instruction` struct: an opcode (enum: `PUSH`, `ADD`, `SUB`,
`STORE`, `LOAD`, `JUMP`, `JUMP_IF_FALSE`, `PRINT`, etc.) plus an optional
operand (the value for `PUSH`, the slot index for `STORE`/`LOAD`, the target
index for jumps). A full program is just a `vector<Instruction>` — you'll
write these by hand for now, e.g. representing `3 + 4 * 2` as:
```
PUSH 3
PUSH 4
PUSH 2
MUL
ADD
```

**A note on opcode vs. operand:**
- **Opcode** = the operation itself — *what* to do (`ADD`, `PUSH`, `JUMP`).
  It's what the `switch` in your `run()` loop branches on.
- **Operand** = the data that instruction needs — the argument. Not every
  instruction needs one (`ADD` doesn't; `PUSH 5` does, with `5` as the
  operand).
- Using a single `double` field for the operand is a common simplification,
  but it's a little sloppy: `PUSH`'s operand is a value to compute with,
  while `JUMP`/`STORE`/`LOAD`'s operand is really an index. It works because
  C++ silently converts between `int` and `double`, but if you want to be
  cleaner, use two separate fields (`int intOperand`, `double numOperand`)
  or a `std::variant<int, double>`.

### Stage 2: The VM itself

```
class VM:
    stack: vector<double>
    variables: map<int, double>   // or vector<double> if slots are indices
    pc: int (program counter)

    run(program):
        while pc < program.size():
            instr = program[pc]
            switch instr.opcode:
                case PUSH: stack.push(instr.operand); pc++
                case ADD:  b=pop(); a=pop(); stack.push(a+b); pc++
                case SUB:  b=pop(); a=pop(); stack.push(a-b); pc++
                case STORE: variables[instr.operand] = pop(); pc++
                case LOAD:  stack.push(variables[instr.operand]); pc++
                case JUMP:  pc = instr.operand   // no pc++, jump sets it directly
                case JUMP_IF_FALSE:
                    if pop() == 0: pc = instr.operand
                    else: pc++
                case PRINT: print(pop()); pc++
```

The key mental shift from the calculator/JSON parser: there's no recursion
here. It's a flat loop over a program counter, and `JUMP` instructions just
move that counter around — this is literally how loops and if-statements
compile down to machine-level behavior.

**Suggested build order:**
1. Get `PUSH`/`ADD`/`SUB`/`MUL`/`DIV`/`PRINT` working — hand-write bytecode
   for a few arithmetic expressions and confirm correct output.
2. Add `STORE`/`LOAD` — write a program that stores a value, does math,
   stores the result, loads it back, prints it.
3. Add `JUMP` — write a program with an infinite loop (`JUMP` back to an
   earlier instruction) and manually break it with a print + hardcoded exit
   condition, just to see the pc jumping around.
4. Add `JUMP_IF_FALSE` + comparison ops — write a program that sums 1 to 10
   using a loop (`STORE` a counter, `LOAD` it, compare, `JUMP_IF_FALSE` out,
   otherwise loop back) — this is the real test that control flow works.
5. (Stretch) Add `CALL`/`RET` with a second stack for return addresses —
   write a program that "calls" a reusable block of instructions and
   returns.

The loop-that-sums-1-to-10 program in step 4 is the one to be proud of —
it's the first time you'll have built something that's genuinely
Turing-complete-flavored, entirely by hand, with no parser involved.

---

## 4. Tiny Scripting Language Interpreter

**Goal:** Combine your calculator's parsing skills with new language
features — variables, if/else, while loops, functions — tree-walked
directly (interpret the AST, don't compile to bytecode yet).

**Requirements (build in this order):**
1. Variable declaration/assignment: `let x = 5;`
2. Expressions using variables: `let y = x + 3;`
3. Print statement: `print(y);`
4. If/else: `if (x > 3) { print("big"); } else { print("small"); }`
5. While loops: `while (x > 0) { print(x); x = x - 1; }`
6. Functions with parameters and return values: `fn add(a, b) { return a + b; }`

**Architecture:**
- **Tokenizer**: extend your calculator's to handle keywords (`let`, `if`,
  `else`, `while`, `fn`, `return`), identifiers, `{`, `}`, `;`, `==`, `<`,
  `>`, comparison operators.
- **Parser**: builds a proper AST now, not just a computed value — each node
  type (`AssignNode`, `IfNode`, `WhileNode`, `FunctionCallNode`) needs its
  own struct/class, likely with a shared base class or `std::variant`.
- **Environment**: a `map<string, Value>` that holds variable bindings, with
  a new one created per function call (this is what makes recursion and
  scoping work).
- **Interpreter**: walks the AST recursively — an `evaluate(node)` function
  that dispatches on node type, calling itself on children.

**Suggested build order:** get variables + expressions + print working first
(test: `let x = 5; print(x + 1);`), then if/else, then while loops, then
functions last — functions are the hardest because they need their own
environment/scope and a way to return values back up through the recursive
evaluation.

---

## 5. Compile AST to Bytecode

**Goal:** Take the interpreter from #4 and, instead of walking the AST
directly, compile it into bytecode instructions for the VM you built in
#3 — fusing your two previous projects into one real compiler pipeline.

**Requirements (build in this order):**
1. Compile simple expressions to bytecode: `3 + 4` → `PUSH 3, PUSH 4, ADD`
2. Compile variable assignment/access: `let x = 5;` → `PUSH 5, STORE 0`
   (map variable names to slot indices at compile time)
3. Compile if/else → `JUMP_IF_FALSE` and `JUMP` with computed target indices
4. Compile while loops → `JUMP_IF_FALSE` + `JUMP` back to loop start
5. Compile function calls → `CALL`/`RET` (this needs your VM's stretch goal
   from #3)

**Architecture — this is the new, hard part:**
- A **compiler pass**: walks the same AST from #4, but instead of
  *evaluating* each node, it *emits* instructions into a growing
  `vector<Instruction>`.
- **Two-pass jump resolution**: when compiling an `if` or `while`, you don't
  know the jump target index yet (the body hasn't been compiled). Common
  trick: emit a placeholder `JUMP_IF_FALSE` with a dummy operand, remember
  its position, compile the body, then go back and patch the placeholder
  with the real index now that you know it.
- **Symbol table**: a compile-time `map<string, int>` mapping variable names
  to VM slot indices — separate from the VM's runtime `variables` map, since
  this mapping happens before execution.

**Suggested build order:** start with expression compilation only (no
statements yet) — confirm `PUSH`/arithmetic bytecode gets generated
correctly and runs on your VM. Then add variable STORE/LOAD compilation,
then tackle the jump-patching logic for if/else (the trickiest new concept
here), then while loops (same patching logic, just looped), then functions
last.

Once #5 runs correctly, you'll have built source → tokens → AST → bytecode →
execution — the actual architecture real compilers use.

---

## Resource

*Crafting Interpreters* by Robert Nystrom (free online) walks through
exactly this ladder — first as a tree-walking interpreter, then as a
bytecode VM in C. The concepts port directly to C++ and it's widely
considered the best on-ramp for this path.
