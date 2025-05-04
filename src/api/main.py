from fastapi import FastAPI
from fastapi.responses import JSONResponse
import subprocess
import os

app = FastAPI(
    title="Quantum SPHINCS+ API Demo",
    description="API para execução de testes SPHINCS+ e simulações quânticas",
    version="1.0.0"
)

# Base path para localização dinâmica
BASE_DIR = os.path.dirname(os.path.abspath(__file__))

def get_path(*args):
    return os.path.abspath(os.path.join(BASE_DIR, *args))

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

@app.get("/")
def root():
    return {"message": "Quantum SPHINCS+ API online"}

@app.get("/run_c_tests")
def run_c_tests():
    """Executa os testes SPHINCS+ em C"""
    path = get_path("..", "tests")
    result = run_command(["./test_sphincs"], cwd=path)
    return JSONResponse(content=result)

@app.get("/run_attack/ghz")
def run_attack_ghz():
    """Executa ataque quântico GHZ"""
    path = get_path("python")
    result = run_command(["python", "quantum_attack_ghz.py"], cwd=path)
    return JSONResponse(content=result)

@app.get("/run_attack/4qubits")
def run_attack_4qubits():
    """Executa ataque quântico com 4 qubits"""
    path = get_path("python")
    result = run_command(["python", "quantum_attack_4_qubits.py"], cwd=path)
    return JSONResponse(content=result)

@app.get("/run_attack/grover")
def run_attack_grover():
    """Executa ataque quântico Grover"""
    path = get_path("python")
    result = run_command(["python", "quantum_attack_grover.py"], cwd=path)
    return JSONResponse(content=result)

@app.get("/run_attack/all")
def run_all_attacks():
    """Executa todos os ataques quânticos sequencialmente"""
    path = get_path("python")
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

if __name__ == "__main__":
    port = int(os.environ.get("PORT", 8000))
    import uvicorn
    uvicorn.run("src.api.main:app", host="0.0.0.0", port=port)
