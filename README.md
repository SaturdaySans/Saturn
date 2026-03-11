# Saturn

Saturn is a lightweight cpp interpreter :D <br> It supports variables, math, printing, conditionals, loops, and strings!!!

---

## Running the Lagrange:

1. Compile the interpreter (once):

```bash
g++ saturn.cpp -o saturn
```

2. Run a `.sat` script:

```bash
./saturn hello.sat
```

- The `saturn` executable is the interpreter that runs `.sat` files.
- You can run scripts from anywhere once the interpreter is compiled.

---

## Syntax

### Variables Declaration

```sat
let x = 67
let name = "SuxSEVEN"
```

- No explicit types: numbers and strings are inferred
- Stored internally in a key-value map

---

### Printing 

```sat
print "Hello world"
print x
```

- Strings must be in quotes (`"..."`)
- Variables print their stored value

---

### Basic Operators

Supported operators: `+ - * /`

```sat
let a = 10
let b = 5

let sum = a + b
let diff = a - b
let prod = a * b
let div = a / b

print sum
print diff
print prod
print div
```

---

### If Statements

```sat
if x > 5
    print "x is MASSIVE"
end
```

- Comparisons: `> < >= <= == !=`  
- Blocks have to end with `end`

---

### For Loops

```sat
for i = 1 to 5
    print i
end
```

- Loop variable increments automatically
- Loop ends when the variable exceeds the end value

---

### Using Comments

```sat
# Cement
let score = 100
```

- Lines starting with `#` are ignored by the interpreter

---

### Strings

```sat
let greeting = "Hello Saturn"
print greeting
```

- Strings always use double quotes `"..."`.  

---

### Input

```sat
input namme "Enter name: "
input age
```

- Input Variable "prompt text"
- Prompt text is optional

### Sampel Code

```sat
# mewo
input hp "Hp:"
input dmg "Dmg:"
input rounds "Rounds:"

let remaining = hp - dmg
print remaining

if remaining < 50
    print "low hp"
end

for i = 1 to rounds
    if remaining > 0
        print "Rounds:"
        print i
        let remaining = remaining - dmg
        print remaining
    end
end
```

---

### Extra

- Scripts should have the `.sat` extension
- Blocks always end with `end`

Tysm! <3

