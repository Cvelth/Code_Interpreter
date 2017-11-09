use lib '.';
use test4;

my $p = Student->new(
	surname => "Galilei",
	forename => "Galileo",
	address => "9.81 Pisa Apts.",
	occupation => "student",
	grade => "B"
);
my $object2 = Student->new (
	surname => "Einstein",
	forename => "Albert",
	address => "9E16, Relativity Drive",
	occupation => "student",
	grade => "A"
);
print $p->surname, "\t\t", $p->grade, "\n";
print $object2->surname, "\t", $object2->grade, "\n";