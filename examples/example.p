// Example in P (Python-with-braces)
def fact(n) {
    if (n <= 1) {
        return 1;
    }
    else {
        return n * fact(n - 1);
    }
}

// Use a for-loop style (transpiler keeps parens content)
def main() {
    for (i = 1; i <= 6; i = i + 1) {
        print(fact(i));
    }
}

main();
