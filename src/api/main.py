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

# --- Endpoints ---

@app.get("/")
def root():
    return {"message": "Quantum SPHINCS+ API online"}

@app.get("/run_c_tests")
def run_c_tests():
    """Executa os testes SPHINCS+ em C"""
    path = os.path.join("src", "c_tests")
    result = run_command(["./tests"], cwd=path)
    return JSONResponse(content=result)

@app.get("/run_attack/ghz")
def run_attack_ghz():
    """Executa ataque quântico GHZ"""
    path = os.path.join("src", "quantum_attacks")
    result = run_command(["python", "quantum_attack_ghz.py"], cwd=path)
    return JSONResponse(content=result)

@app.get("/run_attack/4qubits")
def run_attack_4qubits():
    """Executa ataque quântico com 4 qubits"""
    path = os.path.join("src", "quantum_attacks")
    result = run_command(["python", "quantum_attack_4_qubits.py"], cwd=path)
    return JSONResponse(content=result)

@app.get("/run_attack/grover")
def run_attack_grover():
    """Executa ataque quântico Grover"""
    path = os.path.join("src", "quantum_attacks")
    result = run_command(["python", "quantum_attack_grover.py"], cwd=path)
    return JSONResponse(content=result)

@app.get("/run_attack/all")
def run_all_attacks():
    """Executa todos os ataques quânticos sequencialmente"""
    path = os.path.join("src", "quantum_attacks")
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
# 🚨 ESSA PARTE É ESSENCIAL para Railway 🚨
# ------------------------------------------
if __name__ == "__main__":
    port = int(os.environ.get("PORT", 8080))  # Railway define a variável PORT automaticamente
    import uvicorn
    uvicorn.run(app, host="0.0.0.0", port=port)
