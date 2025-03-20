<?xml version='1.0' encoding='ISO-8859-1'?>
<case codeversion="1.0" codename="${codename}" xml:lang="en">
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
          <preconditioner name="BlockILU">
          </preconditioner>
        </solver>
       <verbose>high</verbose>
      </linear-solver>

       <!--linear-solver name="GPUSolver">
           <normalize-opt>true</normalize-opt>
           <keep-diag-opt>false</keep-diag-opt>
           <kernel>MCKernel</kernel>
           <max-iteration-num>1000</max-iteration-num>
           <stop-criteria-value>1e-10</stop-criteria-value>
           <preconditioner>ILU0</preconditioner>
       </linear-solver-->
                                                                         
       <linear-solver name="MCGSolver">
           <!--normalize-opt>true</normalize-opt>
           <keep-diag-opt>false</keep-diag-opt-->
           <kernel>CPU_CBLAS_BCSR</kernel>
           <max-iteration-num>1000</max-iteration-num>
           <stop-criteria-value>1e-10</stop-criteria-value>
           <preconditioner>None</preconditioner>
       </linear-solver>

    </service>
  </service-test-mng>
</case>
