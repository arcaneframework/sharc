<?xml version='1.0' encoding='ISO-8859-1'?>
<case codeversion="1.0" codename="${codename}" xml:lang="en">
    <arcane>
        <title>Test the mesh utils</title>
        <timeloop>TestLoop</timeloop>
    </arcane>

    <meshes>
        <mesh>
<!--            <filename>faultx1_2x1x1.vtk</filename>-->
            <filename>faultx4_3x3x2.vtk</filename>
            <specific-reader name="VtkPolyhedralCaseMeshReader">
                <print-mesh-infos>false</print-mesh-infos>
                <print-debug-infos>false</print-debug-infos>
            </specific-reader>
        </mesh>
    </meshes>

    <!-- Service test manager -->
    <service-test-mng name="ServiceTestMng">
        <service name="MeshUtilsTester">
        </service>
        <geometry-service name="Euclidian3Geometry"/>
    </service-test-mng>

</case>

