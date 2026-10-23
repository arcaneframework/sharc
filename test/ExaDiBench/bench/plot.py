#!/usr/bin/env python3
import os

import matplotlib.pyplot as plt
import pandas as pd

# dossier des résultats : results.csv en entrée, steps.png en sortie
RESULTS_DIR = os.path.join(os.path.dirname(os.path.abspath(__file__)), "results")

FORMATS = ["meshb", "msh"]  # meshb à gauche, msh à droite
# étapes empilées : colonne du csv -> (légende, couleur)
STEPS = {"read_s": ("lecture", "#4c6a92"), "convert_s": ("conversion", "#9ab0cc"),
         "build_s": ("construction", "#d9a441"), "partition_s": ("partition", "#b04512")}

plt.rcParams.update({
    "figure.facecolor": "#fcfcfb", "axes.facecolor": "#fcfcfb",
    "axes.titlelocation": "left", "axes.titlesize": 12, "axes.titlecolor": "#0b0b0b",
    "axes.labelcolor": "#52514e", "xtick.color": "#52514e", "ytick.color": "#52514e",
    "axes.edgecolor": "#b5b4ad", "axes.spines.top": False, "axes.spines.right": False,
    "axes.grid": True, "axes.grid.axis": "y", "axes.axisbelow": True,
    "grid.color": "#e5e4df", "grid.linewidth": 0.8,
})

# médiane des répétitions, par maillage, format et nb de rangs (runs plantés ignorés)
df = pd.read_csv(os.path.join(RESULTS_DIR, "results.csv"))
df = df[df.total_s != "FAILED"]
df[["name", "fmt"]] = df["mesh"].str.rsplit(".", n=1, expand=True)
med = df.astype({s: float for s in STEPS}).groupby(["name", "fmt", "nproc"])[list(STEPS)].median()
meshes = df["name"].unique()  # dans l'ordre de bench.sh

nrows = (len(meshes) + 1) // 2
fig, axes = plt.subplots(nrows, 2, figsize=(10, 4 * nrows), squeeze=False)
for ax in axes.flat[len(meshes):]:
    ax.set_visible(False)
width = 0.38
for ax, mesh in zip(axes.flat, meshes):
    ax.set_title(mesh.replace("unit_", ""))
    ax.set_ylabel("temps (s)")
    nprocs = sorted(med.loc[mesh].index.unique("nproc"))
    for i_fmt, fmt in enumerate(FORMATS):
        x = [i + (i_fmt - 0.5) * width for i in range(len(nprocs))]
        bottom = 0
        for step, (label, color) in STEPS.items():
            values = med.loc[(mesh, fmt), step].loc[nprocs].values
            ax.bar(x, values, width * 0.9, bottom=bottom, color=color, label=label if i_fmt == 0 else None)
            bottom = bottom + values
    ax.set_xticks([i + d * width for i in range(len(nprocs)) for d in (-0.5, 0.5)],
                  FORMATS * len(nprocs), fontsize=8)
    for i, n in enumerate(nprocs):
        ax.text(i, -0.13, f"{n} rangs", transform=ax.get_xaxis_transform(),
                ha="center", va="top", fontsize=9, color="#52514e")

fig.legend(*axes[0, 0].get_legend_handles_labels(), loc="upper center", ncol=4,
           frameon=False, fontsize=9, handlelength=4)
fig.tight_layout(rect=(0, 0.02, 1, 0.96))
fig.savefig(os.path.join(RESULTS_DIR, "steps.png"), dpi=130)
