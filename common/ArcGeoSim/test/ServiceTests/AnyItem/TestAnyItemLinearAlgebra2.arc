<?xml version='1.0' encoding='UTF-8'?>
<case codename='${codename}' xml:lang='en' codeversion='1.0'>
  <arcane>
    <title>Test unitaires des solveurs lineaires avec AnyItem</title>
    <timeloop>TestLoop</timeloop>
  </arcane>
  <mesh>
    <file internal-partition="true">cube3D1.vt2</file>
  </mesh>

  <arcane-post-processing>
    <output-period>1</output-period>
    <format name="Ensight7PostProcessor">
      <binary-file>false</binary-file>
    </format>
    <output>
      <variable>T</variable>
      <variable>P</variable>
      <group>AllCells</group>
      <group>AllFaces</group>
    </output>
  </arcane-post-processing>

  <service-test-mng name="ServiceTestMng">
    <geometry-service name="Euclidian3Geometry"/>
    <service name="AnyItemLinearAlgebra2Tester">
      <!-- geometry-mng name='Euclidian3Geometry'/ -->
      <linear-solver name='PETScSolver'>
        <solver name='BiCGStab'>
          <num-iterations-max>40</num-iterations-max>
          <stop-criteria-value>1e-6</stop-criteria-value>
          <preconditioner name='ILU'>
            <level>0</level>
            <shift>1.e-10</shift>
          </preconditioner>
          <right>true</right>
          <init-type>Zero</init-type>
        </solver>
        <verbose>low</verbose>
      </linear-solver>
      <submesh-linear-solver name='PETScSolver'>
        <solver name='BiCGStab'>
          <num-iterations-max>40</num-iterations-max>
          <stop-criteria-value>1e-6</stop-criteria-value>
          <preconditioner name='ILU'>
            <level>0</level>
            <shift>1.e-10</shift>
          </preconditioner>
          <right>true</right>
          <init-type>Zero</init-type>
        </solver>
        <verbose>low</verbose>
      </submesh-linear-solver>
    </service>
  </service-test-mng>
</case>
