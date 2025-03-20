<?xml version='1.0' encoding='ISO-8859-1'?>
<case codeversion="1.0" codename="${codename}" xml:lang="en">

  <arcane>
    <title>Test expressions</title>
    <timeloop>TestLoop</timeloop>
  </arcane>

  <mesh dual='true'>
    <file internal-partition='true'>cube2x2x2.vt2</file>
  </mesh>

  <!-- Service test manager -->
  <service-test-mng name="ServiceTestMng">
  
    <service name="ExpressionsTester">
   
      <dump>true</dump>

      <!-- Choose if refined cpu comparison between PieceWiseFunction and PhysicalLaw -->
      <table-impl-cpu-comparison>false</table-impl-cpu-comparison>
      
      <!-- Choose if expressions performance test is activated -->
      <performance-test>false</performance-test>
   
    <f4 name="ExpressionBuilderR3vR1">
      <expression>(x,y,z;t)->x + 10*y + 100*z + t</expression>
    </f4>


    <f5 name="ConstantFunction">
      <value>10.</value>
    </f5>
    
    <f6 name="KozenyCarmannFunction">
      <parameter>
        <value>2.2</value>
      </parameter>
    </f6>
    
    <f7 name="ExpressionBuilderR1vR1">
      <expression>(x)->1+exp(3*x+2)</expression>
    </f7>
    
  
  
  <!-- Existing table test  -->

    
    <f9 name="LinearPieceWiseFunction">
      <border-condition>
        <border-type>infinite-prolongation</border-type>
        <prolongation-type>linear-prolongation</prolongation-type>
        <cut-type>silent-cut</cut-type>
<!--        <cut-type>exception-throw-cut</cut-type>-->
        <border-limits>
          <lower-limit>-2</lower-limit>
          <upper-limit>50</upper-limit>
        </border-limits>
      </border-condition>
        <x>0.0</x><y>0.0</y> 
        <x>0.5</x><y>1.0</y> 
        <x>1.0</x><y>2.0</y>
        <x>1.5</x><y>2.5</y> 
        <x>2.0</x><y>1.5</y> 
        <x>2.5</x><y>0.5</y>
        <x>3.0</x><y>1.25</y> 
        <x>3.5</x><y>1.5</y>
 <!--   <x>1.5</x><y>2</y>   Fatal ERROR-->
        <x>4.0</x><y>2.5</y>
        <x>4.5</x><y>1.5</y>
        <x>5.0</x><y>0.5</y> 
        <x>5.5</x><y>1.25</y>
        <x>6.0</x><y>1.5</y> 
        <x>6.5</x><y>2.5</y> 
        <x>7.0</x><y>1.5</y>
        <x>7.5</x><y>0.5</y> 
        <x>8.0</x><y>1.25</y> 
        <x>8.5</x><y>1.5</y>
        <x>9.0</x><y>4.5</y> 
        <x>9.5</x><y>4.75</y> 
