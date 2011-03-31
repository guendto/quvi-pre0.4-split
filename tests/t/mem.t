
use warnings;
use strict;

use Test::More;

eval "use JSON";
plan skip_all => "JSON::XS required for testing" if $@;

use Test::Quvi;

my $q = Test::Quvi->new;
my $c = $q->get_config;

plan skip_all =>
  'valgrind required for testing, specify with --valgrind-path'
  unless $c->{valgrind_path};

plan tests => 2;

my ($r) =
  $q->run_with_valgrind('http://vimeo.com/1485507', '--support -qs');
is($r, 0, "exit status == 0");

($r) = $q->run_with_valgrind('http://vimeo.com/1485507', '-qs');
is($r, 0, "exit status == 0");
