#!/usr/bin/env perl

BEGIN { require './t/inc/setup.pl' };

use strict;
use warnings;

plan tests => 16;

my $data = 42;
my $result = 23;
my $callback  = sub { is shift, $data; return $result; };

is (Regress::test_callback_user_data ($callback, $data), $result);

is (Regress::test_callback_destroy_notify ($callback, $data), $result);
is (Regress::test_callback_destroy_notify ($callback, $data), $result);
is (Regress::test_callback_thaw_notifications (), 46);

Regress::test_callback_async ($callback, $data);
Regress::test_callback_async ($callback, $data);
is (Regress::test_callback_thaw_async (), $result);

my $obj = Regress::TestObj->new_callback ($callback, $data);
isa_ok ($obj, 'Regress::TestObj');
is (Regress::test_callback_thaw_notifications (), 23);
