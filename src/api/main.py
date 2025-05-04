
from fastapi import FastAPI
from fastapi.responses import JSONResponse
import subprocess
import os

app = FastAPI(
    title="Quantum SPHINCS+ API Demo",
    description="API para execução de testes SPHINCS+ e simulações quânticas",
    version="1.0.0"
)

# Base directory para corrigir os caminhos (importante para Railway)
BASE_DIR = os.path.dirname(os.path.abspath(__file__))

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

# --- Endpoints ---

@app.get("/")
def root():
    return {"message": "Quantum SPHINCS+ API online"}

@app.get("/run_c_tests")
def run_c_tests():
    """Executa os testes SPHINCS+ em C"""
    path = os.path.join(BASE_DIR, "..", "tests")
    result = run_command(["gcc", "test_sphincs.c", "-o", "test_sphincs", "-I../../external/sphincsplus/ref", "../../external/sphincsplus/ref/*.c", "-lm"], cwd=path)
    if result["return_code"] != 0:
        return JSONResponse(content=result)
    result_exec = run_command(["./test_sphincs"], cwd=path)
    return JSONResponse(content=result_exec)

@app.get("/run_attack/ghz")
def run_attack_ghz():
    """Executa ataque quântico GHZ"""
    path = os.path.join(BASE_DIR, "..", "python")
    result = run_command(["python", "quantum_attack_ghz.py"], cwd=path)
    return JSONResponse(content=result)

@app.get("/run_attack/4qubits")
def run_attack_4qubits():
    """Executa ataque quântico com 4 qubits"""
    path = os.path.join(BASE_DIR, "..", "python")
    result = run_command(["python", "quantum_attack_4_qubits.py"], cwd=path)
    return JSONResponse(content=result)

@app.get("/run_attack/grover")
def run_attack_grover():
    """Executa ataque quântico Grover"""
    path = os.path.join(BASE_DIR, "..", "python")
    result = run_command(["python", "quantum_attack_grover.py"], cwd=path)
    return JSONResponse(content=result)

@app.get("/run_attack/all")
def run_all_attacks():
    """Executa todos os ataques quânticos sequencialmente"""
    path = os.path.join(BASE_DIR, "..", "python")
    scripts = [
        "quantum_attack_ghz.py",
        "quantum_attack_4_qubits.py",
        "quantum_attack_grover.py"
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
# ESSENCIAL para Railway rodar corretamente
# ------------------------------------------
if __name__ == "__main__":
    port = int(os.environ.get("PORT", 8000))
    import uvicorn
    uvicorn.run("main:app", host="0.0.0.0", port=port)
