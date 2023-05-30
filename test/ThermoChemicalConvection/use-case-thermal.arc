<case codename="ShArc" xml:lang="en" codeversion="1.0">
  <arcane>
    <timeloop>ArcResTimeLoop</timeloop>
  </arcane>
  <mesh>
    <file internal-partition="true">mesh-10x1x10.vt2</file>
  </mesh>
  
  <sh-arc>
    <time-manager name="TimeLine">
      <init-time>0.</init-time>
      <end-time>86400e8</end-time>
      <init-time-step>86400e4</init-time-step>
      <min-time-step>86400e2</min-time-step>
      <max-time-step>86400e6</max-time-step>
    </time-manager>
    <time-step-policy name="TimeStepEvolutionPolicy">
      <type>Geometric</type>
      <increase-factor>1.5</increase-factor>
      <decrease-factor>0.5</decrease-factor>
    </time-step-policy>
    <geometry name="Euclidian3Geometry"/>
    <expression-mng name="ExpressionMng"/>
    <post-processing>
      <save-init>false</save-init>
      <output-period>1</output-period>
      <output-path>output</output-path>
      <post-processor>
        <variables>
          <variable>Domain_Pressure_System</variable>
          <variable>Domain_Concentration_System</variable>
          <variable>Domain_Temperature_System</variable>
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
      </fluid-system>
      <solid-system>
        <name>Solid</name>
        <solid-phase>
          <name>Salt</name>
          <species>S</species>
        </solid-phase>
      </solid-system>
    </system>
  </physical-model>
  <thermo-chemical-convection>
    <numerics>
      <newton name="ArcNumNewtonSolver">
        <iteration-max>20</iteration-max>
        <relative-tolerance>1.e-8</relative-tolerance>
        <tolerance>1.e-8</tolerance>
        <control-factor>0.1</control-factor>
        <debug-dump-matlab>false</debug-dump-matlab>
         <linear-solver name="PETScSolver">
	  <solver name="LU" />
          <verbose>low</verbose>
        </linear-solver>
      </newton>
    </numerics>
    <boundary-condition name="DirichletManager">
        <boundary>
          <face-group>ZMIN</face-group>
          <limit-condition>
            <property>[System]System::Pressure</property>
            <value>0</value>
          </limit-condition>
          <limit-condition>
            <property>[System]System::Concentration</property>
            <value>10</value>
          </limit-condition>
          <limit-condition>
            <property>[System]System::Temperature</property>
            <value>200</value>
          </limit-condition>
        </boundary>
        <boundary>
          <face-group>ZMAX</face-group>
          <limit-condition>
            <property>[System]System::Pressure</property>
            <value>0</value>
          </limit-condition>
          <limit-condition>
            <property>[System]System::Concentration</property>
            <value>0</value>
          </limit-condition>
          <limit-condition>
            <property>[System]System::Temperature</property>
            <value>0</value>
          </limit-condition>
        </boundary>
    </boundary-condition>
    <initial-condition name="Constant">
      <property>[System]System::Temperature</property>
      <condition>
        <value>0</value>
      </condition>
    </initial-condition>
    <initial-condition name="Constant">
      <property>[System]System::Concentration</property>
      <condition>
        <value>0</value>
      </condition>
    </initial-condition>
    <initial-condition name="Constant">
      <property>[SubSystem]Fluid::VolumeFraction</property>
      <condition>
        <value>0.1</value>
      </condition>
    </initial-condition>
    <initial-condition name="Constant">
      <property>[SubSystem]Fluid::Permeability</property>
      <condition>
        <value>1</value>
      </condition>
    </initial-condition>
    <law name="FluidDensityLawConfig">
      <output>
        <fluid-density>[Phase]Water::FluidDensity</fluid-density>
      </output>
      <input>
        <concentration>[System]System::Concentration</concentration>
        <temperature>[System]System::Temperature</temperature>
      </input>
      <parameters>
        <rho0>1000</rho0>
        <betat>3.37e-4</betat>
        <betac>6.38e-4</betac>
        <t0>0</t0>
        <c0>0</c0>
      </parameters>
    </law>
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
        <result>[Phase]Water::Viscosity</result>
      </output>
      <input />
      <parameters>
        <value>1.79e-3</value>
      </parameters>
    </law>
    <law name="ConstantLawConfig">
      <output>
        <result>[Phase]Water::RelativePermeability</result>
      </output>
      <input />
      <parameters>
        <value>1</value>
      </parameters>
    </law>
    <law name="ConstantLawConfig">
      <output>
        <result>[Phase]Water::HeatCapacity</result>
      </output>
      <input />
      <parameters>
        <value>4200</value>
      </parameters>
    </law>
    <law name="ConstantLawConfig">
      <output>
        <result>[Phase]Water::HeatConductivity</result>
      </output>
      <input />
      <parameters>
        <value>0.65</value>
      </parameters>
    </law>
    <law name="ConstantLawConfig">
      <output>
        <result>[Phase]Salt::Density</result>
      </output>
      <input />
      <parameters>
        <value>2700</value>
      </parameters>
    </law>
    <law name="ConstantLawConfig">
      <output>
        <result>[Phase]Salt::HeatCapacity</result>
      </output>
      <input />
      <parameters>
        <value>1180</value>
      </parameters>
    </law>
    <law name="ConstantLawConfig">
      <output>
        <result>[Phase]Salt::HeatConductivity</result>
      </output>
      <input />
      <parameters>
        <value>2</value>
      </parameters>
    </law>
  </thermo-chemical-convection>
</case>
