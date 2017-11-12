package Point;

sub new {
	my $class = shift;
	my $self = {@_};
	bless ($self, $class);
	return $self;
};

sub x {
	my $self = shift;
	return $self->{x};
};
sub y {
	my $self = shift;
	return $self->{y};
};
sub z {
	my $self = shift;
	return $self->{z};
};

sub draw {
	my $self = shift;
	return "A Point was drawn at (" . ($self->{x}) . ", " 
	. ($self->{y}) . ", " . ($self->{z}) . ")\n";
};

1;