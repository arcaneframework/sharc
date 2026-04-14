<?xml version='1.0' encoding='ISO-8859-1'?>
<case codeversion="1.0" codename="ShArc" xml:lang="en">
  <arcane>
    <title>Test unitaires des solveurs lineaires</title>
    <timeloop>CAWFLoop</timeloop>
  </arcane>

  <arcane-post-processing>
    <output-period>1</output-period>
    <output>
      <variable>DivU</variable>
      <variable>LayerId</variable>
      <variable>Rho</variable>
      <variable>DRhoDP</variable>
    </output>
  </arcane-post-processing>

  <mesh>
    <file internal-partition='true'>../meshes/cube2.vt2</file>
  </mesh>


    <compaction>
      <bottom>ZMIN</bottom>
      <top>ZMAX</top>
      <front>YMIN</front>
      <back>YMAX</back>
      <left>XMIN</left>
      <right>XMAX</right>

      <event-period>1</event-period>
      <max-iter>4</max-iter>
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

      <dynamic-mesh-mng name="DynamicMeshMng">
        <mesh>../meshes/cube</mesh>
        <format>vt2</format>
        <first-event-id>2</first-event-id>
        <bottom>ZMIN</bottom>
        <top>ZMAX</top>
        <front>YMIN</front>
        <back>YMAX</back>
        <left>XMIN</left>
        <right>XMAX</right>
      </dynamic-mesh-mng>

      <cawf-mng name="PreCICE">
          <app-id>0</app-id>
          <app-name>Compaction</app-name>
          <config-file>../precice-config-evol-v3.xml</config-file>
          <mesh-name>Mesh1</mesh-name>
          <read-data>
              <name>DivU</name>
              <var-name>divu</var-name>
          </read-data>
          <write-data>
            <name>P</name>
            <var-name>P</var-name>
          </write-data>
      </cawf-mng>

  </compaction>
</case>
