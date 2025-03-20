<?xml version='1.0' encoding='ISO-8859-1'?>
<case codeversion="1.0" codename="${codename}" xml:lang="en">
  <arcane>
    <title>Test unitaires des solveurs lineaires</title>
    <timeloop>TestLoop</timeloop>
  </arcane>

  <arcane-post-processing>
    <output-period>1</output-period>
    <output>
      <variable>P</variable>
      <variable>T</variable> 
      <variable>S1</variable>
      <variable>S2</variable>
      <variable>Split</variable>
    </output>
  </arcane-post-processing>

  <mesh>
    <file internal-partition='true'>cube3D.vt2</file>
  </mesh>

  <service-test-mng name="ServiceTestMng">
    
    <service name="LinearAlgebra2VBlockTester">
      <!-- big diagonal-coefficient keep diagonal dominant matrix -->
      <diagonal-coefficient>10</diagonal-coefficient>
      <!--normalise>false</normalise>
          <block-size>2</block-size-->
      
      <linear-solver name="PETScSolver">
        <solver name="BiCGStab">
          <num-iterations-max>1000</num-iterations-max>
          <stop-criteria-value>1e-8</stop-criteria-value>
          <preconditioner name="FieldSplit">
            <verbose>true</verbose>
            <type name="Multiplicative"/>
            <block>
              <tag>Elliptic</tag>
              <solver name="Richardson">
                <init-type>Zero</init-type>
                <stop-criteria-value>1e-5</stop-criteria-value>
                <preconditioner name="Hypre">
                  <type>AMG</type>
                </preconditioner>
                <num-iterations-max>3</num-iterations-max>
              </solver>
            </block>
            <block>
              <tag>P</tag>
              <solver name="Richardson">
                <init-type>Zero</init-type>
                <stop-criteria-value>1e-5</stop-criteria-value>
                <preconditioner name="BlockILU"/>
                <num-iterations-max>3</num-iterations-max>
              </solver>
            </block>
          </preconditioner>
        </solver>
        <verbose>high</verbose>
      </linear-solver>

      <linear-solver name="PETScSolver">
        <verbose>low</verbose>
        <solver name="SuperLU" />
      </linear-solver>

<!--
      <linear-solver name="PETScSolver">
        <verbose>high</verbose>
        <solver name="BiCGStab">
          <num-iterations-max>1000</num-iterations-max>
          <stop-criteria-value>1e-8</stop-criteria-value>
          <preconditioner name="SuperLU">
            <fill-factor>1.0</fill-factor>
          </preconditioner>
        </solver>
      </linear-solver>
-->
 
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
