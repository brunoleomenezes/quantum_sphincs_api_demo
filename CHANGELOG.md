# 📦 CHANGELOG

## [v1.0.0-pre] — 2025-05-04

**Initial research release (Pre-release)**  
**Status:** Pre-release — For academic research and experimentation only.

### Added
- Initial integration of SPHINCS+ post-quantum signature algorithm (C implementation and Python bindings).
- Quantum attack simulation modules using Qiskit:
  - GHZ state attacks.
  - 4-qubit entanglement attacks.
  - Grover's algorithm attacks.
- Full test suite for SPHINCS+ using CMocka.
- REST API endpoints for:
  - Running SPHINCS+ C tests.
  - Running individual quantum attacks.
  - Running all quantum attacks sequentially.
- Automated API responses for attack simulation results.
- Example API usage (curl examples and JSON output).
- Academic Non-Commercial License v1.0 enforcement.
- Citation instructions in LICENSE and README.

### Notes
- This is a **pre-release version** intended for academic research and reproducibility of experiments published at SEGeT 2024.
- Not recommended for production or commercial usage.
- API and attack modules may change in future stable releases.

---

## 📌 Planned for future releases

- Extended quantum attack scenarios.
- Docker and CI/CD pipeline for easy deployment.
- Enhanced API security and performance optimization.
- Comprehensive documentation and usage examples.

---

> **Important**: Always cite the official publication when using this software in research.

MENEZES, Bruno Leonardo Santos et al. UMA SIMULAÇÃO DE ATAQUE QUÂNTICO AO SPHINCS. SEGeT 2024. https://www.aedb.br/seget/arquivos/artigos24/10335113.pdf
