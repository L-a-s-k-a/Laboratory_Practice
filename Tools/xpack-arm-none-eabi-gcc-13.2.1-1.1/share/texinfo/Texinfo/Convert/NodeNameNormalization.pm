# NodeNameNormalization.pm: output tree as normalized node name.
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

# the rules for conversion are decribed in the Texinfo manual, for
# HTML crossrefs in the 'HTML Xref' node.

package Texinfo::Convert::NodeNameNormalization;

use 5.00405;
use strict;

# stop \s from matching non-ASCII spaces, etc.  \p{...} can still be
# used to match Unicode character classes.
use if $] >= 5.014, re => '/a';

# To check if there is no erroneous autovivification
#no autovivification qw(fetch delete exists store strict);

use Unicode::Normalize;
use Text::Unidecode;

# commands classes
use Texinfo::Commands;
# for nobrace_symbol_text
use Texinfo::Common;
# use the hashes and functions
use Texinfo::Convert::Unicode;
# reuse some conversion hashes and ascii_accent function
use Texinfo::Convert::Text;

require Exporter;
use vars qw($VERSION @ISA @EXPORT_OK %EXPORT_TAGS);
@ISA = qw(Exporter);

%EXPORT_TAGS = ( 'all' => [ qw(
  normalize_node
  transliterate_texinfo
  transliterate_protect_file_name
) ] );

@EXPORT_OK = ( @{ $EXPORT_TAGS{'all'} } );

$VERSION = '7.0.3';


my %normalize_node_brace_no_arg_commands
  = %Texinfo::Convert::Text::text_brace_no_arg_commands;
foreach my $command (keys(%Texinfo::Convert::Unicode::unicode_character_brace_no_arg_commands)) {
  $normalize_node_brace_no_arg_commands{$command} =
     $Texinfo::Convert::Unicode::unicode_character_brace_no_arg_commands{$command};
}

my %normalize_node_nobrace_symbol_text
  = %Texinfo::Common::nobrace_symbol_text;
$normalize_node_nobrace_symbol_text{'*'} = ' ';

my %accent_commands = %Texinfo::Commands::accent_commands;

my %ignored_brace_commands;
foreach my $ignored_brace_command ('anchor', 'footnote', 'shortcaption',
       'caption', 'hyphenation', 'sortas', 'seealso', 'seeentry') {
  $ignored_brace_commands{$ignored_brace_command} = 1;
}

my %ignored_types;
foreach my $type ('ignorable_spaces_after_command', 'preamble_before_beginning',
            'spaces_at_end',
            'spaces_before_paragraph',
            'space_at_end_menu_node',
            'spaces_after_close_brace') {
  $ignored_types{$type} = 1;
}


sub normalize_node($)
{
  my $root = shift;
  my $result = _convert($root);
  $result = Unicode::Normalize::NFC($result);
  $result = _unicode_to_protected($result);
  $result = 'Top' if ($result =~ /^Top$/i);
  return $result;
}

sub transliterate_texinfo($;$)
{
  my $root = shift;
  my $no_unidecode = shift;
  my $result = _convert($root);
  $result = Unicode::Normalize::NFC($result);
  $result = _unicode_to_protected(
                _unicode_to_transliterate($result, $no_unidecode));
  return $result;
}

sub transliterate_protect_file_name($;$)
{
  my $input_text = shift;
  my $no_unidecode = shift;
  my $result = Unicode::Normalize::NFC($input_text);
  $result = _unicode_to_file_name(
                _unicode_to_transliterate($result, $no_unidecode));
  return $result;
}

sub convert_to_normalized($)
{
  my $root = shift;
  my $result = _convert($root);
}

sub _protect_unicode_char($)
{
  my $char = shift;
  if (exists($Texinfo::Convert::Unicode::unicode_simple_character_map{$char})) {
    return '_' . lc($Texinfo::Convert::Unicode::unicode_simple_character_map{$char});
  } else {
    if (ord($char) <= hex(0xFFFF)) {
      return '_' . lc(sprintf("%04x",ord($char)));
    } else {
      return '__' . lc(sprintf("%06x",ord($char)));
    }
  }
}

