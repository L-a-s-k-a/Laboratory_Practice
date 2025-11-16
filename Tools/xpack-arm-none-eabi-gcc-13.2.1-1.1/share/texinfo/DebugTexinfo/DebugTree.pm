# DebugTexinfo::DebugTree.pm: debug a Texinfo::Parser tree.
#
# Copyright 2011, 2012, 2013 Free Software Foundation, Inc.
# 
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3 of the License,
# or (at your option) any later version.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
# 
# Original author: Patrice Dumas <pertusus@free.fr>

# Example of calls
# with creation of elements corresponding to sections:
# ./texi2any.pl --set TEXINFO_OUTPUT_FORMAT=debugtree --set USE_NODES=0 file.texi
# with creation of elements corresponding to nodes:
# ./texi2any.pl --set TEXINFO_OUTPUT_FORMAT=debugtree --set USE_NODES=1 file.texi
# no elements
# ./texi2any.pl --set TEXINFO_OUTPUT_FORMAT=debugtree file.texi
# 
# Some unofficial info about the --debug command line option ... with
# --debug=1, the tree is not printed,
# --debug=10 (or more), the tree is printed at the end of the run,
# --debug=100 (or more), the tree is printed at each newline.

use strict;

package DebugTexinfo::DebugTree;

# also for __(
use Texinfo::Common;
use Texinfo::Structuring;
use Texinfo::Convert::Converter;

use vars qw($VERSION @ISA);
@ISA = qw(Texinfo::Convert::Converter);

my %defaults = (
  'EXTENSION' => 'debugtree',
  'OUTFILE' => '-',
);

sub converter_defaults($$)
{
  return %defaults;
}

sub output($$)
{
  my $self = shift;
  my $root = shift;

  my ($output_file, $destination_directory) = $self->determine_files_and_directory();
  # REMARK for this format SPLIT does not means diverse files created
  # so the way the directory is determined/created if there is already
  # a file with the same name does not make sense for this format.
  # Given that this format is only to be used for debugging, this is
  # not an issue that really needs fixing.

  my $elements;
  if ($self) {
    if ($self->get_conf('USE_NODES')) {
      $elements = Texinfo::Structuring::split_by_node($root);
    } elsif (defined($self->get_conf('USE_NODES'))) {
      #print STDERR "U sections\n";
      $elements = Texinfo::Structuring::split_by_section($root);
    }
    # Currently the information added is not used further.
    if ($elements and ($self->get_conf('SPLIT') 
                       or !$self->get_conf('MONOLITHIC'))) {
      #print STDERR "S ".$self->get_conf('SPLIT')."\n";
      Texinfo::Structuring::split_pages($elements,
                                        $self->get_conf('SPLIT'));
    }
  }
  if ($elements) {
    $root = {'type' => 'elements_root',
             'contents' => $elements };
  }

  my ($encoded_destination_directory, $dir_encoding)
    = $self->encoded_output_file_name($destination_directory);
  my $succeeded
    = $self->create_destination_directory($encoded_destination_directory,
                                          $destination_directory);
  return undef unless $succeeded;

  my $fh;
  my $encoded_output_file;
  if (! $output_file eq '') {
    my $path_encoding;
    ($encoded_output_file, $path_encoding)
      = $self->encoded_output_file_name($output_file);
    my $error_message;
    ($fh, $error_message) = Texinfo::Common::output_files_open_out(
                             $self->output_files_information(), $self,
                                     $encoded_output_file);
    if (!$fh) {
      $self->document_error($self,
           sprintf(__("could not open %s for writing: %s"),
                                    $output_file, $error_message));
      return undef;
    }
  }
  my $result = $self->write_or_return(_print_tree($self, $root), $fh);
  # NOTE that we close STDOUT too here
  if ($fh) {
    Texinfo::Common::output_files_register_closed(
             $self->output_files_information(), $encoded_output_file);
    if (!close ($fh)) {
      $self->document_error($self,
               sprintf(__("error on closing %s: %s"),
                                    $output_file, $!));
      return undef;
    }
  }
  return $result;
}

sub convert($$)
{
  my $self = shift;
  my $root = shift;

  return _print_tree($self, $root);
}

sub convert_tree($$)
{
  my $self = shift;
  my $root = shift;

  return _print_tree($self, $root);
}

sub _print_tree($$;$$);

sub _protect_text($)
{
  my $text = shift;
  $text =~ s/\n/\\n/g;
  $text =~ s/\f/\\f/g;
  $text =~ s/\r/\\r/g;
  return $text;
}

sub _print_tree($$;$$)
{
  my $self = shift;
  my $element = shift;
  my $level = shift;
  my $argument = shift;
  $level = 0 if (!defined($level));

  my $result = ' ' x $level;
  if ($argument) {
    $result .= '%';
    $level++;
  }
  if ($element->{'cmdname'}) {
    $result .= "\@$element->{'cmdname'} ";
  }
  if (defined($element->{'type'})) {
    $result .= "$element->{'type'} ";
  }
  if (defined($element->{'text'})) {
    my $text = _protect_text($element->{'text'});
    $result .= "|$text|";
  }
  if ($element->{'extra'}
      and defined($element->{'extra'}->{'spaces_before_argument'})) {
    $result .= ' '
    .'b/'._protect_text($element->{'extra'}->{'spaces_before_argument'}).'/';
  }
  if ($element->{'extra'}
      and defined($element->{'extra'}->{'spaces_after_argument'})) {
    $result .= ' '
    .'a/'._protect_text($element->{'extra'}->{'spaces_after_argument'}).'/';
  }
  $result .= "\n";
  if ($element->{'extra'}
      and defined($element->{'extra'}->{'comment_at_end'})) {
    $result .= ' ' x ($level + 1).'/comment_at_end/'."\n";
    $result .= _print_tree ($self, $element->{'extra'}->{'comment_at_end'},
                            $level +2);
  }
  if ($element->{'args'}) {
    foreach my $arg (@{$element->{'args'}}) {
      $result .= _print_tree ($self, $arg, $level +1, 1);
    }
  }
  if ($element->{'contents'}) {
    foreach my $content (@{$element->{'contents'}}) {
      $result .= _print_tree ($self, $content, $level+1);
    }
  }
  return $result;
}
