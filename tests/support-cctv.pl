my $rc = quviTest::run_t("cctv"); # single-segment
$rc = quviTest::run("-t", "12474")  if !$rc; # multi-segment
exit $rc;
