import matplotlib.pyplot as plt
from pathlib import Path

# CFR convergence results
iterations = [
    1,
    10,
    100,
    1_000,
    10_000,
    100_000,
    1_000_000
]

exploitability = [
    0.458333,
    0.096209,
    0.025675,
    0.007269,
    0.002318,
    0.000676,
    0.000219
]

# Create graph
plt.figure(figsize=(8, 5))

plt.loglog(
    iterations,
    exploitability,
    marker="o"
)

plt.xlabel("CFR Iterations")
plt.ylabel("Exploitability (antes/hand)")
plt.title("CFR Exploitability Convergence")
plt.grid(True, which="both", linestyle="--", alpha=0.5)

plt.tight_layout()

# Save relative to the project root, regardless of where script is run
project_root = Path(__file__).resolve().parent.parent
output_dir = project_root / "docs"
output_dir.mkdir(exist_ok=True)

output_path = output_dir / "exploitability.png"
plt.savefig(output_path, dpi=300)

print(f"Saved convergence plot to {output_path}")