<?xml version='1.0' encoding='ISO-8859-1'?>
<case codeversion="1.0" codename="${codename}" xml:lang="en">
  <arcane>
    <title>Test unitaires des solveurs lineaires</title>
    <timeloop>TestLoop</timeloop>
  </arcane>

  <arcane-post-processing>
    <output-period>1</output-period>
    <output>
      <variable>Unknown</variable>
      <variable>Residual</variable>
    </output>
  </arcane-post-processing>

  <mesh>
    <file internal-partition='true'>cube3D20.vt2</file>
  </mesh>

  <service-test-mng name="ServiceTestMng">

    <service name="LinearSolverTester">
      <!-- big diagonal-coefficient keep diagonal dominant matrix -->
      <diagonal-coefficient>29</diagonal-coefficient>
      <stencil-by>node</stencil-by>
      <builder>BlockInserter</builder>
      <block-size>3</block-size>

      <check-memory>false</check-memory>
      <!-- <building-only>true</building-only> -->


      <!-- -3- example using PETSc ILU preconditioner -->
      <linear-solver name="PETScSolver">
        <solver name="BiCGStab">
          <num-iterations-max>1000</num-iterations-max>
          <stop-criteria-value>1e-10</stop-criteria-value>
          <preconditioner name="ILU">
            <level>2</level>
          </preconditioner>
        </solver>
        <verbose>high</verbose>
      </linear-solver>
      
      <linear-solver name="GPUSolver">
        <unit-test>false</unit-test>
        <matrix-file-name>TestMatrix.txt</matrix-file-name>
        <max-iteration-num>1000</max-iteration-num>
        <stop-criteria-value>1e-10</stop-criteria-value>
      </linear-solver>


    </service>
  </service-test-mng>
</case>
