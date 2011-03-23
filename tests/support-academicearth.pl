my $r = quviTest::run(
"http://www.academicearth.org/lectures/building-dynamic-websites-http",
"--page-title",
    "HTTP",
"--media-id",
    "har_buildsite_lecture1876",
"--file-length",
    "419838936",
"--file-suffix",
    "flv",
"--no-shortened"
);

exit $r if $r;

exit quviTest::run(
"http://www.academicearth.org/lectures/intro-roman-architecture",
"--page-title",
    "1. Introduction to Roman Architecture",
"--media-id",
    "qd3MJPHaotQ",
"--file-length",
    "78035755",
"--file-suffix",
    "flv",
"--no-shortened"
);
