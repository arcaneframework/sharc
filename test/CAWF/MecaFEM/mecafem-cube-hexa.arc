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

  <meshes>
    <mesh>
      <filename>meshes/cube.hexa.msh</filename>
      <subdivider>
        <nb-subdivision>2</nb-subdivision>
      </subdivider>
    </mesh>
  </meshes>


  <fem>
    <hex-quad-mesh>true</hex-quad-mesh>
    <E>200e9</E>
    <nu>0.3</nu>
    <f>-9.8e12, -7.5e12, 5.9e12</f>
    <boundary-conditions>
      <dirichlet>
        <surface>side</surface>
        <value>NULL 0. NULL</value>
      </dirichlet>
      <dirichlet>
        <surface>left</surface>
        <value>0. NULL NULL</value>
      </dirichlet>
      <dirichlet>
        <surface>right</surface>
        <value>0. NULL NULL</value>
      </dirichlet>
      <dirichlet>
        <surface>bot</surface>
        <value>0. 0. 0.</value>
      </dirichlet>
    </boundary-conditions>

    <max-iter>1</max-iter>
    <matrix-format>Alien-BSR</matrix-format>
    <linear-system name="AlienLinearSystem">
      <use-accelerator>false</use-accelerator>
      <linear-solver name="AlienCoreSolver">
        <backend>SimpleCSR</backend>
        <solver>BCGS</solver>
        <preconditioner>Diag</preconditioner>
        <max-iter>1000</max-iter>
        <tol>1.e-6</tol>
        <output-level>1</output-level>
      </linear-solver>
    </linear-system>
    <dynamic-mesh-mng name="DynamicMeshMng">
        <mesh>mesh/cube</mesh>
        <format>vt2</format>
        <first-event-id>1</first-event-id>
    </dynamic-mesh-mng>
    <geometry-mng name="Euclidian3Geometry"/>
  </fem>
</case>
