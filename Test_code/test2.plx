use lib '.';
use test2;

my $p = Person->new(
	surname => "Galilei",
	forename => "Galileo",
	address => "9.81 Pisa Apts.",
	occupation => "bombadier"
);
print "Surname: " . ($p->surname) . "\n";