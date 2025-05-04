import os
from fastapi import FastAPI, HTTPException
from fastapi.responses import JSONResponse
import subprocess

app = FastAPI(
    title="Quantum SPHINCS+ API Demo",
    description="API para execução de testes SPHINCS+ e simulações quânticas",
    version="1.0.0"
)

# Detectar se está em produção ou não (Railway define o env RAILWAY_ENVIRONMENT)
IS_PRODUCTION = os.getenv("RAILWAY_ENVIRONMENT") is not None

def run_command(command, cwd=None):
    try:
        result = subprocess.run(command, capture_output=True, text=True, cwd=cwd)
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
    """
    Executa os testes SPHINCS+ em C (Apenas Local)
    """
    if IS_PRODUCTION:
        raise HTTPException(status_code=403, detail="Este endpoint está disponível apenas em ambiente local.")
    
    path = os.path.join("src", "c_tests")
    result = run_command(["./tests"], cwd=path)
    return JSONResponse(content=result)

@app.get("/run_attack/ghz")
def run_attack_ghz():
    path = os.path.join("src", "quantum_attacks")
    result = run_command(["python", "quantum_attack_ghz.py"], cwd=path)
    return JSONResponse(content=result)

@app.get("/run_attack/4qubits")
def run_attack_4qubits():
    path = os.path.join("src", "quantum_attacks")
    result = run_command(["python", "quantum_attack_4_qubits.py"], cwd=path)
    return JSONResponse(content=result)

@app.get("/run_attack/grover")
def run_attack_grover():
    path = os.path.join("src", "quantum_attacks")
    result = run_command(["python", "quantum_attack_grover.py"], cwd=path)
    return JSONResponse(content=result)

@app.get("/run_attack/all")
def run_all_attacks():
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

if __name__ == "__main__":
    port = int(os.environ.get("PORT", 8000))
    import uvicorn
    uvicorn.run("src.api.main:app", host="0.0.0.0", port=port)
