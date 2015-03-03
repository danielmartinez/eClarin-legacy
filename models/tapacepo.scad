radio=14;
altura=1;
grosor=2.5;
ancho=50;
largo=75;
diametroclarin=18;
alturaclarin=30;
$fn=200;
difference () {
union() {
translate([-(largo-2*radio)/2,-(ancho-2*radio)/2,0])
      minkowski() {
        cylinder(altura/2,r=radio);
        cube([largo-2*radio,ancho-2*radio,altura/2]);
      }

translate([0,0,-1+grosor/2])
      resize([(largo-grosor),(ancho-grosor),altura+grosor+2])
        translate([-(largo-2*radio)/2,-(ancho-2*radio)/2,0])
          minkowski() {
            cylinder(altura/2,r=radio);
            cube([largo-2*radio,ancho-2*radio,altura/2]);
          }
}
translate([0,0,0.5+grosor/2])
      resize([(largo-grosor-grosor),(ancho-grosor-grosor),altura+grosor+5])
        translate([-(largo-2*radio)/2,-(ancho-2*radio)/2,0])
          minkowski() {
            cylinder(altura/2,r=radio);
            cube([largo-2*radio,ancho-2*radio,altura/2]);
          }
translate([12.5-45/2,-3-grosor-4+ancho/2,-1]) cylinder(d=5,h=10);
translate([largo/2-grosor-10,-4.5,-1]) cube([5,9,10]);
}