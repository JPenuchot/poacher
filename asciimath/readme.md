# AsciiMath

Constexpr AsciiMath parser for C++20.

Other AsciiMath parsers, used as references:

- [Official AsciiMath parser](
  https://github.com/asciimath/asciimathml/blob/master/ASCIIMathML.js)
- [Ruby parser from AsciiDoctor](
  https://github.com/asciidoctor/asciimath/blob/master/lib/asciimath/parser.rb)

The goal of this project is to implement a high performance compiler for mathematical expressions using Blaze and/or Eigen.

## Helpful info

[Grammar by AsciiDoctor](
https://github.com/asciidoctor/asciimath/blob/master/lib/asciimath/parser.rb):

```
identifier = [A-z]
symbol = /* any string in the symbol table */
number = '-'? [0-9]+ ( '.' [0-9]+ )?
constant = number | symbol | identifier
text = '"' [^"]* ' "'
binary_op = 'frac' | 'root' | 'stackrel'
binary_expr = binary_op simp simp
unary_op = 'sqrt' | 'text'
unary_expr = unary_op simp
rparen = ')' | ']' | '}' | ':)' | ':}'
lparen = '(' | '[' | '{' | '(:' | '{:'
paren_expr = lparen expr rparen
sub =  '_' simp super?
super = '^' simp
fraction = '/' simp
simp = constant | paren_expr | unary_expr | binary_expr | text
expr = ( simp ( fraction | sub | super ) )+
```
