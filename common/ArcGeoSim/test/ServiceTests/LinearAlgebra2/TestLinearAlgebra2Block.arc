<?xml version='1.0' encoding='ISO-8859-1'?>
<case codeversion="1.0" codename="${codename}" xml:lang="en">
  <arcane>
    <title>Test unitaires des solveurs lineaires</title>
    <timeloop>TestLoop</timeloop>
  </arcane>

  <mesh>
    <file internal-partition='true'>cube3D.vt2</file>
  </mesh>

  <service-test-mng name="ServiceTestMng">

    <service name="LinearAlgebra2BlockTester">
      <!-- big diagonal-coefficient keep diagonal dominant matrix -->
      <diagonal-coefficient>24</diagonal-coefficient>
      <normalise>false</normalise>
      <block-size>2</block-size>

      <linear-solver name="PETScSolver">
        <solver name="BiCGStab">
          <num-iterations-max>1000</num-iterations-max>
          <stop-criteria-value>1e-8</stop-criteria-value>
          <preconditioner name="BlockILU">
          </preconditioner>
        </solver>
       <verbose>high</verbose>
      </linear-solver>

      <linear-solver name="IFPSolver">
        <verbose>true</verbose>
        <num-iterations-max>1000</num-iterations-max>
        <stop-criteria-value>1e-12</stop-criteria-value>
        <precond-option>ILU0</precond-option>
      </linear-solver>

      <linear-solver name="MTLSolver">
      	<solver>BiCGStab</solver>
      </linear-solver>

    </service>
  </service-test-mng>
  <appli-service-mng>

    <units-system name="UnitsSystem"/>

    <geometry-service name="Euclidian3Geometry">
    </geometry-service>

    <!-- exp-parser name = "ExpressionParser" /-->
    <expression-mng name="ExpressionMng" />

  </appli-service-mng>

</case>
