# MiaKomp syntax

The rules of the language as decided so far. Source files use the `.miak` extension.
Anything not listed here is undecided, not implied. See **Open questions** at the bottom.

---

## Program structure

- The top level may contain only **declarations**: `make` and `fn`. No statements.
  `i = 9;` or `print(i);` at the top level is an error (the C++ rule).
- `main { ... }` is the entry point, like `main` in C++. Execution starts there.
- Top-level `make` variables are in the outer scope, visible to `main` and to every function.

```
make i:int = 5;

main {
    i = 9;
    print(i);
}
```

## Declarations and assignment

- `make name:type = value;` declares a new variable. The type annotation is **required**.
- `name = value;` assigns to an existing variable. It never creates one.
- Assigning in an inner scope changes the variable where it was declared (no Python `global` needed).

```
make count:int = 0;
count = count + 1;
```

## Types

- `int`: integer
- `float`: decimal
- `bool`: `true` / `false`
- `string`: text
- Types are checked **before the program runs**, by a type-checking pass between the parser and
  the backends. `make b:bool = 5;` is rejected without running anything.
- Until that pass exists, annotations are parsed and stored, and the interpreter ignores them.

## Functions

- `fn name(param:type, ...):returnType { ... }`. The parameter types and the return type are
  **required**. Whitespace around `:` does not matter.
- Parameters may have defaults: `b:int = 5`.
- `RET value;` returns. `RET` is uppercase.
- A function that can reach its end without `RET` is a type-check error.

```
fn checkAddition(a:int, b:int = 5):bool {
    if (a + b) =? 3 {
        RET true;
    }
    RET false;
}
```

## Control flow

- `if condition { ... } else { ... }`
- `while condition { ... }`
- The condition is any expression and ends at `{`. Parentheses around it are optional:
  `while i <? 15 {` and `while (i <? 15) {` are the same.
- A condition must be a `bool`. `if 5 {` is a type-check error.

## Operators

| Kind       | Operators                          |
|------------|------------------------------------|
| Arithmetic | `+` `-` `*` `/`, unary `-`         |
| Comparison | `=?` `!=?` `<?` `>?` `<=?` `>=?`   |
| Logic      | `and` `or` `not`                   |
| Assignment | `=`                                |

- Every comparison ends in `?`. A lone `<`, `>` or `!` is not a valid token.
- `and` / `or` **short-circuit**: in `a and b`, `b` is not evaluated when `a` is false; in
  `a or b`, `b` is not evaluated when `a` is true.
- The operands of `and`, `or` and `not` must be `bool`.
- `-` is always its own token. `-6` is `-` followed by `6`, and the parser handles unary minus,
  so `i-6` is subtraction.

## Literals

- **int**: digits only: `0`, `42`
- **float**: digits, `.`, digits. Both sides of the point are required: `3.5` is legal, `5.` and
  `.5` are errors.
- **bool**: `true`, `false`
- **string**: `"..."`. Supported escapes: `\"` `\\` `\n` `\t`. Any other escape is an error.
  An unterminated string is an error. A string may span lines: a literal line break between the
  quotes is part of the value.

## Printing

- `print(value);`
- `print` is a **built-in function**, not a keyword. The lexer sees an identifier followed by `(`,
  and the call is parsed like any other function call.
- A `bool` prints as `true` / `false`, not `1` / `0`.

## Lexical rules

- `#` starts a comment that runs to the end of the line.
- Every statement ends with `;`. Newlines have no meaning.
- Keywords are **case-sensitive**: `RET` is a keyword, `ret` is an identifier.
- Keywords: `make` `fn` `RET` `if` `else` `while` `main` `and` `or` `not` `true` `false`
  `int` `float` `bool` `string`. `print` is not a keyword.
- Identifiers: a letter or `_`, then letters, digits or `_`.
- Punctuation: `(` `)` `{` `}` `;` `,` `:`

---

## Open questions

Decide these before the stage that needs them:

1. **`main`'s return type**: return types are required everywhere else. Is `main` an exception,
   or is it `main:int {`? *(parser)*
2. **Operator precedence**: proposed: `or` < `and` < `not` < comparisons < `+ -` < `* /` <
   unary `-`. *(parser)*
3. **Defaults**: must parameters with defaults come last, as in C++? *(parser)*
4. **`int / int`**: integer division, or a `float` result? *(type checker)*
5. **Mixing `int` and `float`**: is `1 + 2.5` legal? *(type checker)*

---

## Example

```
make i:int = 5;

fn checkAddition(a:int, b:int = 5):bool {
    if (a + b) =? 3 {
        RET true;
    }
    RET false;
}

main {
    i = 9;
    print(i);                               # 9
    make b:bool = checkAddition(i, -6);
    print(b);                               # true
    while i <? 15 {
        print(i);                           # 9 10 11 12 13 14
        i = i + 1;
    }
}
```
