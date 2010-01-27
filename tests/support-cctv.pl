my $rc = quviTest::run("-t", "cctv"); # single-segment
$rc = quviTest::run("-t", "12474")  if !$rc; # multi-segment
exit $rc;
