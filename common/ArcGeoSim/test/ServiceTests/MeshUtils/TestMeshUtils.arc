<?xml version='1.0' encoding='ISO-8859-1'?>
<case codeversion="1.0" codename="${codename}" xml:lang="en">
    <arcane>
        <title>Test the mesh utils</title>
        <timeloop>TestLoop</timeloop>
    </arcane>

    <mesh>
        <file internal-partition='true'>cube3D.vt2</file>
    </mesh>

    <!-- Service test manager -->
    <service-test-mng name="ServiceTestMng">
        <service name="MeshUtilsTester">
        </service>
        <geometry-service name="Euclidian3Geometry"/>
    </service-test-mng>

</case>

