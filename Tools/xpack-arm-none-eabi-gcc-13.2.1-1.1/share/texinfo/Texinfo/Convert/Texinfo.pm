# Texinfo.pm: output a Texinfo tree as Texinfo.
#
# Copyright 2010-2022 Free Software Foundation, Inc.
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
# Parts (also from Patrice Dumas) come from texi2html.pl or texi2html.init.

package Texinfo::Convert::Texinfo;

use 5.00405;
use strict;

# To check if there is no erroneous autovivification
#no autovivification qw(fetch delete exists store strict);

use Carp qw(cluck confess);

# commands definitions
use Texinfo::Commands;

require Exporter;
use vars qw($VERSION @ISA @EXPORT_OK %EXPORT_TAGS);
@ISA = qw(Exporter);

%EXPORT_TAGS = ( 'all' => [ qw(
  convert_to_texinfo
  node_extra_to_texi
) ] );

@EXPORT_OK = ( @{ $EXPORT_TAGS{'all'} } );

$VERSION = '7.0.3';


my %brace_commands           = %Texinfo::Commands::brace_commands;
my %block_commands           = %Texinfo::Commands::block_commands;
my %def_commands             = %Texinfo::Commands::def_commands;

# used in root_heading_command_to_texinfo
my %sectioning_heading_commands = %Texinfo::Commands::sectioning_heading_commands;

my @ignored_types = ('spaces_inserted', 'bracketed_inserted',
'command_as_argument_inserted');
my %ignored_types;
for my $a (@ignored_types) {
  $ignored_types{$a} = 1;
}

# used to put a node name in error messages.
sub node_extra_to_texi($)
{
  my $node = shift;
  my $result = '';
  if ($node->{'manual_content'}) {
    $result = '('.convert_to_texinfo({'contents'
                              => $node->{'manual_content'}}) .')';
  }
  if ($node->{'node_content'}) {
    $result .= convert_to_texinfo({'contents' => $node->{'node_content'}});
  }
  return $result;
}

# for debugging.
sub root_heading_command_to_texinfo($)
{
  my $element = shift;
  my $tree;
  if ($element->{'cmdname'}) {
    if ($element->{'cmdname'} eq 'node') {
      $tree = $element->{'extra'}->{'node_content'};
    } elsif ($sectioning_heading_commands{$element->{'cmdname'}}
             and $element->{'args'}->[0]->{'contents'}) {
      $tree = $element->{'args'}->[0]->{'contents'};
    }
  } else {
    return "Not a root command";
  }
  return '@'.$element->{'cmdname'}.' '.convert_to_texinfo({'contents' => $tree})
          if ($tree);
  return '@'.$element->{'cmdname'};
}

# Following subroutines deal with transforming a texinfo tree into texinfo
# text.  Should give the text that was used parsed, except for a few cases.

# the second arguments, if defined triggers replaced
# tree item to be shown, in the default case they are
# not shown.
# expand a tree to the corresponding texinfo.
sub convert_to_texinfo($;$);
sub convert_to_texinfo($;$)
{
  my $element = shift;
  my $expand_replaced = shift;

  confess "convert_to_texinfo: element undef" if (!defined($element));
  confess "convert_to_texinfo: bad element type (".ref($element).") $element"
     if (ref($element) ne 'HASH');
  my $result = '';

  return '' if ($element->{'type'}
                and ($ignored_types{$element->{'type'}}
                     or ($element->{'type'} eq 'replaced'
                         and not $expand_replaced)));
  if (defined($element->{'text'})) {
    $result .= $element->{'text'};
  } else {
    if ($element->{'cmdname'}
       or ($element->{'type'} and ($element->{'type'} eq 'def_line'
                                or $element->{'type'} eq 'menu_entry'
                                or $element->{'type'} eq 'menu_comment'))) {
      $result .= _expand_cmd_args_to_texi($element, $expand_replaced);
    }
    if ($element->{'type'}
        and ($element->{'type'} eq 'bracketed'
             or $element->{'type'} eq 'bracketed_def_content')) {
      $result .= '{';
      if ($element->{'extra'}
          and $element->{'extra'}->{'spaces_before_argument'}) {
         $result .= $element->{'extra'}->{'spaces_before_argument'};
      }
    }
    if (defined($element->{'contents'})) {
      foreach my $child (@{$element->{'contents'}}) {
        $result .= convert_to_texinfo($child, $expand_replaced);
      }
    }
    if ($element->{'extra'} and $element->{'extra'}->{'spaces_after_argument'}) {
      $result .= $element->{'extra'}->{'spaces_after_argument'};
    }
    if ($element->{'extra'} and $element->{'extra'}->{'comment_at_end'}) {
      $result .= convert_to_texinfo($element->{'extra'}->{'comment_at_end'},
                         $expand_replaced);
    }
    $result .= '}' if ($element->{'type'}
                       and ($element->{'type'} eq 'bracketed'
                            or $element->{'type'} eq 'bracketed_def_content'));
  }
  return $result;
}

