/*
  Weather Station

  Copyright (c) R. Linder
  This work is licensed under a Creative Commons Attribution-NonCommercial 4.0 International License.

 I had been toying with creating a weather station for a few years,
 then recently I saw Dan Bemowski's https://www.thingiverse.com/thing:2757369

 I had a few issues with sizes, design and printing, after looking at his
 scad code I decided it would be easier to create my own, I love some of
 Dan's concepts esp. the mounting.
 
 Things that are noticably different are from top to bottom, the wind vane
 cap, I overlap for better weather protection, bearing sizes I use ceramic
 608ZZ with inserts, the fit between the top and bottom casings, the anemometer
 environment shield and rain gauge are totally different but include the
 great pipe mount concept.

 We are Metric so I choose 15mm copper pipe since its what I had available
 and the electronics are still on the way but will be Wemos based and log
 all data wirelessly. A solar panel will charge the built in 16850 more to
 come on that as soon as the parts arrive. 
*/

include <support.scad>

$fn=180;



// Spacer between moving parts and bearing
module bearing_spacer ()
{
	difference ()
	{
		union ()
		{
			cylinder (d=11, h=6);
			cylinder (d=8, h=9);
		}
		translate ([0,0,-1]) cylinder (d=5, h=16);
	}
}

// Inserts to mate M5 to 608ZZ's
module bearing_bush ()
{
	difference ()
	{
		union ()
		{
			cylinder (d=12, h=2);
			cylinder (d=8, h=5);
		}
		cylinder (d=5, h=6);
	}
}

// ========================= Anemometer "cups" ==============================
// After a bit of reseach this is the best shape that is easy
// to 3D print with good overall performance.
// See research PDF
module anemometer (stick=true)
{
	difference ()
	{
		union ()
		{
			for (rot = [0:120:240])
			{
				rotate ([0,0,rot]) translate ([-2,0,0])
				{
					cube ([4,60,4]);
					translate ([-38,40,0]) rotate([0,0,47]) linear_extrude (height=40) arc (40, 2, 90);
					translate ([3,41.5,0]) rotate([0,0,0]) cylinder (d=2,h=40);
					translate ([-39.5,81,0]) rotate([0,0,0]) cylinder (d=2,h=40);
					
					rotate ([0,0,60]) translate ([-15,0,0]) cube([30,10,4]);
				}
			}
		}
			
		for (rot = [0:120:240])
		{
			rotate ([0,0,rot]) translate ([-2,0,0])
			{
				translate ([-38,40,-1]) rotate([0,0,47]) linear_extrude (height=6) arc (42, 6, 90);
				
				rotate ([0,0,60]) translate ([1,38,-1]) cylinder (d=65,h=6);
			}
		}
		
		translate ([0,0,-1]) cylinder (d=5.0, h=6);
	}
	// Adhesion pads 
	if (stick)
	{
		for (rot = [0:120:240])
		{
			rotate ([0,0,rot])
			{
				translate ([-42,81,0]) rotate([0,0,0]) cylinder (d=18,h=0.5);
			}
		}
	}

}

// ========================= Anemometer "cups" ==============================
// After a bit of reseach this is the best shape that is easy
// to 3D print with good overall performance.
// See research PDF
module anemometer1 (stick=true)
{
	difference ()
	{
		union ()
		{
			for (rot = [0:120:240])
			{
				rotate ([0,0,rot]) translate ([-2,0,0])
				{
					cube ([4,58,4]);
					translate ([-33,45,0]) rotate([0,0,60]) linear_extrude (height=40) arc (35, 2, 120);
					translate ([3,45,0]) rotate([0,0,0]) cylinder (d=2,h=40);
					translate ([-51.05,76.15,0]) rotate([0,0,0]) cylinder (d=2,h=40);
					
					rotate ([0,0,60]) translate ([-15,0,0]) cube([30,10,4]);
				}
			}
		}

		for (rot = [0:120:240])
		{
			rotate ([0,0,rot]) translate ([-2,0,0])
			{
				rotate ([0,0,60]) translate ([1,38,-1]) cylinder (d=65,h=6);

				translate ([-33,45,-1]) rotate([0,0,45]) linear_extrude (height=6) arc (37, 6, 90);
				
							}
		}
		
		translate ([0,0,-1]) cylinder (d=5.0, h=6);
	}

	// Adhesion pads 
	if (stick)
	{
		for (rot = [0:120:240])
		{
			rotate ([0,0,rot])
			{
				translate ([-57,76.1,0]) cylinder (d=18,h=0.5);
			}
		}
	}

}

