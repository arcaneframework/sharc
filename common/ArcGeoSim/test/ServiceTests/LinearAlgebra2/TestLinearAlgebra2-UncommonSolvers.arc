<?xml version='1.0' encoding='ISO-8859-1'?>
<case codeversion="1.0" codename="ArcaneDemo" xml:lang="en">
  <arcane>
    <title>Test unitaires des solveurs lineaires</title>
    <timeloop>TestLoop</timeloop>
  </arcane>

  <arcane-post-processing>
    <output-period>1</output-period>
    <output>
      <variable>U</variable>
    </output>
  </arcane-post-processing>

  <mesh>
    <file internal-partition='true'>cube3D.vt2</file>
  </mesh>

  <service-test-mng name="ServiceTestMng">

    <service name="LinearAlgebra2Tester">
      <!-- big diagonal-coefficient keep diagonal dominant matrix -->
      <diagonal-coefficient>24</diagonal-coefficient>
      <stencil-by>node</stencil-by>

      <check-memory>false</check-memory>
      <building-only>false</building-only>

      <repeat-loop>4</repeat-loop>
      <extra-equation-count>10</extra-equation-count>

      <builder>ProfiledBuilder</builder> 
      <builder>DirectBuilder</builder>
      <builder>StreamBuilder</builder>

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
                  <field-split-mode>true</field-split-mode>
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

<!--
      <linear-solver name="MTLSolver">
      	<solver>LU</solver>
      </linear-solver>
-->

      <linear-solver name="PETScSolver">
        <verbose>high</verbose>
        <solver name="Custom"> 
          <option><name>ksp_type</name><value>tcqmr</value></option>
          <option><name>ksp_rtol</name><value>1e-6</value></option>
          <option><name>pc_type</name><value>sor</value></option>
        </solver>
      </linear-solver>

      <!-- GMRES solver preconditioned by a BiCGStab solver with 1e-2 tolerance -->
      <linear-solver name="PETScSolver">
        <solver name="Custom">
          <option><name>ksp_type</name><value>gmres</value></option>
          <option><name>ksp_rtol</name><value>1e-6</value></option>
          <option><name>pc_type</name><value>ksp</value></option>
          <option><name>ksp_ksp_type</name><value>bicg</value></option>
          <option><name>ksp_ksp_rtol</name><value>1e-2</value></option>
        </solver>
      </linear-solver>

    </service>
  </service-test-mng>
</case>