<!--        <x>10.0</x><y>0.5</y>-->
<!--        <x>10.5</x><y>1.25</y> -->
<!--        <x>11.0</x><y>1.5</y> -->
<!--        <x>11.5</x><y>2.5</y>-->
<!--        <x>12.0</x><y>1.5</y> -->
<!--        <x>12.5</x><y>0.5</y> -->
<!--        <x>13.0</x><y>1.25</y>-->
<!--        <x>13.5</x><y>1.5</y> -->
<!--        <x>14.0</x><y>2.5</y> -->
<!--        <x>14.5</x><y>1.5</y>-->
<!--        <x>15.0</x><y>0.5</y> -->
<!--        <x>15.5</x><y>1.25</y> -->
<!--        <x>16.0</x><y>1.5</y>-->
<!--        <x>16.5</x><y>2.5</y> -->
<!--        <x>17.0</x><y>1.5</y> -->
<!--        <x>17.5</x><y>0.5</y>-->
<!--        <x>18.0</x><y>1.25</y> -->
<!--        <x>18.5</x><y>1.5</y> -->
<!--        <x>19.0</x><y>9.5</y>-->
<!--        <x>19.5</x><y>9.75</y> -->
<!--        <x>20.0</x><y>0.5</y> -->
<!--        <x>20.5</x><y>1.25</y>-->
<!--        <x>21.0</x><y>1.5</y> -->
<!--        <x>21.5</x><y>2.5</y> -->
<!--        <x>22.0</x><y>1.5</y>-->
<!--        <x>22.5</x><y>0.5</y> -->
<!--        <x>23.0</x><y>1.25</y> -->
<!--        <x>23.5</x><y>1.5</y>-->
<!--        <x>24.0</x><y>2.5</y> -->
<!--        <x>24.5</x><y>1.5</y> -->
<!--        <x>25.0</x><y>0.5</y>-->
<!--        <x>25.5</x><y>1.25</y> -->
<!--        <x>26.0</x><y>1.5</y> -->
<!--        <x>26.5</x><y>2.5</y>-->
<!--        <x>27.0</x><y>1.5</y> -->
<!--        <x>27.5</x><y>0.5</y> -->
<!--        <x>28.0</x><y>1.25</y>-->
<!--        <x>28.5</x><y>1.5</y> -->
<!--        <x>29.0</x><y>2.5</y> -->
<!--        <x>29.5</x><y>1.5</y>-->
<!--        <x>30.0</x><y>0.5</y> -->
<!--        <x>30.5</x><y>1.25</y> -->
<!--        <x>31.0</x><y>1.5</y>-->
<!--        <x>31.5</x><y>2.5</y> -->
<!--        <x>32.0</x><y>1.5</y> -->
<!--        <x>32.5</x><y>0.5</y>-->
<!--        <x>33.0</x><y>1.25</y> -->
<!--        <x>33.5</x><y>1.5</y> -->
<!--        <x>34.0</x><y>2.5</y>-->
<!--        <x>34.5</x><y>1.5</y> -->
<!--        <x>35.0</x><y>0.5</y> -->
<!--        <x>35.5</x><y>1.25</y>-->
<!--        <x>36.0</x><y>1.5</y> -->
<!--        <x>36.5</x><y>2.5</y> -->
<!--        <x>37.0</x><y>1.5</y>-->
<!--        <x>37.5</x><y>0.5</y> -->
<!--        <x>38.0</x><y>1.25</y> -->
<!--        <x>38.5</x><y>1.5</y>-->
<!--        <x>39.0</x><y>19.5</y> -->
<!--        <x>39.5</x><y>19.75</y>-->
    </f9>
    
    <f10 name="ConstantPieceWiseFunction">
      <border-condition>
        <border-type>finite-prolongation</border-type>
        <cut-type>silent-cut</cut-type>
