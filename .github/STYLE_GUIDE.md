# Moztopia Project Style Guide

This guide defines the structural and stylistic conventions shared across all Moztopia repositories.

---

## ✅ Code Style .. in general

- Favor clarity over cleverness.
- Use consistent indentation (2 spaces for YAML, 4 spaces for code).
- Group related logic; keep components modular.
  Specifical language guides are as follows:
- PHP Language .. [**PSR-12**](https://www.php-fig.org/psr/psr-12/)
- Dart Language .. [**Effective Dart**](https://dart.dev/effective-dart)
- JavaScript Language .. [**ESLint**](https://eslint.org/docs/latest/contribute/code-conventions)
- C Language .. [**Indian Hill**](https://www.doc.ic.ac.uk/lab/cplus/cstyle.html)

---

## 🔖 Commit Conventions

- Use semantic prefixes: `feat:`, `fix:`, `docs:`, `chore:`, etc.
- Use present tense, imperative voice:  
  ✔️ `Add login timeout hook`  
  ❌ `Added a timeout`

---

## 📁 Folder Layout

- `.github/` contains workflows, templates, contributor guides.
- `.wiki/` stores project documentation (from template repo).
- `src/`, `scripts/`, and `docs/` reflect purpose by name.

---

## 📚 Documentation

- All public functions/modules should include descriptive headers.
- Use Markdown for internal and contributor-facing docs.
- README files must include: Overview, Setup, Usage, Contribute, License.

---

## 🎨 UX & Copy

- Label buttons and forms clearly with verbs.
- Use neutral, inclusive, and non-jargon language in UI and docs.
- Favor active voice and sentence fragments for brevity.

---

## 🤝 Pull Requests

- Include a summary, screenshots (if applicable), and "before/after" when relevant.
- Link related issues using `Closes #<ID>` or `Fixes #<ID>`.

---

This guide evolves—contributions welcome.
