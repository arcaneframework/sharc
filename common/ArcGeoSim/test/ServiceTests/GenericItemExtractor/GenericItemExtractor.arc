<?xml version='1.0' encoding='ISO-8859-1'?>
<case codeversion="1.0" codename="${codename}" xml:lang="en">

  <arcane>
    <title>Test the items extractor</title>
    <timeloop>TestLoop</timeloop>
  </arcane>
  <mesh dual='true'>
    <meshgenerator>
       <sod>
          <x>100</x>
          <y>5</y>
          <z>5</z>
       </sod>
    </meshgenerator>  
  </mesh>

  <!-- Service test manager -->
  <service-test-mng name="ServiceTestMng">
    <service name="GenericItemExtractorTester">
    	<itemExtractor name="GenericItemExtractor">
    		<input-file>test_voronoi.TMF</input-file>
    		<output-file>test_voronoi.vor</output-file>
    		<item-file>test_voronoi.format</item-file>
    	</itemExtractor>
    </service>
  </service-test-mng>
</case>
