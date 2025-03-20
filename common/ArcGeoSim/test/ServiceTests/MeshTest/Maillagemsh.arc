<?xml version='1.0' encoding='ISO-8859-1'?>
<case codeversion="1.0" codename="CooresArcane" xml:lang="en">

  <arcane>
    <title>Verification Maillage</title>
    <timeloop>MeshTestDebugLoop</timeloop>
  </arcane>

  <mesh dual='true'>
    <file internal-partition='true'>meshes/cube27_1111_grdecl.msh</file>
  </mesh>

  <arcane-post-processing>
    <format name="Ensight7PostProcessor">
    <binary-file>false</binary-file>
     </format>
    <output-period>1</output-period>
    <output>
      <variable>CellVolume</variable>
      <variable>CellPermx</variable>
      <variable>CellGroup</variable>
    </output>
  </arcane-post-processing>

  <!-- ***** Coores service manager ***** -->

  <coores-service-mng name="MeshServiceMng">

     <geometry-service name="StaticGeometry"/>
    
  </coores-service-mng>



  <!-- ***** Mesh test manager ***** -->
  <mesh-test-mng name = "MeshTestMng">
    <group_property>Well2</group_property>
    <scalar_property>CellPorosity</scalar_property>
   </mesh-test-mng>
 
</case>
