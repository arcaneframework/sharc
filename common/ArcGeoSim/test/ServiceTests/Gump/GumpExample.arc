<?xml version='1.0' encoding='ISO-8859-1'?>
<case codename="${codename}" xml:lang="en" codeversion="1.0">
	<arcane>
		<title>Test unitaires de Gump</title>
		<timeloop>TestLoop</timeloop>
	</arcane>
  <mesh>
    <meshgenerator><simple><mode>1</mode></simple></meshgenerator>
  </mesh>

  <service-test-mng name="ServiceTestMng">
    <service name="GumpExampleTester" />
  </service-test-mng>

</case>
