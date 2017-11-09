use lib '.';
use test3;

my $p = Person->new(
	surname => "Galilei",
	forename => "Galileo",
	address => "9.81 Pisa Apts.",
	occupation => "bombadier"
);
my $object2 = Person->new (
	surname => "Einstein",
	forename => "Albert",
	address => "9E16, Relativity Drive",
	occupation => "Plumber"
);
print "Population now: ", Person->population, "\n";