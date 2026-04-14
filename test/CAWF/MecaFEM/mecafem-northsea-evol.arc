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
    <file internal-partition="true">../meshes/north_sea_evol_mesh2.msh</file>
  </mesh>

  <fem>
    <hex-quad-mesh>true</hex-quad-mesh>
    <E>200e9</E>
    <nu>0.3</nu>
    <f>-9.8e12, -7.5e12, 5.9e12</f>

    <bottom>BottomBoundary</bottom>
    <top>TopBoundary</top>
    <border>BorderBoundary</border>
    <boundary-conditions>
      <dirichlet>
        <surface>BorderBoundary</surface>
        <value>0.</value>
      </dirichlet>
      <dirichlet>
        <surface>BottomBoundary</surface>
        <value>0. 0. 0.</value>
      </dirichlet>
    </boundary-conditions>

    <event-period>1</event-period>
    <max-iter>20</max-iter>

    <matrix-format>Alien-BSR</matrix-format>
    <linear-system name="AlienLinearSystem">
      <linear-solver name="AlienCoreSolver">
        <backend>SimpleCSR</backend>
        <solver>BCGS</solver>
        <preconditioner>ILU0</preconditioner>
        <max-iter>1000</max-iter>
        <tol>1.e-6</tol>
        <output-level>1</output-level>
      </linear-solver>
    </linear-system>
    <dynamic-mesh-mng name="DynamicMeshMng">
        <mesh>../meshes/north_sea_evol_mesh</mesh>
        <format>msh</format>
        <first-event-id>2</first-event-id>
        <bottom>BottomBoundary</bottom>
        <top>TopBoundary</top>
        <border>BorderBoundary</border>
    </dynamic-mesh-mng>
    <geometry-mng name="Euclidian3Geometry"/>
  </fem>
</case>
