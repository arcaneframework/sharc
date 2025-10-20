<?xml version='1.0' encoding='ISO-8859-1'?>
<case codeversion="1.0" codename="ShArc" xml:lang="en">
	<arcane>
		<title>Experimentation Arcane</title>
		<timeloop>TimeLoop</timeloop>
	</arcane>
  <mesh>
    <file internal-partition='false'>unit_cube_1k.meshb</file>
  </mesh>

  <sharc>
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
  </sharc>

  <exa-di-bench>
    <use-external-storage>true</use-external-storage>
    <factor>0.5</factor>
	  <geometry name="Euclidian3Geometry"/>
  </exa-di-bench>
</case>
