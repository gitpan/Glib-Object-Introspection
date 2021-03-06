use Config;
use Glib::Object::Introspection;
use Test::More;

unless (-e qq(build/libregress.$Config{dlext}) &&
        -e qq(build/libgimarshallingtests.$Config{dlext}))
{
  plan skip_all => 'Need the test libraries';
}

if ($^O eq 'MSWin32') {
  unless (defined $ENV{PATH} &&
          $ENV{PATH} =~ m/\bbuild\b/)
  {
    plan skip_all => 'Need "build" in PATH';
  }
}
else {
  unless (defined $ENV{LD_LIBRARY_PATH} &&
          $ENV{LD_LIBRARY_PATH} =~ m/\bbuild\b/)
  {
    plan skip_all => 'Need "build" in LD_LIBRARY_PATH';
  }
}

Glib::Object::Introspection->setup(
  basename => 'Regress',
  version => '1.0',
  package => 'Regress',
  search_path => 'build');

Glib::Object::Introspection->setup(
  basename => 'GIMarshallingTests',
  version => '1.0',
  package => 'GI',
  search_path => 'build');

# Inspired by Test::Number::Delta
sub delta_ok ($$;$) {
  my ($a, $b, $msg) = @_;
  ok (abs ($a - $b) < 1e-6, $msg);
}

sub check_gi_version {
  my ($x, $y, $z) = @_;
  #return !system ('pkg-config', "--atleast-version=$x.$y.$z", 'gobject-introspection-1.0');
  return Glib::Object::Introspection->CHECK_VERSION ($x, $y, $z);
}

1;
