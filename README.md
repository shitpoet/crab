# skim

It is a simple rewriter, working almost at character level. It does simple transformatins without constructing full fedged AST and even without full lexical analysis. For speed.

Transformations include adding braces for indented blocks of code, adding brackets for `if`, `for`, `while` conditions and expanding several shortcuts (`elif`, `fun`).

Target language - javascript. But can be adopted for any C-like language.

Rewriter has two modes: compact and expanded. In compact mode line numbers are preserved, what is usefull for debugging. In expanded mode line numbers aren't preserved because closing braces are added on new line. This makes code more readable.

## Example

```
fun hello(name)
  if name.length > 0
    if name.toLowerCase()==name
      name = name[0].toUpperCase + name.slice(1)
  else
    throw new Error('oh no')
  console.log(`hello ${name}`)
```

