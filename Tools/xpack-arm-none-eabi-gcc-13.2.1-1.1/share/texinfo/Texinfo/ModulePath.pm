# @configure_input@
#
# Add directories to @INC, Perl's module search path, to find modules,
# either in the source or build directories.
#
# 'use Texinfo::ModulePath' should only occur once per program so that
# the initialization is only done once.  This makes the way this module
# is used very unusual.

package Texinfo::ModulePath;

use vars qw($VERSION);

$VERSION = '6.3dev';

use File::Basename;
use File::Spec;

# Used as a flag to say whether to look for data files in uninstalled 
# locations.
our $texinfo_uninstalled = 0;

# uninstalled locations paths, if $Texinfo::ModulePath::texinfo_uninstalled
# Pathname of the tp/ build directory.  Used to find the locale
# data.
our $builddir = '';

our $top_builddir;
our $top_srcdir;

# installed locations paths, if not $Texinfo::ModulePath::texinfo_uninstalled
our $pkgdatadir;

# If $LIB_DIR and $LIBEXEC_DIR are given,
# (likely the installation directories)
# use them to add directories
# to @INC.
#
# LIB_DIR is for bundled libraries.
# LIBEXEC_DIR is for XS modules.
# PKGDATADIR is for HTML extensions, javascript
#
# otherwise use 'top_srcdir'
# and 'top_builddir' environment variables.
sub init {
  my $lib_dir = shift;
  my $libexec_dir = shift;
  $pkgdatadir = shift;
  my %named_args = @_;

  if (!$named_args{'installed'}) {
    $texinfo_uninstalled = 1;

    if ($ENV{'top_srcdir'}) {
      $top_srcdir = $ENV{'top_srcdir'};
    } elsif (defined $named_args{'updirs'}) {
      my ($real_command_name, $command_directory, $command_suffix) 
              = fileparse($0, '.pl');
      my $updir = File::Spec->updir();

      # e.g. tp/t -> tp/t/../.. for 'updirs' = 2.
      my $count = $named_args{'updirs'};
      $top_srcdir = $command_directory;
      while ($count-- > 0) {
        $top_srcdir = File::Spec->catdir($top_srcdir, $updir);
      }
    }
     
    if (defined($top_srcdir)) {
      # For Texinfo::Parser and the rest.
      unshift @INC, File::Spec->catdir($top_srcdir, 'tp');
      $lib_dir = File::Spec->catdir($top_srcdir, 'tp', 'maintain');
    }

    # Find XS modules in the build directory
    if (defined($ENV{'top_builddir'})) {
      $top_builddir = $ENV{'top_builddir'};
    } else {
      $top_builddir = $top_srcdir;
    }
    if (defined($top_builddir)) {
      unshift @INC, File::Spec->catdir($top_builddir, 'tp',
        'Texinfo', 'XS');
      unshift @INC, File::Spec->catdir($top_builddir, 'tp',
        'Texinfo', 'XS', 'parsetexi');

      $builddir = File::Spec->catdir($top_builddir, 'tp');
    }

  } else {
    # for the Texinfo modules
    if (defined($lib_dir)) {
      unshift @INC, $lib_dir;
    }
    # for the XS libraries
    if (defined($libexec_dir)) {
      unshift @INC, $libexec_dir;
    }
  }

  if (defined($lib_dir)) {
    # Bundled libraries
    # '@USE_EXTERNAL_LIBINTL @' and similar are substituted
    if ('no' ne 'yes') {
      unshift @INC, (File::Spec->catdir($lib_dir, 'lib', 'libintl-perl', 'lib'));
    }
    if ('no' ne 'yes') {
      unshift @INC, (File::Spec->catdir($lib_dir, 'lib', 'Unicode-EastAsianWidth', 'lib'));
    }
    if ('no' ne 'yes') {
      unshift @INC, (File::Spec->catdir($lib_dir, 'lib', 'Text-Unidecode', 'lib'));
    }
  }

}

sub import { 
  my $class = shift;
  goto &init;
}

1;