// ======================= Anemometor Encoder ============================

module anemometer_encoder()
{
	difference ()
	{
		union ()
		{
			cylinder (d=51, h=3.6);
			cylinder (d=15, h=4.2);
		}
		
		translate ([0,0,-0.1]) cylinder (d=5,h=6.2);
		translate ([10,-0.5,-0.1]) cube ([20,1,5]);

	}
}


// ========================= Bottom Housing ==============================

module bottom ()
{
	
	difference ()
	{
		union ()
		{
			minkowski()
			{
				cylinder (d=71, h=1);
				sphere (2);
			}
		 
			translate ([0,0,1]) cylinder (d=75, h=34);
		}
		union ()
		{
			translate ([0,0,-3]) cylinder (d=13, h=15);			
			translate ([0,0,2]) cylinder (d=67, h=34);
			bottom_g_cuts ();			
		}
	}
	
	difference ()
	{
		union ()
		{
			translate ([0,0,2]) cylinder (d=28, h=9);
			rotate ([0,0,30]) translate ([27,0,2]) rotate ([0,0,9]) difference ()
            {
                cube ([9,10.2,18]);
                translate ([-0.1,2.1,4]) cube ([11,6.1,18]);
            }
            for (rot=[0,120,240])
			{
				rotate ([0,0,rot])
				{
					translate ([35,0,00]) rotate([0,0,180]) linear_extrude (height=35-1.7) arc (2, 6, 180);
				}
			}
			
		}
		
		union ()
		{
			translate ([0,0,-1]) cylinder (d=22.1, h=15);
			bottom_g_cuts ();
		}
	}
}

module bottom_g_cuts ()
{
	for (rot=[0,120,240])
	{
		rotate ([0,0,rot])
		{
			translate ([32.5,0,-3]) cylinder (d=8, h=30);
			translate ([32.5,0,21]) cylinder (d=3.2, h=16);
		}
	}
	translate ([0,0,33]) rotate([0,0,0]) linear_extrude (height=3) arc ((75-3.8)/2, 2.5, 360);
}

// ========================= Top Housing ==============================
module top ()
{
	
	difference ()
	{
		union ()
		{
			minkowski()
			{
				cylinder (d=71, h=1);
				sphere (2);
			}
		 
			translate ([0,0,1]) cylinder (d=75, h=34);
			rotate ([0,0,30]) translate ([33,0,20]) rotate ([0,90,0]) cylinder (d=15, h=24);
		}
		union ()
		{
			translate ([0,0,-3]) cylinder (d=13, h=15);			
			translate ([0,0,2]) cylinder (d=67, h=34);
			rotate ([0,0,30]) translate ([32,0,20]) rotate ([0,90,0]) cylinder (d=10, h=36);
			top_g_cuts ();			
		}
	}
	
	difference ()
	{
		union ()
		{
			translate ([0,0,2]) cylinder (d=28, h=9);
			for (rot=[0,120,240])
			{
				rotate ([0,0,rot])
				{
					translate ([35,0,20]) rotate([0,0,180]) linear_extrude (height=15-1.7) arc (2, 6, 180);
				}
			}
			
		}
		
		union ()
		{
			translate ([0,0,-1]) cylinder (d=22.1, h=15);
			top_g_cuts ();
		}
	}
}

module top_g_cuts ()
{
	for (rot=[0,120,240])
	{
		rotate ([0,0,rot])
		{
			translate ([32.5,0,21]) cylinder (d=2.8, h=16);
		}
	}
	translate ([0,0,33]) rotate([0,0,0]) linear_extrude (height=3) arc ((75-8.2)/2, 2.5, 360);

}
// ========================= magnometer Housing ==============================
module magnometer ()
{
	rotate ([0,180,0]) difference ()
	{
		cylinder (d=32, h=19);

		translate ([0,0,-0.01])cylinder (d=28.5, h=13);
		translate ([-5,-5,15]) cube ([15,13.5, 4.1]);
	}
}

// ========================= Vane Nut Cap ==============================
module vane_nut_cap ()
{
	difference ()
	{
		cylinder (d1=29, d2=10, h=8);
		translate ([0,0,-0.1])
		{
			cylinder (d=9, h=6, $fn=6);
			cylinder (d=14, h=1.5);
		}
	}
}
		
