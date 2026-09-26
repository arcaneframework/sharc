lc =0.1;
Point(1) = {0.0,0.0,0.0,lc};
Point(2) = {1,0.0,0.0,lc};
Point(3) = {1,1,0.0,lc};
Point(4) = {0,1,0.0,lc};
Line(1) = {4,3};
Line(2) = {3,2};
Line(3) = {2,1};
Line(4) = {1,4};
Line Loop(5) = {2,3,4,1};
Plane Surface(1) = {5};
Extrude {0,0.0,1} {
  Surface{1};
}
Physical Surface("Back") = {6};
Physical Surface("Right") = {15};
Physical Surface("Bottom") = {19};
Physical Surface("Left") = {23};
Physical Surface("Top") = {27};
Physical Surface("Front") = {28};

Physical Volume("Vol")={1};


Transfinite Line "*";
Transfinite Surface "*";
Recombine Surface "*";
Transfinite Volume "*";

Mesh 3;

//Save "./mesh3D/hexa_fin_01.vtk";
