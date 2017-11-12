package Student;
use test3;

our @ISA = Person;

sub grade {
	my $self = shift;
	return $self->{grade}
};

1;