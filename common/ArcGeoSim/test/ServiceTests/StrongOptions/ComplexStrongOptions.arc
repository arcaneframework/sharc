<?xml version='1.0' encoding='ISO-8859-1'?>
<case codename="${codename}" xml:lang="en" codeversion="1.0">

  <arcane>
    <title>Test complex Strong Options</title>
    <timeloop>TestLoop</timeloop>
  </arcane>
  <mesh>
    <meshgenerator><simple><mode>1</mode></simple></meshgenerator>
  </mesh>

  <service-test-mng name="ServiceTestMng">

    <service name="RootTester">
 
      <!-- Simple types without default values -->

      <simple-int-3>2</simple-int-3>
      <simple-real-3>2.5</simple-real-3>
      <simple-bool-3>true</simple-bool-3>
      <simple-string-3>MCG</simple-string-3>
      <simple-enum-3>enum3</simple-enum-3>

      <!-- Services without default values -->

      <service-x-3 name = "X2">
        <simple-int-1>5</simple-int-1>
      </service-x-3>
      <service-x-4 name = "X2">
        <simple-int-1>2</simple-int-1>
      </service-x-4>

      <!-- Multiple occurences without default values -->

      <simple-int-4>1</simple-int-4>
      <simple-int-4>5</simple-int-4>
      <simple-int-4>10</simple-int-4>

      <simple-real-4>101.5</simple-real-4>
      <simple-real-4>102.5</simple-real-4>
      <simple-real-4>103.5</simple-real-4>

      <simple-string-4>Test String 2</simple-string-4>
      <simple-string-4>Test String 3</simple-string-4>

      <simple-bool-4>true</simple-bool-4>

      <simple-enum-4>enum2</simple-enum-4>
  
    </service>
    
  </service-test-mng>

</case>
