package Sphere;
use test5_0;

our @ISA = Point;

sub draw {
	my $self = shift;
	return "A Sphere was drawn at (" 
	. ($self->{x}) . ", " . ($self->{y}) . ", " . ($self->{z}) . ") "
	. "with radius: " . ($self->{r}) . "\n";
};

1;