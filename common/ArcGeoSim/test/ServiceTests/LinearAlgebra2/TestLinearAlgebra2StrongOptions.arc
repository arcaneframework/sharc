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
  
    <service name="LinearAlgebra2StrongOptionsTester">
      <!-- big diagonal-coefficient keep diagonal dominant matrix -->
      <diagonal-coefficient>24</diagonal-coefficient>
      <normalise>false</normalise>
      <block-size>2</block-size>
      <!-- l idee est que ca ne converge pas dans un premier temps -->
	  <solver-parameters>
          <num-iterations-max>30</num-iterations-max>
          <stop-criteria-value>1e-10</stop-criteria-value>
          <verbose>high</verbose>
      </solver-parameters>
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