sub _unicode_to_protected($)
{
  my $text = shift;
  my $result = '';
  while ($text ne '') {
    if ($text =~ s/^([A-Za-z0-9]+)//) {
      $result .= $1;
    } elsif ($text =~ s/^ +//) {
      $result .= '-';
    # with /a some special spaces are not caught without /s, maybe because they are
    # considered as newlines
    } elsif ($text =~ s/^(.)//s) {
      $result .= _protect_unicode_char($1);
    } else {
      warn "Bug: unknown character _unicode_to_protected (likely in infinite loop)\n";
      print STDERR "Text: !!$text!!\n";
      sleep 1;
    }
  }
  return $result;
}

sub _unicode_to_file_name($)
{
  my $text = shift;
  my $result = '';
  while ($text ne '') {
    if ($text =~ s/^([A-Za-z0-9_\.\-]+)//) {
      $result .= $1;
    } elsif ($text =~ s/^ +//) {
      $result .= '-';
    # /s is specified to caught special spaces considered as newlines too
    } elsif ($text =~ s/^(.)//s) {
      $result .= _protect_unicode_char($1);
    } else {
      warn "Bug: unknown character _unicode_to_file_name (likely in infinite loop)\n";
      print STDERR "Text: !!$text!!\n";
      sleep 1;
    }
  }
  return $result;
}

sub _unicode_to_transliterate($;$)
{
  my $text = shift;
  my $no_unidecode = shift;
  if (chomp($text)) {
     warn "Bug: end of line to transliterate: $text\n";
  }
  my $result = '';
  while ($text ne '') {
    if ($text =~ s/^([A-Za-z0-9 ]+)//) {
      $result .= $1;
    } elsif ($text =~ s/^(.)//s) {
      my $char = $1;
      if (exists($Texinfo::Convert::Unicode::unicode_simple_character_map{$char})) {
        $result .= $char;
      } elsif (ord($char) <= hex(0xFFFF)
               and exists($Texinfo::Convert::Unicode::transliterate_map{uc(sprintf("%04x",ord($char)))})) {
        $result .= $Texinfo::Convert::Unicode::transliterate_map{uc(sprintf("%04x",ord($char)))};
      } elsif (ord($char) <= hex(0xFFFF)
               and exists($Texinfo::Convert::Unicode::diacritics_accent_commands{uc(sprintf("%04x",ord($char)))})) {
        $result .= '';
      # in this case, we want to avoid calling unidecode, as we are sure
      # that there is no useful transliteration of the unicode character
      # instead we want to keep it as is.
      # This is the case, for example, for @exclamdown, is corresponds
      # with x00a1, but unidecode transliterates it to a !, we want
      # to avoid that and keep x00a1.
      } elsif (ord($char) <= hex(0xFFFF)
               and exists($Texinfo::Convert::Unicode::no_transliterate_map{uc(sprintf("%04x",ord($char)))})) {
        $result .= $char;
      } else {
        if ($no_unidecode) {
          if (ord($char) <= hex(0xFFFF)
              and exists ($Texinfo::Convert::Unicode::transliterate_accent_map{uc(sprintf("%04x",ord($char)))})) {
            $result .= $Texinfo::Convert::Unicode::transliterate_accent_map{uc(sprintf("%04x",ord($char)))};
          } else {
            $result .= $char;
          }
        } else {
          $result .= unidecode($char);
        }
      }
      #print STDERR " ($no_unidecode) $text -> CHAR: ".ord($char)." ".uc(sprintf("%04x",ord($char)))."\n$result\n";
    } else {
      warn "Bug: unknown character _unicode_to_transliterate (likely in infinite loop)\n";
      print STDERR "Text: !!$text!!\n";
      sleep 1;
    }
  }
  return $result;
}


sub _convert($;$);

