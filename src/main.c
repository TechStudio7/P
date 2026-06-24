#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define INDENT_STEP 4

// Simple buffer that grows
typedef struct {
    char *s;
    size_t len;
    size_t cap;
} Buffer;

static void buf_init(Buffer *b) {
    b->cap = 4096;
    b->s = malloc(b->cap);
    b->len = 0;
}

static void buf_append(Buffer *b, const char *t) {
    size_t n = strlen(t);
    if (b->len + n + 1 > b->cap) {
        while (b->len + n + 1 > b->cap) b->cap *= 2;
        b->s = realloc(b->s, b->cap);
    }
    memcpy(b->s + b->len, t, n);
    b->len += n;
    b->s[b->len] = '\0';
}

static void buf_putc(Buffer *b, char c) {
    if (b->len + 2 > b->cap) {
        b->cap *= 2;
        b->s = realloc(b->s, b->cap);
    }
    b->s[b->len++] = c;
    b->s[b->len] = '\0';
}

static char *read_file(const char *path) {
    FILE *f = fopen(path, "rb");
    if (!f) return NULL;
    fseek(f, 0, SEEK_END);
    long sz = ftell(f);
    fseek(f, 0, SEEK_SET);
    char *buf = malloc(sz + 1);
    if (!buf) { fclose(f); return NULL; }
    fread(buf, 1, sz, f);
    buf[sz] = '\0';
    fclose(f);
    return buf;
}

// Check if identifier at s starting pos matches keyword and is not part of larger id
static int match_keyword(const char *s, const char *kw) {
    size_t klen = strlen(kw);
    if (strncmp(s, kw, klen) != 0) return 0;
    char after = s[klen];
    return after == '\0' || isspace((unsigned char)after) || after == '(' || after == '{';
}

static void emit_indent(Buffer *out, int indent) {
    for (int i = 0; i < indent; ++i) buf_putc(out, ' ');
}

// Very small, forgiving transpiler converting C-like braces to Python indentation.
// It handles: if/for/while/elif/else/def to yield Python-style colons and indentation.
int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <input.p> [output.py]\n", argv[0]);
        return 1;
    }
    char *src = read_file(argv[1]);
    if (!src) { perror("read"); return 1; }

    Buffer out;
    buf_init(&out);

    int indent = 0;
    size_t i = 0;
    size_t n = strlen(src);
    while (i < n) {
        // handle comments
        if (src[i] == '/' && i+1 < n && src[i+1] == '/') {
            // single-line comment -> Python '#'
            buf_putc(&out, '#');
            i += 2;
            while (i < n && src[i] != '\n') {
                buf_putc(&out, src[i++]);
            }
            if (i < n && src[i] == '\n') { buf_putc(&out, '\n'); i++; emit_indent(&out, indent); }
            continue;
        }
        if (src[i] == '/' && i+1 < n && src[i+1] == '*') {
            // block comment -> convert to lines of '#'
            i += 2;
            buf_putc(&out, '#');
            while (i+1 < n && !(src[i] == '*' && src[i+1] == '/')) {
                if (src[i] == '\n') buf_putc(&out, '\n'), buf_putc(&out, '#');
                else buf_putc(&out, src[i]);
                i++;
            }
            if (i+1 < n) i += 2;
            buf_putc(&out, '\n');
            emit_indent(&out, indent);
            continue;
        }

        // skip whitespace at top of loop, but preserve newlines
        if (isspace((unsigned char)src[i])) {
            if (src[i] == '\n') {
                buf_putc(&out, '\n');
                // after newline preserve current indent
                emit_indent(&out, indent);
            }
            i++; continue;
        }

        // handle closing brace '}' -> dedent then newline + indent
        if (src[i] == '}') {
            if (indent >= INDENT_STEP) indent -= INDENT_STEP;
            // ensure we output a newline if not at line start
            size_t len = out.len;
            if (len == 0 || out.s[len-1] != '\n') buf_putc(&out, '\n');
            emit_indent(&out, indent);
            i++; continue;
        }

        // handle keywords that typically have parens
        if (match_keyword(&src[i], "if") || match_keyword(&src[i], "while") || match_keyword(&src[i], "for") || match_keyword(&src[i], "elif")) {
            // extract keyword
            const char *kw = NULL;
            if (match_keyword(&src[i], "if")) kw = "if";
            else if (match_keyword(&src[i], "while")) kw = "while";
            else if (match_keyword(&src[i], "for")) kw = "for";
            else if (match_keyword(&src[i], "elif")) kw = "elif";
            size_t klen = strlen(kw);
            i += klen;
            // skip spaces
            while (i < n && isspace((unsigned char)src[i])) i++;
            // if next char is '(', capture until matching ')'
            if (i < n && src[i] == '(') {
                size_t start = ++i; int depth = 1;
                while (i < n && depth > 0) {
                    if (src[i] == '(') depth++;
                    else if (src[i] == ')') depth--;
                    i++;
                }
                size_t end = i - 1;
                // copy inner content
                char *inner = malloc(end - start + 1);
                memcpy(inner, src + start, end - start);
                inner[end-start] = '\0';
                // emit: keyword + ' ' + inner + ':'
                buf_append(&out, kw);
                buf_putc(&out, ' ');
                // trim leading/trailing spaces of inner
                char *p = inner; while (*p && isspace((unsigned char)*p)) p++;
                char *q = inner + strlen(inner) - 1; while (q >= p && isspace((unsigned char)*q)) *q-- = '\0';
                buf_append(&out, p);
                buf_append(&out, ":\n");
                free(inner);
                // increase indent and emit indent
                indent += INDENT_STEP;
                emit_indent(&out, indent);
                // skip optional whitespace to the '{' which we'll consume in next loop
                while (i < n && isspace((unsigned char)src[i])) i++;
                if (i < n && src[i] == '{') i++; // consume
                continue;
            }
        }

        // handle 'else' which may be 'else {'
        if (match_keyword(&src[i], "else")) {
            i += 4; // skip 'else'
            while (i < n && isspace((unsigned char)src[i])) i++;
            if (i < n && src[i] == '{') i++; // consume
            buf_append(&out, "else:\n");
            indent += INDENT_STEP;
            emit_indent(&out, indent);
            continue;
        }

        // handle opening brace in other contexts: convert to colon and increase indent
        if (src[i] == '{') {
            buf_append(&out, ":\n");
            indent += INDENT_STEP;
            emit_indent(&out, indent);
            i++; continue;
        }

        // semicolons -> newline + indent
        if (src[i] == ';') {
            buf_putc(&out, '\n');
            emit_indent(&out, indent);
            i++; continue;
        }

        // default: copy char
        buf_putc(&out, src[i]);
        i++;
    }

    // write to stdout or output file
    if (argc >= 3) {
        FILE *fo = fopen(argv[2], "wb");
        if (!fo) { perror("open output"); return 1; }
        fwrite(out.s, 1, out.len, fo);
        fclose(fo);
    } else {
        fwrite(out.s, 1, out.len, stdout);
    }

    free(src);
    free(out.s);
    return 0;
}