// ========================= Vane Cap ==============================
module vane_cap ()
{
	difference ()
	{
		union ()
		{
			translate ([0,0,10]) cylinder (d1=83, d2=30, h=25);
			cylinder (d=83, h=10.1);
		}
		
		vane_g_cuts ();
		translate ([0,0,-1]) cylinder (d=79, h=10);
		// Uncomment below if you want a hollow cup
		//translate ([0,0,9]) cylinder (d1=82, d2=24, h=22);
	}
	difference ()
	{
		translate ([0,0,8]) cylinder (d1=82.1, d2=24.1, h=22.1);
		
		translate ([7,-45,0])cube ([50,90,30]);
		translate ([-50-7,-45,0])cube ([50,90,30]);
		vane_g_cuts ();
	}
}

module vane_g_cuts ()
{
	translate ([0,0,-1]) cylinder (d=5.1, h=37);
	translate ([-4,-45,13]) cube ([8.2,90,8.2]);
}

// ========================= Vane Beams ==============================

module vane_beams ()
{
	difference ()
	{
		union ()
		{
			rcube ([8, 115, 8]);
			translate ([10,0,0]) rcube ([8,105,8]);
		}
		translate ([3.5,0,-1]) cube ([1.55,71,10]);
		translate ([-1,10,4]) rotate ([0,90,0]) cylinder (d=2.8, h=10);
		translate ([-1,50,4]) rotate ([0,90,0]) cylinder (d=2.8, h=10);
	}
}

module vane ()
{
	linear_extrude(height=1.5, center = true, convexity = 10)
		import (file = "vane.dxf", layer = "0");   
}

module vane_cone ()
{
	difference ()
	{
		scale([1,1,3]) sphere(12);
		
		translate ([-25,-25,-50]) cube ([50,50,50]);
		translate ([-4.05,-4.05,-1]) cube ([8.1,8.1,25]);
	}
}

module spacers ()
{
	for (o=[0,15])
	{
		translate ([o,0,0]) bearing_spacer ();
		translate ([o,15,0]) bearing_bush ();
	}
}

//========================= Environment Shelter ==============================


module ev_shelter_plate (top=false)
{
	difference ()
	{
		
		cylinder (d1=71, d2=80, h=20);

		if (top)
		{
			// ML8511 window, hold in place with marine silicone sealer
			translate ([-2,-2,-1]) cube ([4.1,4,3]);
		}
		else
		{
			translate ([0,0,-1]) cylinder (d=45, h=22);
		}
		translate ([0,0,1.5]) cylinder (d1=70, d2=79, h=22);
		if (!top)
		{
			ev_shelter_plate_g_cuts ();
		}
	}
	difference ()
	{
		for (rot = [0,120,240])
		{
			rotate ([0,0,rot]) translate ([30,0,0])
			{
				cylinder (d1=9,d2=7,h=15);
				translate ([0,0,1.8]) scale ([1,1,0.3]) sphere (d=11);
			}
		}
		ev_shelter_plate_g_cuts ();
	}
}

module ev_shelter_plate_g_cuts ()
{
	for (rot = [0,120,240])
	{
		rotate ([0,0,rot]) translate ([30,0,5])
		{
			cylinder (d=2.8,h=10.5);
		}
		rotate ([0,0,60+rot]) translate ([30,0,-1])
		{
			cylinder (d=3.2,h=3);
		}
	}
}

module ev_shelter_bottom ()
{
	
	difference ()
	{
		union ()
		{
			minkowski()
			{
				cylinder (d=71, h=23);
				sphere (2);
			}
			rotate ([0,0,30]) translate ([33,0,8]) rotate ([0,90,0]) cylinder (d=15, h=24);
		}
		union ()
		{
			translate ([0,0,2]) cylinder (d=67, h=26);
			for (g = [5,8,11,14])
			{
				translate ([g,-10,-2]) rcube ([1.8,20,6,0.7]);
			}
			ev_shelter_bottom_g_cuts ();
			rotate ([0,0,30]) translate ([33,0,8]) rotate ([0,90,0]) cylinder (d=10, h=36);
		}
	}
	
	difference ()
	{
		union ()
		{
			translate ([-8,-8,0]) rcube ([3,16,65]);
			for (rot=[0,120,240])
			{
				rotate ([0,0,rot])
				{
					translate ([30,0,0]) rotate([0,0,180])
					{
						linear_extrude (height=25) arc (2, 6, 180);
						translate ([-4,-8,0]) cube ([4,16,25]);
					}
					
				}
			}
			
		}
		
		union ()
		{
			translate ([-10,0,60]) rotate ([0,90,0]) cylinder (d=4, h=8);
			ev_shelter_bottom_g_cuts ();
		}
	}
}

