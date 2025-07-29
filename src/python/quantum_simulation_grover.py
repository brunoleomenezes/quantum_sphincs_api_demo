
from qiskit import QuantumCircuit, transpile
from qiskit_aer import Aer

sim = Aer.get_backend('qasm_simulator')
qc = QuantumCircuit(2, 2)
qc.h([0, 1])
qc.cz(0, 1)
qc.h([0, 1])
qc.x([0, 1])
qc.h(1)
qc.cx(0, 1)
qc.h(1)
qc.x([0, 1])
qc.h([0, 1])
qc.measure([0, 1], [0, 1])
result = sim.run(transpile(qc, sim)).result()
print(result.get_counts(qc))
