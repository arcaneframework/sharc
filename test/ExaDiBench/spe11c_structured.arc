<?xml version='1.0' encoding='ISO-8859-1'?>
<case codeversion="1.0" codename="ArcaneDemo" xml:lang="en">
	<arcane>
		<title>Experimentation Arcane</title>
		<timeloop>TimeLoop</timeloop>
	</arcane>
  <meshes>
  <mesh>
    <!-- <meshgenerator><simple><mode>1</mode></simple></meshgenerator> -->
    <!--file internal-partition='true'>cube3D1.vt2</file-->
    <!--filename>spe11c.msh</filename-->
    <filename>spe11c_structured.msh</filename>
    <!--filename>spe11c.vtk</filename-->
    <!--filename>sphere.vtk</filename-->
    <!--file internal-partition='true'>unit3dcube_coarse.vor</file-->
    <!-- <file internal-partition='true'>abordPuits-3D-gros-2.vt2</file> -->
  </mesh>
  </meshes>

  <arcane-demo>
    <post-processing>
      <output-period>1</output-period>
      <save-init>true</save-init>
      <post-processor>
        <format name="Ensight7PostProcessor">
          <binary-file>false</binary-file>
        </format>
        <variables>
          <variable>NodeCoord</variable>
          <variable>TestVolumes</variable>
          <variable>TestCenters</variable>
          <variable>LocalId</variable>
        </variables>
      </post-processor>
    </post-processing>
    <max-iteration>3</max-iteration>
  </arcane-demo>

  <geometry-test>
    <use-external-storage>true</use-external-storage>
    <factor>0.5</factor>
	  <geometry name="Euclidian3Geometry"/>
  </geometry-test>
</case>
