<case codename="ShArc" xml:lang="en" codeversion="1.0">
  <arcane>
    <timeloop>ArcResTimeLoop</timeloop>
  </arcane>
  <mesh>
    <meshgenerator>
        <cartesian>
          <face-numbering-version>4</face-numbering-version>
           <origine>0 0 -100</origine>
           <nsd>__NPX__ __NPY__ __NPZ__</nsd>
           <lx nx="128">1000</lx>
           <ly ny="128">1000</ly>
           <lz nz="10">100</lz>
        </cartesian>
    </meshgenerator>
  </mesh>
  
  <sh-arc>
    <time-manager name="TimeLine">
      <verbose>true</verbose>
      <init-time>0.</init-time>
      <end-time>86400e2</end-time> <!--100 days-->
      <init-time-step>4320</init-time-step>
      <min-time-step>4320e-1</min-time-step>
      <max-time-step>21600</max-time-step>
    </time-manager>
    <time-step-policy name="TimeStepEvolutionPolicy">
      <type>Geometric</type>
      <increase-factor>1.5</increase-factor>
      <decrease-factor>0.5</decrease-factor>
    </time-step-policy>
    <geometry name="Euclidian3Geometry"/>
    <!--validator name="SyntheticServiceValidator">
      <variable-field name="VariableAccessor">
        <name>Domain_Pressure_System</name>
      </variable-field>
      <group-name>AllCells</group-name>
      <reference-value>10512859.6282460578</reference-value>
      <reduction>Mean</reduction>
      <comparator>AbsoluteError</comparator>
      <tolerance>1e-6</tolerance>
      <verbose>true</verbose>
    </validator-->
    <!--validator name="SyntPheticServiceValidator">
      <variable-field name="VariableAccessor">
        <name>Domain_Saturation_Water</name>
      </variable-field>
      <group-name>AllCells</group-name>
      <reference-value>0.0516165805</reference-value>
      <reduction>Mean</reduction>
      <comparator>AbsoluteError</comparator>
      <tolerance>1e-6</tolerance>
      <verbose>true</verbose>
    </validator-->
    <expression-mng name="ExpressionMng"/>
    <post-processing>
      <save-init>false</save-init>
      <output-period>10</output-period>
      <output-path>output</output-path>
      <post-processor>
        <format name="Ensight7PostProcessor">
          <force-first-geometry>true</force-first-geometry>
        </format>
        <variables>
          <variable>Domain_Pressure_System</variable>
          <variable>Domain_Permeability_Fluid</variable>
          <variable>Domain_Saturation_Water</variable>
        </variables>
      </post-processor>
    </post-processing>
  </sh-arc>
  <physical-model>
    <system name="UserSystem">
      <name>System</name>
      <fluid-system>
        <name>Fluid</name>
        <fluid-phase>
          <name>Water</name>
          <species>H2O</species>
        </fluid-phase>
        <fluid-phase>
          <name>Gas</name>
          <species>CO2</species>
        </fluid-phase>
      </fluid-system>
      <solid-system>
        <name>Solid</name>
      </solid-system>
    </system>
  </physical-model>
  <two-phase-flow-simulation>
    <group-creator name="StandardGroupCreator">
      <scalar-eval>false</scalar-eval>
      <facegroup>
        <name>XMIN</name>
        <area>GC_allBoundaryFaces</area>
        <filter>h(1.e-14-x)</filter>
      </facegroup>
      <facegroup>
        <name>XMAX</name>
        <area>GC_allBoundaryFaces</area>
        <filter>h(x-1000)</filter>
      </facegroup>
    </group-creator>
    <numerics>
      <newton name="ArcNumNewtonSolver">
        <iteration-max>20</iteration-max>
        <relative-tolerance>1.e-8</relative-tolerance>
        <tolerance>1.e-8</tolerance>
        <control-factor>0.1</control-factor>
        <debug-dump-matlab>false</debug-dump-matlab>
        <debug-stat-linear-solver>true</debug-stat-linear-solver>

        <linear-solver name="MCGSolver">
          <output>1</output>
          <rowsum>true</rowsum>
          <max-iteration-num>1000</max-iteration-num>
          <stop-criteria-value>1e-8</stop-criteria-value>
          <kernel>GPU_CUBLAS_BCSP</kernel>
          <preconditioner>CprAmg</preconditioner>
          <CprAmg>
            <cxr-solver>amgx</cxr-solver>
            <relax-solver>ColorILU0</relax-solver>
          </CprAmg>
        </linear-solver>

      </newton>
    </numerics>
    <boundary-condition name="DirichletManager">
        <boundary>
          <face-group>XMIN</face-group>
          <limit-condition>
            <property>[System]System::Pressure</property>
            <value>600.e5</value>
          </limit-condition>
          <limit-condition>
            <property>[Phase]Water::Saturation</property>
            <value>0.</value>
          </limit-condition>
          <limit-condition>
            <property>[Phase]Gas::Saturation</property>
            <value>1.</value>
          </limit-condition>
        </boundary>
        <boundary>
          <face-group>XMAX</face-group>
          <limit-condition>
            <property>[System]System::Pressure</property>
            <value>100.e5</value>
          </limit-condition>
          <limit-condition>
            <property>[Phase]Water::Saturation</property>
            <value>1.</value>
          </limit-condition>
          <limit-condition>
            <property>[Phase]Gas::Saturation</property>
            <value>0.</value>
          </limit-condition>
        </boundary>
    </boundary-condition>
    <initial-condition name="Constant">
      <property>[System]System::Pressure</property>
      <condition>
        <value>105.e5</value>
      </condition>
    </initial-condition>
    <initial-condition name="Constant">
      <property>[SubSystem]Fluid::VolumeFraction</property>
      <condition>
        <value>0.2</value>
      </condition>
    </initial-condition>
    <initial-condition name="Constant">
      <property>[Phase]Water::Saturation</property>
      <condition>
        <value>1.</value>
      </condition>
    </initial-condition>
    <initial-condition name="Constant">
      <property>[Phase]Gas::Saturation</property>
      <condition>
        <value>0.</value>
      </condition>
    </initial-condition>
     <initial-condition name="Expression">
      <property>[SubSystem]Fluid::Permeability</property>
      <condition name="ExpressionBuilderR3vR1">
       <expression>(x,y,z)->exp(-30+3*sin(2*pi*x/1000)*sin(8*pi*y/1000)+y/1000)</expression>
       <constant>
        <name>pi</name>
        <value>3.14</value>
       </constant>
      </condition>
     </initial-condition>
    <law name="ConstantLawConfig">
      <output>
        <result>[Phase]Water::Density</result>
      </output>
      <input />
      <parameters>
        <value>1000</value>
      </parameters>
    </law>
    <law name="ConstantLawConfig">
      <output>
        <result>[Phase]Gas::Density</result>
      </output>
      <input />
      <parameters>
        <value>1000</value>
      </parameters>
    </law>
    <law name="ConstantLawConfig">
      <output>
        <result>[Phase]Water::Viscosity</result>
      </output>
      <input />
      <parameters>
        <value>1.e-3</value>
      </parameters>
    </law>
    <law name="ConstantLawConfig">
      <output>
        <result>[Phase]Gas::Viscosity</result>
      </output>
      <input />
      <parameters>
        <value>1.e-4</value>
      </parameters>
    </law>
    <law name="RelativePermeabilityPowerLawConfig">
      <output>
        <relative-permeability>[Phase]Water::RelativePermeability</relative-permeability>
      </output>
      <input>
        <saturation>[Phase]Water::Saturation</saturation>
      </input>
      <parameters>
        <alpha>2</alpha>
        <Swi>0</Swi>
        <Sgc>0</Sgc>
      </parameters>
    </law>
    <law name="RelativePermeabilityPowerLawConfig">
      <output>
        <relative-permeability>[Phase]Gas::RelativePermeability</relative-permeability>
      </output>
      <input>
        <saturation>[Phase]Gas::Saturation</saturation>
      </input>
      <parameters>
        <alpha>2</alpha>
        <Swi>0</Swi>
        <Sgc>0</Sgc>
      </parameters>
    </law>
    <law name="CapillaryPressureLawConfig">
      <output>
        <capillary-pressure>[Phase]Water::CapillaryPressure</capillary-pressure>
      </output>
      <input>
        <saturation>[Phase]Water::Saturation</saturation>
      </input>
      <parameters>
        <Pe>0</Pe>
      </parameters>
    </law>
    <law name="CapillaryPressureLawConfig">
      <output>
        <capillary-pressure>[Phase]Gas::CapillaryPressure</capillary-pressure>
      </output>
      <input>
        <saturation>[Phase]Gas::Saturation</saturation>
      </input>
      <parameters>
        <Pe>1.e5</Pe>
        <Sr-ref>0.2</Sr-ref>
        <Sr>0.1</Sr>
        <lambda>1</lambda>
      </parameters>
    </law>
  </two-phase-flow-simulation>
</case>
