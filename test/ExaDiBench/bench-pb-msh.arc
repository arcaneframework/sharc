<?xml version='1.0' encoding='ISO-8859-1'?>
<case codeversion="1.0" codename="ShArc" xml:lang="en">
  <arcane>
	<title>Experimentation Arcane</title>
	<timeloop>TimeLoop</timeloop>
  </arcane>
  <meshes>
    <mesh>
      <filename>PB.msh</filename>
    </mesh>
  </meshes>

  <sh-arc>
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
  </sh-arc>

  <exa-di-bench>
    <use-external-storage>true</use-external-storage>
    <factor>0.5</factor>
	  <geometry name="Euclidian3Geometry"/>
  </exa-di-bench>
</case>
