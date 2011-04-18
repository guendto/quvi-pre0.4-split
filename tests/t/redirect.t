
use warnings;
use strict;

use Test::More;

eval "use JSON::XS";
plan skip_all => "JSON::XS required for testing" if $@;

eval "use Test::Deep";
plan skip_all => "Test::Deep required for testing" if $@;

plan tests => 4;

use Test::Quvi;

my $q = Test::Quvi->new;

# Test self.redirect in the academicearth.lua script.
my $u =
  "http://www.academicearth.org/lectures/intro-roman-architecture";

my ($r, $o) = $q->run($u, "-q");

is($r, 0, "quvi exit status == 0")
  or diag $u;

my $f = "data/resolve/inscript_redirect.json";
my $j = $q->get_json_obj;

SKIP:
{
    skip 'quvi exit status != 0', 1 if $r != 0;
    my $e = $q->read_json($f, 1)
      ;    # 1=prepend --data-root (if specified in cmdline)
    cmp_deeply($j->decode($o), $e, "compare with $f")
      or diag $u;
}

# Test a typical URL shortener support.
$u = "http://is.gd/EbVFoa";    # -> http://vimeo.com/1485507

($r, $o) = $q->run($u, "-q");

is($r, 0, "quvi exit status == 0")
  or diag $u;

SKIP:
{
    skip 'quvi exit status != 0', 1 if $r != 0;
    $f = "data/format/default/vimeo.json";
    my $e = $q->read_json($f, 1)
      ;    # 1=prepend --data-root (if specified in cmdline)
    cmp_deeply($j->decode($o), $e, "compare with $f")
      or diag $u;
}
