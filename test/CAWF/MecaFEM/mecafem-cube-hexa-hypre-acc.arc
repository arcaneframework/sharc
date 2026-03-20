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
        <nb-subdivision>4</nb-subdivision>
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
    <matrix-format>Alien-BSR-Acc</matrix-format>
    <!--linear-system name="HypreLinearSystem">
      <solver>bicgstab</solver>
      <rtol>1e-9</rtol>
      <atol>0.</atol>
    </linear-system-->
    <linear-system name="AlienLinearSystem">
      <use-accelerator>true</use-accelerator>
      <linear-solver name="HypreSolver">
        <exec-space>Device</exec-space>
        <memory-type>Device</memory-type>
        <solver>BiCGStab</solver>
        <num-iterations-max>1000</num-iterations-max>
        <stop-criteria-value>1e-6</stop-criteria-value>
        <preconditioner>BJ-ILUK</preconditioner>
        <verbose>true</verbose>
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
