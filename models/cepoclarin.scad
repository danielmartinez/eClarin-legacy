radio=14;
altura=60;
grosor=2.5;
ancho=50;
largo=75;
diametroclarin=18;
alturaclarin=30;
$fn=200;
difference(){
  union(){
    //Cepo body
    difference() {
    translate([-(largo-2*radio)/2,-(ancho-2*radio)/2,0])
      minkowski() {
        cylinder(altura/2,r=radio);
        cube([largo-2*radio,ancho-2*radio,altura/2]);
      }
    translate([0,0,grosor/2])
      resize([(largo-grosor),(ancho-grosor),altura+grosor])
        translate([-(largo-2*radio)/2,-(ancho-2*radio)/2,0])
          minkowski() {
            cylinder(altura/2,r=radio);
            cube([largo-2*radio,ancho-2*radio,altura/2]);
          }
    }
    //Chanter support
    cylinder(d=diametroclarin+grosor+0.75,h=alturaclarin);
    //Slice switch support
    translate([grosor-0.7+(largo-radio)/2,-2.5,0]) cube([4,5,7+6+altura/2]);
    //VS1053 Module support
    translate([-2-45/2,-5.3-grosor+ancho/2,0]) cube([3,7,altura-5]);
    mirror([1,0,0]) translate([-2-45/2,-5.3-grosor+ancho/2,0]) cube([3,7,altura-5]);
    //ARDUINO NANO/MINI PRO support
    translate([7-largo/2,-8/2,0]) cube([6,8,15]);
  }
  //LED Hole
  translate([-2-largo/2,0,33]) rotate([0,90,0]) cylinder(d=3,h=2*grosor);
  //Slide switch Hole
  translate([(largo-radio)/2,-2,altura/2]) cube([grosor+5,4,7]);
  //Chanter Hole
  translate([0,0,-1]) cylinder(d=diametroclarin+0.75,h=alturaclarin+2);
  //VS1053
  translate([-45/2,-grosor-4+ancho/2,15]) cube([45,2,45]);
  //ARDUINO
  translate([9-largo/2,-20/2,10]) cube([1,20,15]);
}

