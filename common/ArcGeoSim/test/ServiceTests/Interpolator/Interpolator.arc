<?xml version='1.0' encoding='ISO-8859-1'?>
<case codeversion="1.0" codename="${codename}" xml:lang="en">

  <arcane>
    <title>Test the dense LU solver</title>
    <timeloop>TestLoop</timeloop>
  </arcane>

  <mesh dual='true'>
    <file internal-partition='true'>meshes/cube4x4x4.vt2</file>
  </mesh>

  <!-- Service test manager -->
  <service-test-mng name="ServiceTestMng">

    <!-- Geometry service -->
    <geometry-service name="Euclidian3Geometry"/>

    <service name="InterpolatorTester">
      <!-- The interpolator to be tested -->
      <interpolator name="GaussInterpolator"/>

      <!-- The continuous field -->
      <continuous-field>
        <x name="ExpressionBuilderR3vR1">
         <expression>(x,y,z)->x^2</expression>
         </x>
        <y name="ExpressionBuilderR3vR1">
         <expression>(x,y,z)->sin(3.141563*x)*sin(3.141563*y)</expression>
         </y>
        <z name="ExpressionBuilderR3vR1">
         <expression>(x,y,z)->y^2</expression>
         </z>
      </continuous-field>
    </service>
  </service-test-mng>

  <!-- Internal debugger -->
  <internal-debugger>
   <command>Exec</command>
   <commandfile>gdb.cmd</commandfile>
   <autostart>true</autostart>
   <commandline>xterm -title 'process %myid' -geometry 200x40 -e gdb %name %pid </commandline>
  </internal-debugger>
</case>
