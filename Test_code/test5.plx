use lib '.';
use test5_0;
use test5_1;
use test5_2;

@geometry = (
	Circle->new(x => 1, y => 2, z => 3, r => 5),
	Circle->new(x => 3, y => 2, z => 1, r => 5),
	Sphere->new(x => 1, y => 3, z => 2, r => 55),
	Point->new(x => 1, y => 5, z => 3),
	Circle->new(x => 2, y => 2, z => 2, r => 2)
);
foreach $a (@geometry) {
	print $a->draw();
};