# TextContent.pm: return the text contents stripped of commands
#
# Copyright 2012-2018 Free Software Foundation, Inc.
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

package Texinfo::Convert::TextContent;

use 5.00405;
use strict;

# To check if there is no erroneous autovivification
#no autovivification qw(fetch delete exists store strict);

use Texinfo::Commands;
use Texinfo::Common;
use Texinfo::Convert::Utils;
use Texinfo::Convert::Text;
use Texinfo::Convert::Converter;

use vars qw($VERSION @ISA);
@ISA = qw(Texinfo::Convert::Converter);

$VERSION = '7.0.3';


my %ignored_brace_commands;
# Handle better @errormsg?
foreach my $ignored_brace_command ('hyphenation', 'errormsg') {
  $ignored_brace_commands{$ignored_brace_command} = 1;
}
my %ignored_block_commands;
foreach my $ignored_command (
  'html', 'tex', 'xml', 'docbook', 'ignore', 'macro', 'rmacro') {
  $ignored_block_commands{$ignored_command} = 1;
}

my %ignored_types;
foreach my $type ('ignorable_spaces_after_command', 'preamble_before_beginning',
            'spaces_before_paragraph',
            'spaces_after_close_brace') {
  $ignored_types{$type} = 1;
}

my %defaults = (
  'FORMAT_MENU'          => 'menu',
  # different from the default, which is undef
  'OUTFILE'              => '-',
);

sub converter_defaults($$)
{
  return %defaults;
}

sub converter_initialize($)
{
  my $self = shift;
  
  %{$self->{'formatted_line_commands'}}
    = %Texinfo::Common::formatted_line_commands;
  if ($self->get_conf('TEXTCONTENT_COMMENT')) {
    $self->{'formatted_line_commands'}->{'c'} = 1;
    $self->{'formatted_line_commands'}->{'comment'} = 1;
  }
}

sub convert_tree($$)
{
  my $self = shift;
  my $root = shift;

  return $self->_convert($root);
}

sub convert($$)
{
  my $self = shift;
  my $root = shift;

  return $self->_convert($root);
}

sub _convert($$);

sub _convert($$)
{
  my $self = shift;
  my $element = shift;

  return '' if (!($element->{'type'} and $element->{'type'} eq 'def_line')
     and (($element->{'type'} and $ignored_types{$element->{'type'}})
          or ($element->{'cmdname'}
             and ($ignored_brace_commands{$element->{'cmdname'}}
                 or ($ignored_block_commands{$element->{'cmdname'}}
                     and !(defined($self->{'expanded_formats_hash'})
                           and $self->{'expanded_formats_hash'}->{$element->{'cmdname'}}))
                 or ($Texinfo::Common::inline_format_commands{$element->{'cmdname'}}
                     and (!$element->{'extra'}->{'format'}
                          or !$self->{'expanded_formats_hash'}->{$element->{'extra'}->{'format'}}))
                 or ($element->{'cmdname'} eq 'menu' and $self->get_conf('FORMAT_MENU') eq 'nomenu')
             # here ignore most of the line commands
                 or ($element->{'args'} and $element->{'args'}->[0]
                     and $element->{'args'}->[0]->{'type'}
                     and ($element->{'args'}->[0]->{'type'} eq 'line_arg'
                         or $element->{'args'}->[0]->{'type'} eq 'misc_arg')
                     and !$self->{'formatted_line_commands'}->{$element->{'cmdname'}})))));
  if (defined($element->{'text'})) {
    return $element->{'text'};
  }
  if (defined($element->{'cmdname'})) {
    if (exists($Texinfo::Common::nobrace_symbol_text{$element->{'cmdname'}})) {
      return $Texinfo::Common::nobrace_symbol_text{$element->{'cmdname'}};
    } elsif ($element->{'cmdname'} eq 'today') {
      if ($self->get_conf('TEST')) {
        return 'a sunny day';
      }
      my($sec, $min, $hour, $mday, $mon, $year, $wday, $yday, $isdst)
        = localtime(time);
      $year += ($year < 70) ? 2000 : 1900;
      return "$Texinfo::Convert::Utils::MONTH_NAMES[$mon] $mday, $year";
    } elsif (defined($Texinfo::Convert::Text::text_brace_no_arg_commands{$element->{'cmdname'}})) {
      return Texinfo::Convert::Text::brace_no_arg_command($element, undef);
    } elsif ($Texinfo::Commands::accent_commands{$element->{'cmdname'}}) {
      my %options = Texinfo::Convert::Text::copy_options_for_convert_text($self);
      my $result = Texinfo::Convert::Text::text_accents($element,
                                        $options{'enabled_encoding'});
      return $result;
    }
  }
  my $result = '';
  if ($element->{'args'}
      and (!$element->{'cmdname'}
           or !$Texinfo::Commands::blockitem_commands{$element->{'cmdname'}})) {
    my $args;
    if ($element->{'cmdname'}
        and $Texinfo::Common::inline_format_commands{$element->{'cmdname'}}) {
      my @args = @{$element->{'args'}};
      shift @args;
      $args = \@args;
    } else {
      $args = $element->{'args'};
    }
    foreach my $arg (@{$args}) {
      $result .= _convert($self, $arg);
      if ($arg->{'type'}
          and $arg->{'type'} eq 'block_line_arg'
          and $arg->{'extra'} and $arg->{'extra'}->{'spaces_after_argument'}
          and $result =~ /\S/) {
        $result .= $arg->{'extra'}->{'spaces_after_argument'};
      }
    }
  }
  if ($element->{'contents'}) {
    foreach my $content (@{$element->{'contents'}}) {
      $result .= _convert($self, $content);
    }
  }
  $result = '{'.$result.'}'
     if ($element->{'type'} and $element->{'type'} eq 'bracketed'
         and (!$element->{'parent'}->{'type'} or
              ($element->{'parent'}->{'type'} ne 'block_line_arg'
               and $element->{'parent'}->{'type'} ne 'line_arg')));

  if ($element->{'type'} and $element->{'type'} eq 'line_arg'
      # in that case there is already a 'spaces_at_end'
      and not ($element->{'parent'}->{'type'}
               and $element->{'parent'}->{'type'} eq 'def_line')) {
    $result .= "\n";
  }

  return $result;
}

1;
