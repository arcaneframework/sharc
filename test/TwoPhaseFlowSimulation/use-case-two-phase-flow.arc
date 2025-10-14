<case codename="ShArc" xml:lang="en" codeversion="1.0">
  <arcane>
    <timeloop>ArcResTimeLoop</timeloop>
  </arcane>
  <mesh>
    <file internal-partition="true">mesh-10x1x1.vt2</file>
  </mesh>
  
  <sh-arc>
    <time-manager name="TimeLine">
      <verbose>true</verbose>
      <init-time>0.</init-time>
      <end-time>86400e5</end-time>
      <init-time-step>86400e2</init-time-step>
      <min-time-step>86400e2</min-time-step>
      <max-time-step>86400e3</max-time-step>
    </time-manager>
    <time-step-policy name="TimeStepEvolutionPolicy">
      <type>Geometric</type>
      <increase-factor>1.5</increase-factor>
      <decrease-factor>0.5</decrease-factor>
    </time-step-policy>
    <geometry name="Euclidian3Geometry"/>
    <validator name="SyntheticServiceValidator">
      <variable-field name="VariableAccessor">
        <name>Domain_Pressure_System</name>
      </variable-field>
      <group-name>AllCells</group-name>
      <reference-value>10512859.6282460578</reference-value>
      <reduction>Mean</reduction>
      <comparator>AbsoluteError</comparator>
      <tolerance>1e-6</tolerance>
      <verbose>true</verbose>
    </validator>
    <validator name="SyntheticServiceValidator">
      <variable-field name="VariableAccessor">
        <name>Domain_Saturation_Water</name>
      </variable-field>
      <group-name>AllCells</group-name>
      <reference-value>0.0516165805</reference-value>
      <reduction>Mean</reduction>
      <comparator>AbsoluteError</comparator>
      <tolerance>1e-6</tolerance>
      <verbose>true</verbose>
    </validator>
    <expression-mng name="ExpressionMng"/>
    <post-processing>
      <save-init>false</save-init>
      <output-period>1</output-period>
      <output-path>output</output-path>
      <post-processor>
        <variables>
          <variable>Domain_Pressure_System</variable>
          <variable>Domain_ZPermeability_Fluid</variable>
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
    <numerics>
      <newton name="ArcNumNewtonSolver">
        <iteration-max>20</iteration-max>
        <relative-tolerance>1.e-8</relative-tolerance>
        <tolerance>1.e-8</tolerance>
        <control-factor>0.1</control-factor>
        <debug-dump-matlab>false</debug-dump-matlab>
        <linear-solver name="HypreSolver">
          <num-iterations-max>1000</num-iterations-max>
          <stop-criteria-value>1e-3</stop-criteria-value>
          <solver>BiCGStab</solver>
          <preconditioner>Euclid</preconditioner>
          <verbose>true</verbose>
        </linear-solver>
         <!--linear-solver name="PETScSolver">
             <solver name="SuperLU" />
             <solver name="BiCGStab">
               <num-iterations-max>5000</num-iterations-max>
               <stop-criteria-value>1e-12</stop-criteria-value>
               <preconditioner name="BlockILU"/>
             </solver>
             <verbose>low</verbose>
        </linear-solver-->
      </newton>
    </numerics>
    <boundary-condition name="DirichletManager">
        <boundary>
          <face-group>XMIN</face-group>
          <limit-condition>
            <property>[System]System::Pressure</property>
            <value>110.e5</value>
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
    <initial-condition name="Constant">
      <property>[SubSystem]Fluid::Permeability</property>
      <condition>
        <value>1000.e-15</value>
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
        <value>1.e-3</value>
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
  </two-phase-flow-simulation>
</case>
