<?xml version="1.0"?>
<case codename="ShArc" xml:lang="en" codeversion="1.0">
  <arcane>
    <title>Sample</title>
    <timeloop>ElasticityLoop</timeloop>
  </arcane>

  <arcane-post-processing>
   <output-period>1</output-period>
   <output>
     <variable>U</variable>
     <variable>CellDivU</variable>
     <variable>CellPressure</variable>
   </output>
  </arcane-post-processing>

  <mesh>
    <file internal-partition="true">../meshes/cube2.vt2</file>
  </mesh>

  <fem>
    <hex-quad-mesh>true</hex-quad-mesh>
    <E>200e9</E>
    <nu>0.3</nu>
    <f>-9.8e12, -7.5e12, 5.9e12</f>

    <bottom>ZMIN</bottom>
    <top>ZMAX</top>
    <front>YMIN</front>
    <back>YMAX</back>
    <left>XMIN</left>
    <right>XMAX</right>
    <boundary-conditions>
      <dirichlet>
        <surface>YMIN</surface>
        <value>NULL 0. NULL</value>
      </dirichlet>
      <dirichlet>
        <surface>YMAX</surface>
        <value>NULL 0. NULL</value>
      </dirichlet>
      <dirichlet>
        <surface>XMIN</surface>
        <value>0. NULL NULL</value>
      </dirichlet>
      <dirichlet>
        <surface>XMAX</surface>
        <value>0. NULL NULL</value>
      </dirichlet>
      <dirichlet>
        <surface>ZMIN</surface>
        <value>0. 0. 0.</value>
      </dirichlet>
    </boundary-conditions>

    <event-period>1</event-period>
    <max-iter>0</max-iter>

    <matrix-format>Alien-BSR</matrix-format>
    <linear-system name="AlienLinearSystem">
      <linear-solver name="AlienCoreSolver">
        <backend>SimpleCSR</backend>
        <solver>BCGS</solver>
        <preconditioner>Diag</preconditioner>
        <max-iter>1000</max-iter>
        <tol>1.e-6</tol>
        <output-level>1</output-level>
      </linear-solver>
    </linear-system>
    <!--dynamic-mesh-mng name="DynamicMeshMng">
        <mesh>meshes/cube</mesh>
        <format>vt2</format>
        <first-event-id>1</first-event-id>
        <bottom>ZMIN</bottom>
        <top>ZMAX</top>
        <front>YMIN</front>
        <back>YMAX</back>
        <left>XMIN</left>
        <right>XMAX</right>
    </dynamic-mesh-mng-->
    <geometry-mng name="Euclidian3Geometry"/>
  </fem>
</case>
