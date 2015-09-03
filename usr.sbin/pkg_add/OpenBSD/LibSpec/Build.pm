# ex:ts=8 sw=4:
# $OpenBSD: Build.pm,v 1.6 2010/11/14 07:24:06 espie Exp $
#
# Copyright (c) 2010 Marc Espie <espie@openbsd.org>
#
# Permission to use, copy, modify, and distribute this software for any
# purpose with or without fee is hereby granted, provided that the above
# copyright notice and this permission notice appear in all copies.
#
# THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
# WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
# MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
# ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
# WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
# ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
# OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
#
use strict;
use warnings;

# the specs used during build are slightly different from the specs at
# runtime.
package OpenBSD::Library::Static;
our @ISA = qw(OpenBSD::Library);
sub new
{
	my ($class, $dir, $stem) = @_;
	bless {dir => $dir, stem => $stem}, $class;
}

sub no_match_dispatch
{
	my ($library, $spec, $base) = @_;
	return $spec->no_match_static($library, $base);
}

sub to_string
{
	my $self = shift;
	return "$self->{dir}/lib$self->{stem}.a";
}

sub version { ".a" }

sub is_static { 1 }

sub is_better { 0 }

package OpenBSD::Library::Build;
our @ISA = qw(OpenBSD::Library);

sub static
{ 'OpenBSD::Library::Static'; }

sub from_string
{
	my ($class, $filename) = @_;
	if (my ($dir, $stem) = $filename =~ m/^(.*)\/lib([^\/]+)\.a$/o) {
		return $class->static->new($dir, $stem);
	} else {
		return $class->SUPER::from_string($filename);
	}
}

package OpenBSD::LibSpec;
sub no_match_static
{
	&OpenBSD::LibSpec::no_match_name;
}

sub findbest
{
	my ($spec, $repo, $base) = @_;
	my $spec2 = OpenBSD::LibSpec::GT->new($spec->{dir}, $spec->{stem},
	    0, 0);
	my $r = $spec2->lookup($repo, $base);
	my $best;
	for my $candidate (@$r) {
		if (!defined $best || $candidate->is_better($best)) {
			$best = $candidate;
		}
	}
	if (defined $best) {
		if ($best->is_static) {
			return $best;
		}
		if ($spec->match($best, $base)) {
			return $best;
		}
	}
	return undef;
}

package OpenBSD::LibSpec::GT;
our @ISA = qw(OpenBSD::LibSpec);
sub no_match_major
{
	my ($spec, $library) = @_;
	return $spec->major > $library->major;
}

sub to_string
{
	my $self = shift;
	return $self->key.">=".$self->major.".".$self->minor;

}


package OpenBSD::LibSpec::Build;
our @ISA = qw(OpenBSD::LibSpec);

sub new_from_string
{
	my ($class, $string) = @_;

	$string =~ s/\.$//;
	if (my ($stem, $strict, $major, $minor) = $string =~ m/^(.*?)(\>?)\=(\d+)\.(\d+)$/o) {
		return $class->new_object($stem, $strict, $major, $minor);
	} elsif (($stem, $strict, $major) = $string =~ m/^(.*?)(\>?)\=(\d+)$/o) {
		return $class->new_object($stem, $strict, $major, 0);
	} else {
		return $class->new_object($string, '>', 0, 0);
	}
}

sub new_object
{
	my ($class, $stem, $strict, $major, $minor) = @_;
	my $n = $strict eq '' ? "OpenBSD::LibSpec" : "OpenBSD::LibSpec::GT";
	return $n->new_with_stem($stem, $major, $minor);
}

1;
