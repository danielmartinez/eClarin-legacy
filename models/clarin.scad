$fn=200;
difference(){
rotate_extrude(){
difference() {
    union() {
      polygon([[0,0],[26/2,0],[26/2,16],[35/2,21],[23/2,21+26],[21/2,21+26+35],[20/2,21+26+35+30],[19/2,21+26+35+30+30],[18/2,21+26+35+30+30+30],[17/2,21+26+35+30+30+30+30],[16/2,21+26+35+30+30+30+30+30],[27/2,21+26+35+30+30+30+30+30+8],[18/2,21+26+35+30+30+30+30+30+8],[18/2,21+26+35+30+30+30+30+30+8+30],[0,21+26+35+30+30+30+30+30+8+30]]);
      translate([10,12,0]) circle(9);
      translate([13,2,0]) circle(2);
      intersection() {
        translate([4,1+21+26+35+30+30+30+30+30+8]) circle(10);
        translate([0,1+26+35+30+30+30+30+30+8])square([20,20]);
      }
    }
    translate([60+23/2,21+26] )circle(60);
  }
}

translate([-15,16,222]) rotate([0,90,0]) cylinder(h=30,r=10);
translate([-15,-16.1,212]) rotate([0,90,0]) cylinder(h=30,r=10);
translate([-15,-16.4,192]) rotate([0,90,0]) cylinder(h=30,r=10);
translate([-15,-16.9,167]) rotate([0,90,0]) cylinder(h=30,r=10);
translate([-15,-17.4,143]) rotate([0,90,0]) cylinder(h=30,r=10);
translate([-15,-17.8,125]) rotate([0,90,0]) cylinder(h=30,r=10);
translate([-15,-18.3,99]) rotate([0,90,0]) cylinder(h=30,r=10);
rotate([0,0,-30]) translate([-15,-18.9,81]) rotate([0,90,0]) cylinder(h=30,r=10);
}
//rotate([0,0,-90]) color("grey") translate([-98.5,-94.5,0]) import("clarin2.stl");