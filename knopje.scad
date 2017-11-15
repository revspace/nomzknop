width = 40;
height = 20;
depth = 1.6;

tri_width = 20;
tri_height = 17;
tri_depth = 1.2;

text_depth = 2;

difference() {
    translate([-width/2, -height/2, 0]) cube([width, height, depth]);
    translate([-tri_width/2, -tri_height/2, 0]) cube([tri_width, tri_height, tri_depth]);
}

//translate([1.5, 0, depth]) {
//    linear_extrude(height = text_depth) {
//        text("NOMZ", halign = "center", valign = "center", font = "Envy Code R:style=Bold", spacing=1.25);
//    }
//}