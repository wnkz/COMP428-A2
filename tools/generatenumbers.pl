#!/usr/bin/perl

use warnings;
use strict;

my $n = 100;
my $max = 1000;
my $min = 0;

if (scalar @ARGV == 1) {
  $n = $ARGV[0];
} elsif (scalar @ARGV == 2) {
  $n = $ARGV[0];
  $max = $ARGV[1];
} elsif (scalar @ARGV == 3) {
  $n = $ARGV[0];
  $max = $ARGV[1];
  $min = $ARGV[2];
}

my %cache;
for (1..$n) {
  my $x = int(rand($max) - $min + 1);
  redo if exists $cache{$x};
  $cache{$x} = $_;
}

my $i = 1;
while ( my ($key, $value) = each(%cache) ) {
  print $key;
  if ($i == $n) {
    print "\n";
  } else {
    print ",";
  }
  $i++;
}
