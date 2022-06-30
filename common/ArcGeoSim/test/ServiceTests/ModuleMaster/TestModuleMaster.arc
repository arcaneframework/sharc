<?xml version='1.0' encoding='UTF-8'?>
<case codename='${codename}' xml:lang='en' codeversion='1.0'>
  <arcane>
    <title>Test unitaires du module master ArcGeoSim</title>
    <timeloop>TestLoop</timeloop>
  </arcane>
  <mesh>
    <meshgenerator><simple><mode>1</mode></simple></meshgenerator>
  </mesh>
  
  <service-test-mng name="ServiceTestMng">
    <service name="ModuleMasterTester">
      <iteration-max>10</iteration-max>
      <restore-at-iteration>1</restore-at-iteration>
      <restore-at-iteration>3</restore-at-iteration>
      <restore-at-iteration>4</restore-at-iteration>
      <restore-at-iteration>5</restore-at-iteration>
      <restore-at-iteration>9</restore-at-iteration>
    </service>
  </service-test-mng>

</case>
