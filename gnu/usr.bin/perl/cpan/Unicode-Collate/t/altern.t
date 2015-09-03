
BEGIN {
    unless ('A' eq pack('U', 0x41)) {
	print "1..0 # Unicode::Collate cannot pack a Unicode code point\n";
	exit 0;
    }
    unless (0x41 == unpack('U', 'A')) {
	print "1..0 # Unicode::Collate cannot get a Unicode code point\n";
	exit 0;
    }
    if ($ENV{PERL_CORE}) {
	chdir('t') if -d 't';
	@INC = $^O eq 'MacOS' ? qw(::lib) : qw(../lib);
    }
}

use strict;
use warnings;
BEGIN { $| = 1; print "1..37\n"; }
my $count = 0;
sub ok ($;$) {
    my $p = my $r = shift;
    if (@_) {
	my $x = shift;
	$p = !defined $x ? !defined $r : !defined $r ? 0 : $r eq $x;
    }
    print $p ? "ok" : "not ok", ' ', ++$count, "\n";
}

use Unicode::Collate;

ok(1);

#########################

sub _pack_U   { Unicode::Collate::pack_U(@_) }
sub _unpack_U { Unicode::Collate::unpack_U(@_) }

my $A_acute = _pack_U(0xC1);
my $acute   = _pack_U(0x0301);

my $Collator = Unicode::Collate->new(
  table => 'keys.txt',
  normalization => undef,
);

my %origAlt = $Collator->change(alternate => 'Blanked');

ok($Collator->lt("death", "de luge"));
ok($Collator->lt("de luge", "de-luge"));
ok($Collator->lt("de-luge", "deluge"));
ok($Collator->lt("deluge", "de\x{2010}luge"));
ok($Collator->lt("deluge", "de Luge"));

$Collator->change(alternate => 'Non-ignorable');

ok($Collator->lt("de luge", "de Luge"));
ok($Collator->lt("de Luge", "de-luge"));
ok($Collator->lt("de-Luge", "de\x{2010}luge"));
ok($Collator->lt("de-luge", "death"));
ok($Collator->lt("death", "deluge"));

$Collator->change(alternate => 'Shifted');

ok($Collator->lt("death", "de luge"));
ok($Collator->lt("de luge", "de-luge"));
ok($Collator->lt("de-luge", "deluge"));
ok($Collator->lt("deluge", "de Luge"));
ok($Collator->lt("de Luge", "deLuge"));

$Collator->change(alternate => 'Shift-Trimmed');

ok($Collator->lt("death", "deluge"));
ok($Collator->lt("deluge", "de luge"));
ok($Collator->lt("de luge", "de-luge"));
ok($Collator->lt("de-luge", "deLuge"));
ok($Collator->lt("deLuge", "de Luge"));

$Collator->change(%origAlt);

ok($Collator->{alternate}, 'shifted');

##############

# ignorable after alternate

# Shifted;
ok($Collator->eq("?\x{300}!\x{301}\x{315}", "?!"));
ok($Collator->eq("?\x{300}A\x{301}", "?$A_acute"));
ok($Collator->eq("?\x{300}", "?"));
ok($Collator->eq("?\x{344}", "?")); # U+0344 has two CEs.

$Collator->change(level => 3);
ok($Collator->eq("\cA", "?"));

$Collator->change(alternate => 'blanked', level => 4);
ok($Collator->eq("?\x{300}!\x{301}\x{315}", "?!"));
ok($Collator->eq("?\x{300}A\x{301}", "?$A_acute"));
ok($Collator->eq("?\x{300}", "?"));
ok($Collator->eq("?\x{344}", "?")); # U+0344 has two CEs.

$Collator->change(level => 3);
ok($Collator->eq("\cA", "?"));

$Collator->change(alternate => 'Non-ignorable', level => 4);

ok($Collator->lt("?\x{300}", "?!"));
ok($Collator->gt("?\x{300}A$acute", "?$A_acute"));
ok($Collator->gt("?\x{300}", "?"));
ok($Collator->gt("?\x{344}", "?"));

$Collator->change(level => 3);
ok($Collator->lt("\cA", "?"));

$Collator->change(alternate => 'Shifted', level => 4);