<!--        <cut-type>exception-throw-cut</cut-type>-->
        <border-limits>
          <lower-limit>-3</lower-limit>
          <upper-limit>15</upper-limit>
        </border-limits>
      </border-condition>
      <x>9.5</x><y>1.5</y>
      <x>1.5</x><y>2.5</y>
      <x>3.5</x><y>1.5</y>
      <x>2.0</x><y>1.5</y>
      <x>1.0</x><y>1.5</y>
      <x>2.5</x><y>0.5</y>
      <x>5.5</x><y>1.25</y>
      <x>3.0</x><y>1.25</y>
      <x>6.5</x><y>2.5</y>
      <x>4.0</x><y>2.5</y>
      <x>8.5</x><y>1.5</y>
      <x>5.0</x><y>0.5</y>
      <x>0.5</x><y>1.25</y>
      <x>6.0</x><y>1.5</y>
      <x>9.0</x><y>2.5</y>
      <x>8.0</x><y>1.25</y>
      <x>4.5</x><y>1.5</y>
      <x>7.0</x><y>1.5</y>
      <x>7.5</x><y>0.5</y>
      <x>0.0</x><y>0.5</y>
 <!-- <x>4.5</x><y>2.5</y>  Fatal ERROR-->
    </f10>

	<fandrade name="AndradePieceWiseFunction">
      <border-condition>
        <border-type>not-prolongated</border-type>
      </border-condition>
      <x>1.0</x><y>1.0</y>
      <x>2.0</x><y>2.0</y>
      <x>3.0</x><y>3.0</y>
      <x>4.0</x><y>4.0</y>
      <x>5.0</x><y>5.0</y>
      <x>6.0</x><y>6.0</y>
      <x>7.0</x><y>7.0</y>
	</fandrade>
            
    <flog name="LogLinearPieceWiseFunction">
      <ordinate-mode>log-ordinate-given</ordinate-mode>
      <border-condition>
        <border-type>not-prolongated</border-type>
      </border-condition>
      <x>0.0</x><y>0.0</y>
      <x>1.0</x><y>1.0</y>
      <x>2.0</x><y>2.0</y>
      <x>3.0</x><y>3.0</y>
      <x>4.0</x><y>4.0</y>
      <x>5.0</x><y>5.0</y>
      <x>6.0</x><y>6.0</y>
    </flog>
    
    <flogbis name="LogLinearPieceWiseFunction">
      <ordinate-mode>ordinate-given</ordinate-mode>
      <border-condition>
        <border-type>not-prolongated</border-type>
      </border-condition>
      <x>0.0</x><y>1.0</y>
      <x>1.0</x><y>10</y>
      <x>2.0</x><y>100</y>
      <x>3.0</x><y>1000</y>
      <x>4.0</x><y>10000</y>
      <x>5.0</x><y>100000</y>
      <x>6.0</x><y>1000000</y>
    </flogbis>
    
    <fclone name="LinearPieceWiseFunction">
      <border-condition>
        <border-type>finite-prolongation</border-type>
        <prolongation-type>constant-prolongation</prolongation-type>
        <cut-type>silent-cut</cut-type>
