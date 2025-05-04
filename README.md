
# Quantum SPHINCS+ API Demo

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)
[![Python](https://img.shields.io/badge/Python-3.12-blue.svg)](https://www.python.org/)
[![SPHINCS+](https://img.shields.io/badge/SPHINCS%2B-integrated-brightgreen)](https://github.com/sphincs/sphincsplus)

Este repositório demonstra a integração entre o algoritmo de assinaturas digitais pós-quânticas **SPHINCS+** e ataques simulados com circuitos quânticos utilizando **Qiskit**. 

O projeto inclui:

- 📌 Testes completos do SPHINCS+ com integração em C e Python.
- 📌 Simulações quânticas para ataque com GHZ, Grover e emaranhamento.
- 📌 Integração entre execução nativa (C) e scripts de ataque (Python).
- 📌 Avaliação da integridade das assinaturas após ataques simulados.

## 📁 Estrutura do Repositório

```
quantum_sphincs_api_demo/
├── external/                # Submodule SPHINCS+ (https://github.com/sphincs/sphincsplus.git)
├── src/
│   ├── c_tests/             # Testes em C com CMocka
│   └── quantum_attacks/    # Scripts Python com Qiskit para ataques simulados
├── LICENSE
├── .gitignore
└── README.md
```

## 🚀 Como Clonar

```bash
git clone --recurse-submodules https://github.com/brunoleomenezes/quantum_sphincs_api_demo.git
cd quantum_sphincs_api_demo
```

## ⚙️ Preparação do Ambiente

### Dependências

- GCC
- Python 3.12
- Qiskit
- CMocka
- OpenSSL

Em sistemas baseados em Fedora (exemplo do estudo):

```bash
sudo dnf install gcc python3 python3-devel cmocka-devel openssl-devel
pip install qiskit
```

## 🛠️ Compilando o SPHINCS+

Entre na pasta `external/sphincsplus` e compile:

```bash
cd external/sphincsplus/ref
make
```

## ✅ Executando os Testes em C

Na pasta raiz:

```bash
cd src/c_tests
make
./tests
```

## 🧪 Executando as Simulações Quânticas

Os scripts estão em `src/quantum_attacks`:

```bash
cd src/quantum_attacks
python quantum_attack_ghz.py
python quantum_attack_4_qubits.py
python quantum_attack_grover.py
```

## 🔄 Atualizando o Submódulo SPHINCS+

```bash
git submodule update --remote --merge
git add .gitmodules external/sphincsplus
git commit -m "Atualiza submodule SPHINCS+"
git push origin main
```

## 📜 Licença

Este projeto é licenciado sob a licença MIT. Veja o arquivo [LICENSE](LICENSE) para detalhes.

## 👨‍💻 Créditos

- **Bruno Leonardo Santos Menezes**
- **Franklin de Lima Marquezino**
- **Claudio Miceli de Farias**

---
Este projeto é parte de um estudo científico para avaliação de algoritmos pós-quânticos e encontra-se em desenvolvimento contínuo.