sub _convert($;$)
{
  my $element = shift;
  my $in_sc = shift;

  return '' if (($element->{'type'} and $ignored_types{$element->{'type'}})
          or ($element->{'cmdname'}
             and ($ignored_brace_commands{$element->{'cmdname'}}
             # here ignore the line commands
                 or ($element->{'args'} and $element->{'args'}->[0]
                     and $element->{'args'}->[0]->{'type'}
                     and ($element->{'args'}->[0]->{'type'} eq 'line_arg'
                         or $element->{'args'}->[0]->{'type'} eq 'misc_arg')))));
  my $result = '';
  if (defined($element->{'text'})) {
    $result = $element->{'text'};
    $result =~ s/\s+/ /g;
    $result = uc($result) if ($in_sc);
  }
  if ($element->{'cmdname'}) {
    my $command = $element->{'cmdname'};
    if (defined($normalize_node_nobrace_symbol_text{$element->{'cmdname'}})) {
      return $normalize_node_nobrace_symbol_text{$element->{'cmdname'}};
    } elsif (defined($normalize_node_brace_no_arg_commands{$element->{'cmdname'}})) {
      $command = $element->{'extra'}->{'clickstyle'}
         if ($element->{'extra'}
          and defined($element->{'extra'}->{'clickstyle'})
          and defined($normalize_node_brace_no_arg_commands{$element->{'extra'}->{'clickstyle'}}));
      my $result = $normalize_node_brace_no_arg_commands{$command};
      if ($in_sc and $Texinfo::Commands::letter_no_arg_commands{$command}) {
        $result = uc($result);
      }
      return $result;
    # commands with braces
    } elsif ($accent_commands{$element->{'cmdname'}}) {
      return '' if (!$element->{'args'});
      my $accent_text = _convert($element->{'args'}->[0]);
      my $accented_char
        = Texinfo::Convert::Unicode::unicode_accent($accent_text,
                                                    $element);
      if (!defined($accented_char)) {
        # In this case, the node normalization do not follow the specification,
        # but we cannot do better
        $accented_char = Texinfo::Convert::Text::ascii_accent($accent_text,
                                                              $element);
      }
      if ($in_sc) {
        return uc ($accented_char);
      } else {
        return $accented_char;
      }
    #} elsif ($element->{'cmdname'} eq 'image') {
    #  return _convert($element->{'args'}->[0]);
    } elsif ($Texinfo::Commands::ref_commands{$element->{'cmdname'}}) {
      my @args_try_order;
      if ($element->{'cmdname'} eq 'inforef') {
        @args_try_order = (0, 1, 2);
      } else {
        @args_try_order = (0, 1, 2, 4, 3);
      }
      foreach my $index (@args_try_order) {
        if (defined($element->{'args'}->[$index])) {
          my $text = _convert($element->{'args'}->[$index]);
          return $text if (defined($text) and $text =~ /\S/);
        }
      }
      return '';
    # Here all the commands with args are processed, if they have
    # more than one arg the first one is used.
    } elsif ($element->{'args'} and $element->{'args'}->[0]
           and (($element->{'args'}->[0]->{'type'}
                and $element->{'args'}->[0]->{'type'} eq 'brace_command_arg')
                or $element->{'cmdname'} eq 'math')) {
      my $sc = 1 if ($element->{'cmdname'} eq 'sc' or $in_sc);
      return _convert($element->{'args'}->[0], $sc);
    }
  }
  if ($element->{'contents'}) {
    foreach my $content (@{$element->{'contents'}}) {
      $result .= _convert($content, $in_sc);
    }
  }
  $result = '{'.$result.'}'
     if ($element->{'type'} and $element->{'type'} eq 'bracketed');
  return $result;
}

# Called from Texinfo::ParserNonXS and Texinfo::XS::parsetexi::Parsetexi.
# This should be considered an internal function of the parsers for all
# purposes, it is here to avoid code duplication.
# Sets $self->{'nodes'} and $self->{'labels'} based on $self->{'targets'}.
sub set_nodes_list_labels($$$)
{
  my $self = shift;
  my $registrar = shift;
  my $configuration_information = shift;

  $self->{'nodes'} = [];
  my %labels = ();
  if (defined $self->{'targets'}) {
    for my $target (@{$self->{'targets'}}) {
      if ($target->{'cmdname'} eq 'node') {
        if ($target->{'extra'}->{'nodes_manuals'}) {
          for my $node_manual (@{$target->{'extra'}{'nodes_manuals'}}) {
            if (defined $node_manual
                  and defined $node_manual->{'node_content'}) {
              my $normalized = Texinfo::Convert::NodeNameNormalization::normalize_node(
                                    {'contents' => $node_manual->{'node_content'}});
              $node_manual->{'normalized'} = $normalized;
            }
          }
        }
      }
      if (defined $target->{'extra'}
            and defined $target->{'extra'}->{'node_content'}) {
        my $normalized = Texinfo::Convert::NodeNameNormalization::normalize_node(
                             {'contents' => $target->{'extra'}->{'node_content'}});

        if ($normalized !~ /[^-]/) {
          $registrar->line_error($configuration_information,
               sprintf(__("empty node name after expansion `%s'"),
                     Texinfo::Convert::Texinfo::convert_to_texinfo({'contents'
                                   => $target->{'extra'}->{'node_content'}})),
                            $target->{'source_info'});
          delete $target->{'extra'}->{'node_content'};
        } else {
          if (defined $labels{$normalized}) {
            $registrar->line_error($configuration_information,
              sprintf(__("\@%s `%s' previously defined"),
                         $target->{'cmdname'},
                   Texinfo::Convert::Texinfo::convert_to_texinfo({'contents'
                                    => $target->{'extra'}->{'node_content'}})),
                               $target->{'source_info'});
            $registrar->line_error($configuration_information,
              sprintf(__("here is the previous definition as \@%s"),
                               $labels{$normalized}->{'cmdname'}),
                       $labels{$normalized}->{'source_info'});
            delete $target->{'extra'}->{'node_content'};
          } else {
            $labels{$normalized} = $target;
            $target->{'extra'}->{'normalized'} = $normalized;
            if ($target->{'cmdname'} eq 'node') {
              if ($target->{'extra'}
                  and $target->{'extra'}{'node_argument'}) {
                $target->{'extra'}{'node_argument'}{'normalized'}
                  = $normalized;
              }
              push @{$self->{'nodes'}}, $target;
            }
          }
        }
      } else {
        if ($target->{'cmdname'} eq 'node') {
          $registrar->line_error($configuration_information,
               sprintf(__("empty argument in \@%s"),
                  $target->{'cmdname'}), $target->{'source_info'});
          delete $target->{'extra'}->{'node_content'};
        }
      }
    }
  }
  $self->{'labels'} = \%labels;
}
1;

