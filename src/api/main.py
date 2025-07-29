from fastapi import FastAPI
from fastapi.responses import JSONResponse
import subprocess
import os

app = FastAPI(
    title="Quantum SPHINCS+ API Demo",
    description="API para execução de testes SPHINCS+ e simulações quânticas",
    version="1.0.0"
)

# --- Função auxiliar para executar comandos ---
def run_command(command, cwd=None):
    try:
        result = subprocess.run(
            command,
            capture_output=True,
            text=True,
            cwd=cwd
        )
        return {
            "command": " ".join(command),
            "stdout": result.stdout,
            "stderr": result.stderr,
            "return_code": result.returncode
        }
    except Exception as e:
        return {"error": str(e)}

# --- Descobrir path dinâmico ---
def get_attack_path():
    """Define o path correto dos códigos quânticos conforme ambiente (local ou Railway)"""
    if os.path.exists("src/quantum_attacks"):
        return os.path.join("src", "quantum_codes")
    else:
        return os.path.join("src", "python")

def get_c_tests_path():
    """Define o path dos testes em C (não existe no Railway, só local)"""
    path = os.path.join("src", "c_tests")
    if os.path.exists(path):
        return path
    else:
        return None

# --- Endpoints ---

@app.get("/")
def root():
    return {"message": "Quantum SPHINCS+ API online"}

@app.get("/run_c_tests")
def run_c_tests():
    """Executa os testes SPHINCS+ em C (somente se disponíveis)"""
    path = get_c_tests_path()
    if path is None:
        return JSONResponse(content={"error": "Testes em C não disponíveis neste ambiente."})

    result = run_command(["./tests"], cwd=path)
    return JSONResponse(content=result)

@app.get("/run_attack/ghz")
def run_attack_ghz():
    """Executa ataque quântico GHZ"""
    path = get_attack_path()
    result = run_command(["python", "quantum_simulation_ghz.py"], cwd=path)
    return JSONResponse(content=result)

@app.get("/run_attack/4qubits")
def run_attack_4qubits():
    """Executa ataque quântico com 4 qubits"""
    path = get_attack_path()
    result = run_command(["python", "quantum_simulation_4_qubits.py"], cwd=path)
    return JSONResponse(content=result)

@app.get("/run_attack/grover")
def run_attack_grover():
    """Executa ataque quântico Grover"""
    path = get_attack_path()
    result = run_command(["python", "quantum_simulation_grover.py"], cwd=path)
    return JSONResponse(content=result)

@app.get("/run_attack/all")
def run_all_attacks():
    """Executa todos os ataques quânticos sequencialmente"""
    path = get_attack_path()
    scripts = [
        "quantum_simulation_ghz.py",
        "quantum_simulation_4_qubits.py",
        "quantum_simulation_grover.py"
    ]

    results = []

    for script in scripts:
        result = run_command(["python", script], cwd=path)
        results.append({
            "script": script,
            "result": result
        })

    return JSONResponse(content={"results": results})

# ------------------------------------------
# 🚨 ESSA PARTE É ESSENCIAL para Railway 🚨
# ------------------------------------------
if __name__ == "__main__":
    port = int(os.environ.get("PORT", 8000))
    import uvicorn
    uvicorn.run("src.api.main:app", host="0.0.0.0", port=port)