module ev_shelter_bottom_g_cuts ()
{
	for (rot=[0,120,240])
	{
		rotate ([0,0,rot])
		{
			#translate ([30,0,-3]) cylinder (d=8, h=20);
			translate ([30,0,21]) cylinder (d=3.2, h=16);
		}
	}
	translate ([0,0,33]) rotate([0,0,0]) linear_extrude (height=3) arc ((75-3.8)/2, 2.5, 360);
}

//======================= Rain Gauge =============================

module rain_top ()
{
	difference ()
	{
		union ()
		{
			cylinder (d=71,h=60);
			linear_extrude (height=8) arc (70/2, 1.2, 360);
			translate ([0,0,8]) cylinder (d1=72.4, d2=70, h=3);
		}
		translate ([0,0,-1]) cylinder (d=67, d2=69,h=62);
		translate ([0,0,-0.1]) linear_extrude (height=8) arc (67/2, 1.2, 360);
		for (s=[0,120,240])
		{
			rotate ([0,0,s]) translate ([63/2,0,3]) rotate ([0,90,0])cylinder (d=3.1,h=8);
		}
		// Cutaway to check fit
		//translate ([0,-40,-0.1]) cube ([60,80,100]);

		
	}
	difference ()
	{
		translate ([0,0,60]) scale([1.5,1.5,2]) sphere(71/3);
		translate ([-40,-40,60]) cube (81);
		translate ([0,0,60]) scale([1.5,1.5,2]) sphere(69/3);
		cylinder (d=4, h=90);

	}
}

module rain_bottom ()
{
	difference ()
	{
		union ()
		{
			minkowski()
			{
				cylinder (d=67, h=1);
				sphere (2);
			}
			translate ([0,0,1]) cylinder (d=71,h=33);
			rotate ([0,0,90]) translate ([33,0,8]) rotate ([0,90,0]) cylinder (d=15, h=24);
			
		}

		translate ([0,0,28]) linear_extrude (height=8) arc (34.5, 3, 360);
		translate ([0,0,28]) cylinder (d1=67, d2=65,h=6.1);
		translate ([0,0,2]) cylinder (d=67,h=28);
		
		rotate ([0,0,90]) translate ([33,0,8]) rotate ([0,90,0]) cylinder (d=10, h=36);
		// Drainage
		for (g = [5,8,11,14])
		{
			translate ([g+15,-10,-2]) rcube ([1.8,20,6,0.7]);
			translate ([g-36,-10,-2]) rcube ([1.8,20,6,0.7]);
		}
		
		for (s=[0,120,240])
		{
			rotate ([0,0,s]) translate ([63/2,0,31]) rotate ([0,90,0])cylinder (d=2.8,h=8);
		}
	}
	
	difference ()
	{
		union ()
		{
			translate([-62.5/2,15.5,0]) rcube([62.5,2,26]);
			translate([-12,15.5,18]) rcube([24,6,16]);
			translate([-61/2,-18.5,0]) rcube([61,2,26]);
		}
		translate([0,25,12]) rotate([90,0,0]) cylinder(d=3,h=50);
		translate([-10.5,17,20]) rcube([21,2.5,15.5]);
		translate([-9.5,17,20]) cube([19.5,6,15.5]);
	}
}

module rain_bucket_half ()
{
	difference ()
	{
		{
			union ()
			{
				cube ([30,25,20]);
				translate ([0,28,0]) rotate ([0,270,90]) linear_extrude (height=31) arc (0, 5, 180);
			}
			
		}
		translate ([0,29,2]) rotate ([90,90,0]) cylinder (d=3.1,h=33);
		translate ([1,2,6]) rcube ([30,21,15]);
		translate ([1,2,6]) rotate ([0,8,0]) cube ([30,21,15]);
		translate ([3,-1,20]) rotate ([0,25,0]) cube ([35,27,15]);		
		translate ([0,1.75,16]) rotate ([90,90,0]) cylinder (d=6.1,h=4.1);
	}
}
module rain_bucket ()
{
	rain_bucket_half ();
	mirror([1,0,0]) rain_bucket_half ();
}

//======================= WeMoss Box=============================
// Dont have the hardware yet so this is an estimation from a photo

