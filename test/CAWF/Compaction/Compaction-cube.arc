<?xml version='1.0' encoding='ISO-8859-1'?>
<case codeversion="1.0" codename="ShArc" xml:lang="en">
  <arcane>
    <title>Test unitaires des solveurs lineaires</title>
    <timeloop>CAWFLoop</timeloop>
  </arcane>

  <arcane-post-processing>
    <output-period>1</output-period>
    <output>
      <variable>U</variable>
      <variable>C</variable>
      <variable>LayerId</variable>
      <variable>Rho</variable>
      <variable>DRhoDP</variable>
    </output>
  </arcane-post-processing>

  <mesh>
    <file internal-partition='true'>cube1.vt2</file>
  </mesh>


    <compaction>
      <dynamic-mesh-mng name="DynamicMeshMng">
        <mesh>cube</mesh>
        <format>vt2</format>
        <first-event-id>1</first-event-id>
      </dynamic-mesh-mng>

      <event-period>1</event-period>
      <max-iter>4</max-iter>
      <!--cawf-mng name="PreCICE">
          <app-id>0</app-id>
          <app-name>App1</app-name>
          <config-file>../precice-config.xml</config-file>
          <mesh-name>Mesh1</mesh-name>
          <read-data>
              <name>C</name>
              <var-name>C</var-name>
          </read-data>
          <write-data>
            <name>P</name>
            <var-name>U</var-name>
          </write-data>
      </cawf-mng-->

      <!--linear-solver name="PETScSolver">
        <solver name="BiCGStab">
          <num-iterations-max>1000</num-iterations-max>
          <stop-criteria-value>1e-8</stop-criteria-value>
          <preconditioner name="BlockILU">
          </preconditioner>
        </solver>
       <verbose>high</verbose>
      </linear-solver-->
      <surface-density>1.</surface-density>
      <surface-compressibility>0.</surface-compressibility>
      <geometry-mng name="Euclidian3Geometry"/>
      <!--linear-solver name="HypreSolver">
        <solver>BiCGStab</solver>
        <num-iterations-max>100</num-iterations-max>
        <stop-criteria-value>1e-5</stop-criteria-value>
        <preconditioner>BJ-ILUK</preconditioner>
        <verbose>true</verbose>
      </linear-solver-->
      <linear-solver name="AlienCoreSolver">
        <backend>SimpleCSR</backend>
        <solver>BCGS</solver>
        <preconditioner>Diag</preconditioner>
        <max-iter>1000</max-iter>
        <tol>1.e-6</tol>
        <output-level>1</output-level>
      </linear-solver>
  </compaction>
</case>
