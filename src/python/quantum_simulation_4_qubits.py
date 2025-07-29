
from qiskit import QuantumCircuit, transpile
from qiskit_aer import Aer

sim = Aer.get_backend('qasm_simulator')
qc = QuantumCircuit(4, 4)
qc.h(0)
qc.h(1)
qc.cx(0, 2)
qc.cx(1, 3)
qc.swap(2, 3)
qc.x(0)
qc.h(2)
qc.measure([0, 1, 2, 3], [0, 1, 2, 3])
result = sim.run(transpile(qc, sim)).result()
print(result.get_counts(qc))