HOFF = 6;
WT=2;
LHS=87;
SHS=21.6;
BW = SHS+HOFF+WT*2;
module wemos_box ()
{
	difference ()
	{
		
		union ()
		{
			translate ([WT/2,WT/2,0]) rcube ([96+20, BW,25]);
			rcube ([98+20,BW+WT,23]);
		}
		translate ([WT,WT,WT]) rcube ([94+20, BW-WT,25]);
		wemos_box_gcuts ();
		
		// Hole for wire T-Junction
		translate ([98+8,(BW+2)/2,-0.1]) cylinder (d=16, h=5); 
	}
	
	difference ()
	{
		union ()
		{
			translate ([HOFF,HOFF,0]) cylinder (d=6,h=22);
			translate ([HOFF,HOFF+SHS,0]) cylinder (d=6,h=22);

			translate ([HOFF+LHS,HOFF,0]) cylinder (d=6,h=22);
			translate ([HOFF+LHS,HOFF+SHS,0]) cylinder (d=6,h=22);
		}
		wemos_box_gcuts ();
	}
	
	// The lid
	translate ([0,40,0])
	{
		difference ()
		{
			rcube ([98+20,BW+2,7.6]);
			
			translate ([2,2,2]) rcube ([94+20, BW-WT,8]);
			translate ([1,1,4]) rcube ([96+20, BW,8]);
			
			wemos_box_gcuts ();
		}
		
		difference ()
		{
			union ()
			{
				translate ([HOFF,HOFF,0]) cylinder (d=6,h=7);
				translate ([HOFF+LHS,HOFF,0]) cylinder (d=6,h=7);
				translate ([HOFF,HOFF+SHS,0]) cylinder (d=6,h=7);
				translate ([HOFF+LHS,HOFF+SHS,0]) cylinder (d=6,h=7);
			}
			wemos_box_gcuts ();
		}
	}
}

module wemos_pole_clamps ()
{
	for  (o = [0,12,24,36])
		translate ([o,0,0])difference ()
	{
		rcube ([8,BW,8.5]);
			
		rotate ([0,90,0]) translate ([-9.5,BW/2,-1]) cylinder (d=16, h= 12);
		translate([-2,-WT/2,0]) wemos_box_gcuts ();
	}
}

module wemos_box_gcuts ()
{
	translate ([HOFF,HOFF,-0.1]) cylinder (d=3.1,h=30);
	translate ([HOFF+LHS,HOFF,-0.1]) cylinder (d=3.1,h=30);
	translate ([HOFF,HOFF+SHS,-0.1]) cylinder (d=3.1,h=30);
	translate ([HOFF+LHS,HOFF+SHS,-0.1]) cylinder (d=3.1,h=30);
}

//======================= Solar Panel Box =============================
// Dont have the hardware yet so this is an estimation from a photo
module solar_box ()
{
	difference ()
	{
		rcube ([135, 91, 6]);
		
		translate ([12, 5, 1]) cube ([111, 81,6]);
		translate ([10,3,4]) cube ([115, 85,6]);
		
		// Hole for wire T-Junction
		translate ([100,91/2,-0.1]) cylinder (d=16, h=5);
		
		translate ([4.5,91/2-21/2,-0.1]) cylinder (d=3.1,h=30);
		translate ([4.5,91/2+21/2,-0.1]) cylinder (d=3.1,h=30);
		
		translate ([135-4.5,91/2-21/2,-0.1]) cylinder (d=3.1,h=30);
		translate ([135-4.5,91/2+21/2,-0.1]) cylinder (d=3.1,h=30);
	}
}

cylinder (d=10, h=0.5);
//solar_box (); // Uses same clamps as wemos
//translate ([0,-35,0]) wemos_pole_clamps ();

//wemos_box();
//translate ([0,-35,0]) wemos_pole_clamps ();
//magnometer ();
//anemometer_encoder ();
//anemometer1 ();
//rain_top ();
//rain_bottom();
//rain_bottom (); translate([0,0,28.1+8]) rain_top ();
//rain_bucket ();
//rain_bottom (); //translate ([0,12,10]) rotate ([0,0,180]) color ("green") rain_bucket ();

//spacers ();
//bottom (); rotate ([0,180,60]) color ("silver") translate ([0,0,-68.1]) top ();
//top ();\
//difference ()
//bottom ();
//vane ();
//vane_nut_cap ();
//vane_cap ();
//vane_cone ();
//vane_beams ();

//ev_shelter_bottom ();
/*
	for (os=[40,55,70,85])
	{
		translate ([0,0,os]) rotate([0,180,60]) ev_shelter_plate (os==85);
	}
*/

//ev_shelter_plate (true);	// Top
//ev_shelter_plate ();
//translate ([0,0,15]) ev_shelter_plate ();
