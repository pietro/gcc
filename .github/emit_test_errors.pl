#!/bin/perl -n

#From https://github.com/Rust-GCC/gccrs/blob/master/.github/emit_test_errors.pl

sub analyze_errors() {
    /^(FAIL|ERROR|XPASS):\s([^:\s]+):?\s+(.+)/;

    my $type     = $1;
    my $filename = $2;
    my $message  = $3;
    my $line;

    if ( !$type ) {
        return;
    }

    if ( $message =~ /(at line (\d+))?.+(test for \w+, line (\d+))/g ) {
        $line = $2 || $4;
    }

    my $command = "::error file=gcc/testsuite/$filename";
    if ($line) {
        $command = "$command,line=$line";
    }

    print "$command,title=Test failure ($type)::$message\n";
}

analyze_errors();
