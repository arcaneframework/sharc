<?xml version='1.0' encoding='ISO-8859-1'?>
<case codeversion="1.0" codename="${codename}" xml:lang="en">

  <arcane>
    <title>Test the mesh partitioner</title>
    <timeloop>TestLoop</timeloop>
  </arcane>

  <mesh dual='true'>
    <file internal-partition='true'>maillage10x10x12.vt2</file>
    <partitioner>Ifpart</partitioner>
  </mesh>

  <!-- Service test manager -->
  <service-test-mng name="ServiceTestMng">
    <service name="IfpMeshPartitionerTester">
      <partitioner name="Ifpart"/>
    </service>
    <geometry-service name="Euclidian3Geometry"/>
  </service-test-mng>

  <!-- Internal debugger -->
  <internal-debugger>
   <command>Exec</command>
   <commandfile>gdb.cmd</commandfile>
   <autostart>true</autostart>
   <commandline>xterm -title 'process %myid' -geometry 200x40 -e gdb %name %pid </commandline>
  </internal-debugger>
</case>
 