# expand a command argument as texinfo.
sub _expand_cmd_args_to_texi($;$) {
  my $cmd = shift;
  my $expand_replaced = shift;

  my $cmdname = $cmd->{'cmdname'};
  $cmdname = '' if (!$cmd->{'cmdname'});
  my $result = '';
  $result = '@'.$cmdname if ($cmdname);

  # this is done here otherwise for some constructs, there are
  # no 'args', and so the space is never readded.
  if ($cmd->{'extra'} and exists ($cmd->{'extra'}->{'spaces'})) {
    $result .= $cmd->{'extra'}->{'spaces'};
  }
  # block line commands with arguments not separated by commas
  if ($block_commands{$cmdname}
         and ($def_commands{$cmdname}
              or $block_commands{$cmdname} eq 'multitable')
         and $cmd->{'args'}) {
     $result .= $cmd->{'extra'}->{'spaces_before_argument'}
       if $cmd->{'extra'} and $cmd->{'extra'}->{'spaces_before_argument'};
     foreach my $arg (@{$cmd->{'args'}}) {
        $result .= convert_to_texinfo($arg, $expand_replaced);
    }
  # arg_line set for line_commands with type special
  } elsif (($cmd->{'extra'} or $cmdname eq 'macro' or $cmdname eq 'rmacro')
           and defined($cmd->{'extra'}->{'arg_line'})) {
    $result .= $cmd->{'extra'}->{'spaces_before_argument'}
      if $cmd->{'extra'} and $cmd->{'extra'}->{'spaces_before_argument'};
    $result .= $cmd->{'extra'}->{'arg_line'};
  } elsif (($block_commands{$cmdname} or $cmdname eq 'node')
            and defined($cmd->{'args'})) {
    $result .= $cmd->{'extra'}->{'spaces_before_argument'}
      if $cmd->{'extra'} and $cmd->{'extra'}->{'spaces_before_argument'};
    foreach my $arg (@{$cmd->{'args'}}) {
      next if $arg->{'type'} and $ignored_types{$arg->{'type'}};
      if ($arg->{'extra'} and $arg->{'extra'}->{'spaces_before_argument'}) {
        $result .= $arg->{'extra'}->{'spaces_before_argument'};
      }
      $result .= convert_to_texinfo($arg);
      $result .= ',';
    }
    $result =~ s/,$//;
  } elsif (defined($cmd->{'args'})) {
    my $braces;
    $braces = 1 if ($cmd->{'args'}->[0]->{'type'}
                    and ($cmd->{'args'}->[0]->{'type'} eq 'brace_command_arg'
                         or $cmd->{'args'}->[0]->{'type'} eq 'brace_command_context'));
    $result .= '{' if ($braces);
    if ($cmdname eq 'verb') {
      $result .= $cmd->{'extra'}->{'delimiter'};
    }
    if ($cmd->{'extra'}
        and $cmd->{'extra'}->{'spaces_before_argument'}) {
      $result .= $cmd->{'extra'}->{'spaces_before_argument'};
    }
    my $arg_nr = 0;
    foreach my $arg (@{$cmd->{'args'}}) {
      if (exists($brace_commands{$cmdname}) or ($cmd->{'type'}
                    and $cmd->{'type'} eq 'definfoenclose_command')) {
        $result .= ',' if ($arg_nr);
        $arg_nr++;
      }
      if ($arg->{'extra'} and $arg->{'extra'}->{'spaces_before_argument'}) {
        $result .= $arg->{'extra'}->{'spaces_before_argument'};
      }
      $result .= convert_to_texinfo($arg);
    }
    if ($cmdname eq 'verb') {
      $result .= $cmd->{'extra'}->{'delimiter'};
    }
    $result .= '}' if ($braces);
  } else {
    $result .= $cmd->{'extra'}->{'spaces_before_argument'}
      if $cmd->{'extra'} and $cmd->{'extra'}->{'spaces_before_argument'};
  }
  $result .= '{'.$cmd->{'extra'}->{'flag'}.'}' if ($cmdname eq 'value');
  return $result;
}

1;
__END__

=head1 NAME

Texinfo::Convert::Texinfo - Convert a Texinfo tree to Texinfo code

=head1 SYNOPSIS

  use Texinfo::Convert::Texinfo qw(convert_to_texinfo);
  
  my $texinfo_text = convert_to_texinfo($tree);

=head1 NOTES

The Texinfo Perl module main purpose is to be used in C<texi2any> to convert
Texinfo to other formats.  There is no promise of API stability.

=head1 DESCRIPTION

C<Texinfo::Convert::Texinfo> converts a Texinfo tree (described in
L<Texinfo::Parser>) to Texinfo code.  If the Texinfo tree results from
parsing some Texinfo document, The converted Texinfo code should be
exactly the same as the initial document, except that user defined @-macros
and C<@value> are expanded, and some invalid code is discarded.

=head1 METHODS

=over

=item $texinfo_text = convert_to_texinfo($tree)
X<C<convert_to_texinfo>>

Converts the Texinfo tree I<$tree> to Texinfo code.

=back

=head1 AUTHOR

Patrice Dumas, E<lt>pertusus@free.frE<gt>

=head1 COPYRIGHT AND LICENSE

Copyright 2010- Free Software Foundation, Inc.  See the source file for
all copyright years.

This library is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or (at
your option) any later version.

=cut
