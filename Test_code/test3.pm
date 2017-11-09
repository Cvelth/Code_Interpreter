package Person;

my $Population = 0;

sub new {
	my $class = shift;
	my $self = {@_};
	bless ($self, $class);
	$Population++;
	return $self;
}

sub surname {
	my $self = shift;
	return $self->{surname};
}

sub population {
	return $Population
}

1;