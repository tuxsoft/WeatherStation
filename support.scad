// Weather Station support functions
//
// Copyright (c) R. Linder
// This work is licensed under a Creative Commons Attribution 4.0 International License.

// 

include <MCAD/shapes.scad>

$fn=180;

module rcube (dim)
{
	if (len(dim) == undef)
	{
		translate ([dim/2,dim/2,dim/2]) roundedBox (dim,dim,dim,1);
	}
	else if (len (dim) == 4)
	{
		translate ([dim[0]/2,dim[1]/2,dim[2]/2]) roundedBox (dim[0],dim[1],dim[2],dim[3]);
	}
	else
	{
		translate ([dim[0]/2,dim[1]/2,dim[2]/2]) roundedBox (dim[0],dim[1],dim[2],1);
	}
}
	
// Arc module from internet examples

module arc (radius, thick, angle)
{
    intersection ()
    {
        union ()
        {
            rights = floor (angle/90);
            remain = angle-rights*90;
            if (angle > 90)
            {
                for (i = [0:rights-1])
                {
                    rotate (i*90- (rights-1) *90/2)
                    {
                        polygon ([[0, 0], [radius+thick, (radius+thick) *tan (90/2)], [radius+thick, - (radius+thick) *tan (90/2)]]);
                    }
                }
                rotate (- (rights) *90/2)
                polygon ([[0, 0], [radius+thick, 0], [radius+thick, - (radius+thick) *tan (remain/2)]]);
                rotate ((rights) *90/2)
                polygon ([[0, 0], [radius+thick, (radius+thick) *tan (remain/2)], [radius+thick, 0]]);
            }
            else
            {
                polygon ([[0, 0], [radius+thick, (radius+thick) *tan (angle/2)], [radius+thick, - (radius+thick) *tan (angle/2)]]);
            }
        }
        difference ()
        {
            circle (radius+thick);
            circle (radius);
        }
    }
}
