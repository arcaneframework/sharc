#!/usr/bin/env bash

set -uo pipefail

# Maillages à benchmarker, par paires : chaque meshb est suivi de son msh

MESH_FILES=(
  unit_cube_tet_28k.meshb
  unit_cube_tet_28k.msh
  unit_cube_hex_28k.meshb
  unit_cube_hex_28k.msh
  unit_cube_tet_212k.meshb
  unit_cube_tet_212k.msh
  unit_cube_hex_212k.meshb
  unit_cube_hex_212k.msh
)

NPROCS=(2 3 4 5 6)

REPEATS=3


BENCH_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
MESHES_DIR="$(cd "${BENCH_DIR}/../meshes" && pwd)"
SHARC_EXE="$(cd "${BENCH_DIR}/../../../build" && pwd)/bin/ShArc.exe"
TEMPLATE_ARC="${BENCH_DIR}/template.arc"
TEMPLATE_MSH_ARC="${BENCH_DIR}/template_msh.arc"
RESULTS_DIR="${BENCH_DIR}/results"
LOGS_DIR="${RESULTS_DIR}/logs"
CSV_FILE="${RESULTS_DIR}/results.csv"

mkdir -p "$LOGS_DIR"

# timestat <action> <Computation|Communication> : dernière valeur total_Real (action absente = 0)
timestat() {
  grep -a "type=$2 action=$1 " "$log_file" | tail -1 | grep -o "total_Real=[^ ]*" | cut -d= -f2
}
# somme de nombres à virgule
add() {
  echo "$@" | awk '{s = 0; for (i = 1; i <= NF; i++) s += $i; print s}'
}
# temps d'une action : calcul + communication
tot()  { add $(timestat $1 Computation) $(timestat $1 Communication); }
# seulement le calcul, ou seulement la communication
comp() { timestat $1 Computation; }
comm() { timestat $1 Communication; }

if [[ ! -f "$SHARC_EXE" ]]; then
  echo "ShArc.exe not present: $SHARC_EXE" >&2
  exit 1
fi

# vérifie que tous les maillages sont présents
missing=0
for mesh in "${MESH_FILES[@]}"; do
  if [[ ! -e "$MESHES_DIR/$mesh" ]]; then
    echo "Mesh not found: $MESHES_DIR/$mesh" >&2
    missing=1
  fi
done
if [[ $missing -eq 1 ]]; then
  exit 1
fi

echo "mesh,nproc,partition,rep,read_s,convert_s,build_s,partition_s,msh_only_s,ready_s,geometry_s,total_s" > "$CSV_FILE"

for rep in $(seq 1 $REPEATS); do
  for nproc in "${NPROCS[@]}"; do
    for mesh in "${MESH_FILES[@]}"; do

        # meshb : partition Ouranos (false) / msh : partition Arcane (true)
        if [[ $mesh == *.msh ]]; then
          mode=true
          template="$TEMPLATE_MSH_ARC"
        else
          mode=false
          template="$TEMPLATE_ARC"
        fi

        name="${mesh//./_}_np${nproc}_${mode}_rep${rep}"
        arc_file="$RESULTS_DIR/$name.arc"
        log_file="$LOGS_DIR/$name.log"

        # prépare le .arc à partir du template
        sed -e "s#__MESHFILE__#meshes/$mesh#" -e "s/__PARTITION__/$mode/" "$template" > "$arc_file"

        # un rang par coeur, lancé depuis test/ExaDiBench (où se trouve ShArc.config)
        cd "$BENCH_DIR/.."
        mpirun --bind-to core -n $nproc "$SHARC_EXE" "$arc_file" > "$log_file" 2>&1
        cd "$BENCH_DIR"

        # timers de chaque étape (en secondes, rang 0)
        if grep -q "STOP PERFECT" "$log_file"; then
          # lecture : fichier -> chaque rang a sa tranche brute
          read_s=$(add $(tot MeshbRead) $(tot MshRead))
          # conversion (msh : calcul seul, sa communication est comptée dans la partition)
          convert_s=$(add $(tot MeshbConvert) $(comp MshConvert))
          # construction : cells_infos -> maillage Arcane construit, avec ses coordonnées
          build_s=$(add $(tot MeshbBuild) $(tot MshBuild))
          # propre au msh : répartition provisoire + diffusion des mailles
          msh_only_s=$(add $(tot MshNodesPartition) $(comm MshConvert))
          # partition : meshb = Ouranos / msh = répartition provisoire + partition et échange Arcane
          partition_s=$(add $(tot MeshbPartition) $msh_only_s $(tot MeshPartition) $(tot MeshExchange))
          # maillage prêt = somme des étapes
          ready_s=$(add $read_s $convert_s $build_s $partition_s)
          # calcul : geometry()->update() sur toutes les itérations
          geometry_s=$(tot GeometryUpdate)
          # run complet (TotalReel, init + boucle)
          total_s=$(awk '/TotalReel/ {print $4}' "$log_file")
          line="$read_s,$convert_s,$build_s,$partition_s,$msh_only_s,$ready_s,$geometry_s,$total_s"
        else
          ready_s="FAILED"
          total_s="FAILED"
          line="FAILED,FAILED,FAILED,FAILED,FAILED,FAILED,FAILED,FAILED"
        fi

        echo "$name : ready=$ready_s total=$total_s"
        echo "$mesh,$nproc,$mode,$rep,$line" >> "$CSV_FILE"
    done
  done
done

echo
echo "Done : ${CSV_FILE}"
echo "Logs in : ${LOGS_DIR}"
echo
column -t -s, "$CSV_FILE"