<!--        <cut-type>exception-throw-cut</cut-type>-->
        <border-limits>
          <lower-limit>-5</lower-limit>
          <upper-limit>5</upper-limit>
        </border-limits>
      </border-condition>
      <x>0.0</x><y>0.0</y>
      <x>0.5</x><y>0.5</y>
      <x>1.0</x><y>1.0</y>
      <x>1.5</x><y>1.0</y>
      <x>2.0</x><y>1.0</y>
      <x>2.5</x><y>0.5</y>
      <x>3.0</x><y>0.0</y>
    </fclone>
    
	  <bi-row-linear-wise-function name="BiLinearPieceWiseFunction">
		<print-table>true</print-table>
		<row-name>P</row-name>
		<col-name>T</col-name>
		<vector-type>row</vector-type>
		<vector-indexes>0 .1 .8 1. 10.</vector-indexes>
		<vector>
			<index>0</index>
			<values>0 .1 .8 1. 10.</values>
		</vector>
		<vector>
			<index>.1</index>
			<values>.1 .2 .9 1.1 10.1</values>
		</vector>
		<vector>
			<index>.8</index>
			<values>0.8 .9 1.6 1.8 10.8</values>
		</vector>
		<vector>
			<index>1</index>
			<values>1 1.1 1.8 2. 11.</values>
		</vector>
		<vector>
			<index>10</index>
			<values>10 10.1 10.8 11. 20.</values>
		</vector>
		<border-condition>
			<border-type>finite-prolongation</border-type>
			<prolongation-type>linear-prolongation</prolongation-type>
			<cut-type>silent-cut</cut-type>
			<border-limits>
				<lower-row-limit>-100.</lower-row-limit>
				<upper-row-limit>200</upper-row-limit>
				<lower-col-limit>-100.</lower-col-limit>
				<upper-col-limit>200</upper-col-limit>
			</border-limits>
		</border-condition>
	  </bi-row-linear-wise-function>
	  <bi-col-linear-wise-function name="BiLinearPieceWiseFunction">
	  <!--f(x,y)=2x+3y+4 defined by column -->
		<print-table>true</print-table>
		<row-name>P</row-name>
		<col-name>T</col-name>
		<vector-type>column</vector-type>
		<vector-indexes>0 5 7 10 20 22 30</vector-indexes>
		<vector>
			<index>0</index>
			<values>4 19 25 34 64 70 94</values>
		</vector>
		<vector>
			<index>.1</index>
			<values>4.2 19.2 25.2 34.2 64.2 70.2 94.2</values>
		</vector>
		<vector>
			<index>.8</index>
			<values>5.6 20.6 26.6 35.6 65.6 71.6 95.6</values>
		</vector>
		<vector>
			<index>1</index>
			<values>6 21 27 36 66 72 96</values>
		</vector>
		<vector>
			<index>10</index>
			<values>24 39 45 54 84 90 114</values>
		</vector>
		<border-condition>
			<border-type>finite-prolongation</border-type>
			<prolongation-type>linear-prolongation</prolongation-type>
			<cut-type>silent-cut</cut-type>
			<border-limits>
				<lower-row-limit>-100.</lower-row-limit>
				<upper-row-limit>200</upper-row-limit>
				<lower-col-limit>-100.</lower-col-limit>
				<upper-col-limit>200</upper-col-limit>
			</border-limits>
		</border-condition>
	  </bi-col-linear-wise-function>

    <fderivative name="LinearPieceWiseFunction">
      <border-condition>
        <border-type>not-prolongated</border-type>    
      </border-condition>
      <x>0</x><y>0</y>
      <x>1.0</x><y>1</y>
      <x>2.0</x><y>3</y>
      <x>3.0</x><y>3</y>
      <x>4.0</x><y>1</y>
      <x>5.0</x><y>0</y>
    </fderivative>
    
    <fvertical-data name="LinearInterpGridDataFunction">
      <filename>grid_z_hdf.xmf</filename>
      <grid-type>non-uniform</grid-type>
    </fvertical-data>
    
    
    <fvertical-data-time name="LinearInterpGridDataFunction">
      <filename>grid_zt_hdf.xmf</filename>
      <grid-type>non-uniform</grid-type>
    </fvertical-data-time>
    
     <!--fregular-grid-data name="OnePointInterpGridDataFunction">
      <filename>grid_xy_co_hdf.xmf</filename>
     <grid-type>uniform</grid-type>
    </fregular-grid-data-->
       
    <fregular-grid-data name="LinearInterpGridDataFunction">
      <filename>grid_xy_hdf.xmf</filename>
      <grid-type>non-uniform</grid-type>
    </fregular-grid-data>
             
    <fregular-grid-data-time name="LinearInterpGridDataFunction">
      <filename>grid_xyt_hdf.xmf</filename>
      <grid-type>non-uniform</grid-type>
    </fregular-grid-data-time>
    
    <fregular-grid-data-linear name="LinearInterpGridDataFunction">
      <filename>grid_xyzt_co.xmf</filename>
      <grid-type>uniform</grid-type>
    </fregular-grid-data-linear>
    
    <analytical-function name="ExpressionBuilderR1vR1">
        <expression>(x)->x</expression>
    </analytical-function>

    <polynome-function name="ExpressionBuilderR1vR1">
      <expression>(x)->1+x+x*x+x*x*x+x*x*x*x</expression>
    </polynome-function>

    <piece-wise-function name="LinearPieceWiseFunction">
      <x>0. </x><y>0. </y>
      <x>0.2</x><y>0.2</y>
      <x>0.4</x><y>0.4</y>
      <x>0.6</x><y>0.6</y>
      <x>0.8</x><y>0.8</y>
      <x>0.1</x><y>0.1</y>
      <x>0.3</x><y>0.3</y>
      <x>0.5</x><y>0.5</y>
      <x>0.7</x><y>0.7</y>
      <x>0.9</x><y>0.9</y>
      <x>1. </x><y>1. </y>
    </piece-wise-function>
      

      
    <!--regular-grid-data-function name="OnePointInterpGridDataFunction">
      <filename>grid_xy_hdf.xmf</filename>
      <grid-type>non-uniform</grid-type>
    </regular-grid-data-function-->
    
    <regular-grid-data-function name="OnePointInterpGridDataFunction">
      <filename>grid_xy_co_hdf.xmf</filename>
      <grid-type>uniform</grid-type>
    </regular-grid-data-function>
    
    </service>

  </service-test-mng>


</case>