__END__

=head1 NAME

Texinfo::Convert::NodeNameNormalization - Normalize and transliterate Texinfo trees

=head1 SYNOPSIS

  use Texinfo::Convert::NodeNameNormalization qw(normalize_node
                                              transliterate_texinfo);

  my $normalized = normalize_node({'contents' => $node_contents});

  my $file_name = transliterate_texinfo({'contents'
                                            => $section_contents});

=head1 NOTES

The Texinfo Perl module main purpose is to be used in C<texi2any> to convert
Texinfo to other formats.  There is no promise of API stability.

=head1 DESCRIPTION

C<Texinfo::Convert::NodeNameNormalization> allows to normalize node names,
with C<normalize_node> following the specification described in the
Texinfo manual I<HTML Xref> node.  This is usefull each time one want a
unique identifier for Texinfo content that is only composed of letter,
digits, C<-> and C<_>.  In L<Texinfo::Parser>, C<normalize_node> is used
for C<@node>, C<@float> and C<@anchor> names normalization, but also C<@float>
types and C<@acronym> and C<@abbr> first argument.

It is also possible to transliterate non-ASCII letters, instead of mangling
them, with C<transliterate_texinfo>, losing the uniqueness feature of
normalized node names.

Another method, C<transliterate_protect_file_name> transliterates non-ASCII
letters and protect characters that should not appear on file names.

=head1 METHODS

=over

=item $partially_normalized = convert_to_normalized($tree)
X<C<convert_to_normalized>>

The Texinfo I<$tree> is returned as a string, with @-commands and spaces
normalized as described in the Texinfo manual I<HTML Xref> node.  ASCII
7-bit characters other than spaces and non-ASCII characters are left as
is in the resulting string.

=item $normalized = normalize_node($tree)
X<C<normalize_node>>

The Texinfo I<$tree> is returned as a string, normalized as described in the
Texinfo manual I<HTML Xref> node.

The result will be poor for Texinfo trees which are not @-command arguments
(on an @-command line or in braces), for instance if the tree contains
C<@node> or block commands.

=item $transliterated = transliterate_texinfo($tree, $no_unidecode)
X<C<transliterate_texinfo>>

The Texinfo I<$tree> is returned as a string, with non-ASCII letters
transliterated as ASCII, but otherwise similar with C<normalize_node>
output.  If the optional I<$no_unidecode> argument is set, C<Text::Unidecode>
is not used for characters whose transliteration is not built-in.

=item $file_name = transliterate_protect_file_name($string, $no_unidecode)
X<C<transliterate_protect_file_name>>

The string I<$string> is returned with non-ASCII letters transliterated as
ASCII, and ASCII characters not safe in file names protected as in
node normalization.  If the optional I<$no_unidecode> argument is set,
C<Text::Unidecode> is not used for characters whose transliteration is not
built-in.

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
