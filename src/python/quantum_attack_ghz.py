
from qiskit import QuantumCircuit, transpile
from qiskit_aer import Aer

sim = Aer.get_backend('qasm_simulator')
qc = QuantumCircuit(3, 3)
qc.h(0)
qc.cx(0, 1)
qc.cx(0, 2)
qc.measure([0, 1, 2], [0, 1, 2])
result = sim.run(transpile(qc, sim)).result()
print(result.get_counts(qc))
